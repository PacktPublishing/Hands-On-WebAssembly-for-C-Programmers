const BLACK = "rgb(0,0,0)";
const RED = "rgb(255,0,0)";
const BLUE = "rgb(0,0,255)";
const WHITE = "rgb(255,255,255)";


function buttonClick() {
    FS.mkdir('/data');
    FS.mount(IDBFS, {}, '/data');
    FS.syncfs(true, function(){
        setupGameState();
        document.getElementById("name").remove();
        document.getElementById("btn").remove();
        Module.mainLoop(name);
    });
}

function setupGameState() { 
    MOVE = Module.Move; 
    name = document.getElementById("name").value;

    timings = new Array(1000);

    move = MOVE.STATIONARY;

    fps = 0;
    frames = 0;
    totalFrames = 0
    time = Date.now(); 


    window.addEventListener("keydown", function(event) {
        key = Number(event.keyCode);
        if(key == 40) { // down arrow
            move = MOVE.DOWN;
        }
        if(key == 38) { // up arrow
            move = MOVE.UP;
        }
    });

    window.addEventListener("keyup", function(event) {
        move = MOVE.STATIONARY;
    });
}

function calculateFps() {
    frames += 1;
    totalFrames += 1;
    if (Date.now() - time > 1000) {
        fps = frames/((Date.now() - time) / 1000)
        frames = 0;
        time = Date.now()
    }
}


function draw(leftXpos, leftYpos, rightXpos, rightYpos, leftScore, rightScore, ballXpos, ballYpos) {
  var ctx = document.getElementById('canvas').getContext('2d');
  drawBlackBackground(ctx);
  drawDottedLine(ctx);
  drawPaddles(ctx, leftXpos, leftYpos, rightXpos, rightYpos);
  drawBall(ctx, ballXpos, ballYpos);
  drawScore(ctx, leftScore, rightScore);
  drawFps(ctx);
  drawTimings(ctx);
}

function drawScore(ctx, leftScore, rightScore){
  ctx.font = "60px Arial";
  ctx.strokeStyle = RED;
  ctx.strokeText(leftScore, 250, 50);
  ctx.strokeStyle = BLUE;
  ctx.strokeText(rightScore, 550, 50);
}

function drawBlackBackground(ctx) {
    ctx.fillStyle = BLACK;
    ctx.fillRect(0, 0, 800, 600);
}

function drawDottedLine(ctx) {
    ctx.strokeStyle = WHITE;
    ctx.beginPath();
    ctx.setLineDash([5, 15]);
    ctx.moveTo(400, 0);
    ctx.lineTo(400, 600);
    ctx.lineWidth = 10;
    ctx.stroke();

    //reset line
    ctx.setLineDash([]);
    ctx.lineWidth = 1;
}

function drawPaddles(ctx, leftXpos, leftYpos, rightXpos, rightYpos) {
    ctx.fillStyle = RED;
    ctx.fillRect(leftXpos, leftYpos - 50, 25, 100);
    ctx.fillStyle = BLUE;
    ctx.fillRect(rightXpos, rightYpos - 50, 25, 100);
}

function drawBall(ctx, ballXpos, ballYpos) {
    ctx.fillStyle = WHITE;
    ctx.fillRect(ballXpos, ballYpos, 10, 10);
}

function drawFps(ctx) { 
  ctx.font = "16px Arial";
  ctx.fillStyle = WHITE;
  ctx.fillText(fps.toFixed() + " Frames/Second", 650, 560);
}

function drawTimings(ctx) { 
  ctx.font = "16px Arial";
  ctx.fillStyle = WHITE;
  timing = timings.reduce( (a,b) => {return a + b}, 0) / timings.length;
  ctx.fillText(timing.toFixed(2) + " ms render", 650, 580);
}
