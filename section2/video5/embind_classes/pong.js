const BLACK = "rgb(0,0,0)";
const RED = "rgb(255,0,0)";
const BLUE = "rgb(0,0,255)";
const WHITE = "rgb(255,255,255)";


function setupGameState() {
	ball = new Module.Ball();

	leftPaddle = {
	    xpos: 25,
	    ypos: 300
	}

	rightPaddle = {
	    xpos: 750,
	    ypos: 300
	};

    MOVE = Module.Move
    move = MOVE.STATIONARY;
    timings = new Array(1000);

    fps = 0;
    frames = 0;
    totalFrames = 0
    time = Date.now(); 

    leftScore = 0;
    rightScore = 0;


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
    if (ball.xspeed === 0 && ball.yspeed ===0) {
        ball.xspeed = 1; 
        ball.yspeed = (Math.random() * 2) - 1; 
    }
    
    if (move === MOVE.UP && rightPaddle.ypos > 50) {
        rightPaddle.ypos -= 1; 
    }
    if (move === MOVE.DOWN && rightPaddle.ypos < 550) {
        rightPaddle.ypos += 1; 
    }
    
    const AIMove = Module.getAIMove(ball, leftPaddle);
    if (AIMove === MOVE.UP && leftPaddle.ypos > 50) {
        leftPaddle.ypos -= 1; 
    }
    if (AIMove === MOVE.DOWN && leftPaddle.ypos < 550) {
        leftPaddle.ypos += 1; 
    }

    if (ballCollidesWithTopOrBottom()){
        ball.yspeed = -ball.yspeed;
    }
    if (ballCollidesWithPaddle()){
        ball.xspeed = -ball.xspeed;
        ball.xspeed *= 1.05;
        ball.yspeed += calculateReflectionFactor();
    }
    if (ballScoresOnRight()){
        ball.delete();
        ball = new Module.Ball();
        leftScore += 1;
    }
    if (ballScoresOnLeft()){
        ball.delete();
        ball = new Module.Ball();
        rightScore += 1;
    }
    ball.xpos += ball.xspeed;
    ball.ypos += ball.yspeed;
}
function ballCollidesWithTopOrBottom() {
    return ball.ypos - 5 < 0 || ball.ypos + 5 > 595;
}

function ballCollidesWithPaddle() {
    return ballCollidesWithLeftPaddle() || ballCollidesWithRightPaddle();
}

function ballCollidesWithLeftPaddle() {
    return (ball.xpos - 5 < leftPaddle.xpos + 25 && 
            ball.ypos - 5 < leftPaddle.ypos + 50 && 
            ball.ypos + 5 > leftPaddle.ypos - 50);
}

function ballCollidesWithRightPaddle() {
    return (ball.xpos + 5 > rightPaddle.xpos &&
            ball.ypos - 5 < rightPaddle.ypos + 50 &&
            ball.ypos + 5 > rightPaddle.ypos - 50);
}

function ballScoresOnRight() {
    return ball.xpos > 775;
}

function ballScoresOnLeft() {
    return ball.xpos < 25 ;
}

function calculateReflectionFactor(){
    paddle = leftPaddle;
    if(ball.xpos > 400) {
        paddle = rightPaddle;
    }

    const factor = ball.ypos - paddle.ypos;
    return factor / 100;
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

function drawPaddles(ctx) {
    ctx.fillStyle = RED;
    ctx.fillRect(leftPaddle.xpos, leftPaddle.ypos - 50, 25, 100);
    ctx.fillStyle = BLUE;
    ctx.fillRect(rightPaddle.xpos, rightPaddle.ypos - 50, 25, 100);
}

function drawBall(ctx) {
    ctx.fillStyle = WHITE;
    ctx.fillRect(ball.xpos, ball.ypos, 10, 10);
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
