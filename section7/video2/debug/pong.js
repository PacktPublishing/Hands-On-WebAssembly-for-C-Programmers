function buttonClick() {
    FS.mkdir('/data');
    FS.mount(IDBFS, {}, '/data');
    FS.syncfs(true, function(){
        name = document.getElementById("name").value;
        document.getElementById("name").remove();
        document.getElementById("btn").remove();
        Module.mainLoop(name);
    });
}
