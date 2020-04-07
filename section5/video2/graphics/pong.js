const BLACK = "rgb(0,0,0)";
const RED = "rgb(255,0,0)";
const BLUE = "rgb(0,0,255)";
const WHITE = "rgb(255,255,255)";


function buttonClick() {
    FS.mkdir('/data');
    FS.mount(IDBFS, {}, '/data');
    FS.syncfs(true, function(){
        setupGameState();
        name = document.getElementById("name").value;
        document.getElementById("name").remove();
        document.getElementById("btn").remove();
        Module.mainLoop(name);
    });
}

function setupGameState() { 
    MOVE = Module.Move;
    move = MOVE.STATIONARY;

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
