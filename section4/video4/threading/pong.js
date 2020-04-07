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
        render();
    });
}

function setupGameState() { 
    MOVE = Module.Move; 
    name = document.getElementById("name").value;
    gameState = Module.createInitialGameState(name);

    timings = new Array(1000);

    move = MOVE.STATIONARY;

    fps = 0;
    frames = 0;
    totalFrames = 0
    time = Date.now(); 


    window.addEventListener("keydown", function(event) {
        key = Number(event.keyCode);
        if(key == 40) { // down arrow
            gameState.move = MOVE.DOWN;
        }
        if(key == 38) { // up arrow
            gameState.move = MOVE.UP;
        }
    });

    window.addEventListener("keyup", function(event) {
        gameState.move = MOVE.STATIONARY;
    });
}
function render(){
    lastTiming = Date.now()
    calculateFps();
    update();
    draw();
    timings[totalFrames%timings.length] = Date.now() - lastTiming 
    this.setTimeout(render, 0);
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

function update() {
    gameState = Module.updatePosition(gameState);
}

function draw() {
  var ctx = document.getElementById('canvas').getContext('2d');
  drawBlackBackground(ctx);
  drawDottedLine(ctx);
  drawPaddles(ctx);
  drawBall(ctx);
  drawScore(ctx);
  drawFps(ctx);
  drawTimings(ctx);
}

function drawScore(ctx){
  ctx.font = "60px Arial";
  ctx.strokeStyle = RED;
  ctx.strokeText(gameState.leftScore, 250, 50);
  ctx.strokeStyle = BLUE;
  ctx.strokeText(gameState.rightScore, 550, 50);
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

function drawPaddles(ctx) {
    ctx.fillStyle = RED;
    ctx.fillRect(gameState.left.xpos, gameState.left.ypos - 50, 25, 100);
    ctx.fillStyle = BLUE;
    ctx.fillRect(gameState.right.xpos, gameState.right.ypos - 50, 25, 100);
}

function drawBall(ctx) {
    ctx.fillStyle = WHITE;
    ctx.fillRect(gameState.ball.xpos, gameState.ball.ypos, 10, 10);
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
