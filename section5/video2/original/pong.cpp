#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

#ifdef __EMSCRIPTEN_PTHREADS__
#include <chrono>
#include <mutex>
#include <queue>
#include <thread>

std::thread t1;
#endif

#include "boost/date_time/gregorian/gregorian.hpp"

#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/fetch.h>

#include <SDL2/SDL.h>

SDL_Window* window;
SDL_Renderer* renderer;

EM_JS(void, syncFS, (), {
    FS.syncfs(function(){});
})

void postToLeaderboard(const std::string& name, int leftScore, int rightScore) {
    static char buffer[64];
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;


    std::string data = std::to_string(leftScore) + " " + std::to_string(rightScore);
    strncpy(buffer, data.c_str(), 63);
    buffer[63] = '\0';
    attr.requestData = buffer; 
    attr.requestDataSize = strlen(buffer); 

    emscripten_fetch(&attr, std::string("gameScore/"+name).c_str());
}

class DebugLog {

public:
    #ifdef __EMSCRIPTEN_PTHREADS__
    DebugLog() {
        t1 = std::thread(&DebugLog::waitForMessagesToLog, this);
    }

    void waitForMessagesToLog(){
        while(true) {
            {
                std::unique_lock lck(messages_mutex);
                if(!messages.empty()){
                    auto [str, name] = messages.front();
                    messages.pop();
                    lck.unlock();
                    _log(str, name);
                }
            }
            using namespace std::literals::chrono_literals;
            std::this_thread::sleep_for(500ms);
        }
    }
    #endif

    void logMessage(const std::string & str, const std::string& name) {
        #ifdef __EMSCRIPTEN_PTHREADS__
            std::lock_guard g(messages_mutex);
            messages.push({str, name});
        #else
            _log(str, name);
            syncFS();
        #endif
    }

    void _log(const std::string & str, const std::string& name) {
            const auto fileName = "/data/gameScore/" + name;
            std::filesystem::remove(fileName);
            std::ofstream of(fileName, std::ofstream::out);
            of << str;
    }

private:
    #ifdef __EMSCRIPTEN_PTHREADS__
    std::queue<std::pair<std::string, std::string>> messages;
    std::mutex messages_mutex;
    #endif


};

DebugLog debugLog;


EM_JS(void, drawCanvas, (int height, int width), {
    body = document.getElementsByTagName("body")[0];
    canvas = document.createElement("canvas");
    body.appendChild(canvas);
    canvas.setAttribute("id", "canvas");
    Module.canvas = canvas;
});

struct Paddle {
    float xpos;
    float ypos;
    
    void moveUp() {
        if (ypos > 51) {
            ypos -= 2;
        }
    }
    void moveDown() {
        if (ypos < 549) {
            ypos += 2;
        }
    }

    bool isAtPaddleLevel(float ypos) {
        return (ypos - 5 < this->ypos + 50) && (ypos + 5 > this->ypos - 50);
    }

};

struct Ball {
    float xpos = 395;
    float ypos = 295;
    float xspeed = 3;
    float yspeed = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    bool isAtTopOrBottom(){
        return ypos - 5 < 0 || ypos + 5 > 595;
    }
    
    bool scoresOnRight() {
        return xpos > 775;
    }

    bool scoresOnLeft() {
        return xpos < 25 ;
    }

    bool doesHitPaddle(Paddle left, Paddle right) {
        return (xpos - 5 < 50 && left.isAtPaddleLevel(ypos)) || (xpos + 5 > 750 && right.isAtPaddleLevel(ypos)); 
    }

    void update() {
        xpos += xspeed;
        ypos += yspeed;
    }
};


enum class Move{
    STATIONARY = 0,
    UP,
    DOWN
};

struct GameState {
    std::string name;
    int leftScore;
    int rightScore;
    Ball ball{};
    Paddle left{25, 300};
    Paddle right{750, 300}; 
    Move move = Move::STATIONARY;
};



float calculateReflectionFactor(Ball ball, Paddle left, Paddle right){
    Paddle paddle = (ball.xpos > 400) ? right : left;
    const float factor = ball.ypos - paddle.ypos;
    return factor / 100;
}


void makeAIMove(Ball ball, Paddle& leftPaddle) {
    int idealPosition = ball.ypos;
    if(ball.xspeed <= 0) { 
        float turns = (ball.xpos - 50) / (-1 * ball.xspeed);
        idealPosition = ball.ypos + (ball.yspeed * turns);
    }

    if(idealPosition < leftPaddle.ypos) {
        leftPaddle.moveUp();
    }
    if(idealPosition > leftPaddle.ypos) {
        leftPaddle.moveDown(); 
    }
}

std::pair<int, int> getScores(const std::string& name){
    
    const std::string fileName("/data/gameScore/" + name);
    if(std::filesystem::exists(fileName)){
        std::ifstream ifs(fileName, std::ifstream::in);
        int leftScore, rightScore;
        ifs >> leftScore >> rightScore;
        return {leftScore, rightScore};
    }
    return {0,0};
}


GameState createInitialGameState(const std::string& name){
    auto [leftScore, rightScore] = getScores(name);
    drawCanvas(600, 800);
    return GameState{name, leftScore, rightScore};
}


void persistScore(const GameState& gameState) {
    if(!std::filesystem::exists("/data/gameScore")){
        std::filesystem::create_directory("/data/gameScore");
    }

    std::stringstream ss;
    ss << gameState.leftScore << " " << gameState.rightScore; 

    using namespace boost::gregorian;
    std::string ud("20200220");
    date d1(from_undelimited_string(ud));
    ss << "\n" << d1 << "\n" << std::flush;
    
    debugLog.logMessage(ss.str(), gameState.name);
    postToLeaderboard(gameState.name, gameState.leftScore, gameState.rightScore);
}

void updatePosition(GameState& gameState) {


    if(gameState.ball.xspeed == 0 && gameState.ball.yspeed == 0){
        gameState.ball.xspeed = 1;
    }
    if (gameState.move == Move::UP) {
        gameState.right.moveUp();
    }
    if (gameState.move == Move::DOWN) {
        gameState.right.moveDown();
    }
    
    makeAIMove(gameState.ball, gameState.left);
    
    if (gameState.ball.isAtTopOrBottom()) {
        gameState.ball.yspeed = -gameState.ball.yspeed;
    }
    if (gameState.ball.doesHitPaddle(gameState.left, gameState.right)){
        gameState.ball.xspeed = -gameState.ball.xspeed;
        gameState.ball.xspeed *= 1.05;
        gameState.ball.yspeed += calculateReflectionFactor(gameState.ball, gameState.left, gameState.right);
    }

    if (gameState.ball.scoresOnRight()){
        gameState.ball = Ball{};
        gameState.leftScore += 1;
        persistScore(gameState); 
    }
    if (gameState.ball.scoresOnLeft()){
        gameState.ball = Ball{};
        gameState.rightScore += 1;
        persistScore(gameState);
    }
    gameState.ball.update();

    #if __EMSCRIPTEN_PTHREADS__
    syncFS();
    #endif
    
}


EM_JS(int, preUpdate, (), { 
    lastTiming = Date.now();
    calculateFps();
    return move.value;
})

EM_JS(void, postUpdate, (int leftXpos, int leftYpos,
                         int rightXpos, int rightYpos,
                         int leftScore, int rightScore,
                         float ballXpos, float ballYpos), {
    draw(leftXpos, leftYpos, rightXpos, rightYpos, leftScore, rightScore, ballXpos, ballYpos);
    timings[totalFrames%timings.length] = Date.now() - lastTiming;
})


void loopIteration(void* gameStateVoidPtr) {
    if(gameStateVoidPtr){
        GameState& gameState = *reinterpret_cast<GameState*>(gameStateVoidPtr);
        gameState.move = Move(preUpdate());
        updatePosition(gameState);
        postUpdate(gameState.left.xpos, gameState.left.ypos,
            gameState.right.xpos, gameState.right.ypos,
            gameState.leftScore, gameState.rightScore,
            gameState.ball.xpos, gameState.ball.ypos);
    }
}

void mainLoop(const std::string& name){
    static GameState gameState = createInitialGameState(name);
    if( SDL_Init( SDL_INIT_VIDEO ) < 0){
        std::cerr << "Can't init video " << SDL_GetError() << "\n";
    }
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_BORDERLESS, &window, &renderer);
    SDL_RenderSetLogicalSize(renderer, 800, 600);
    emscripten_set_main_loop_arg(loopIteration, &gameState, 60, false);
}


EMSCRIPTEN_BINDINGS(gameState) {

    emscripten::class_<Ball>("Ball")
	.constructor<>()
        .property("xpos", &Ball::xpos)
        .property("ypos", &Ball::ypos)
        .property("xspeed", &Ball::xspeed)
        .property("yspeed", &Ball::yspeed)
        ;

    emscripten::value_object<Paddle>("Paddle")
        .field("xpos", &Paddle::xpos)
        .field("ypos", &Paddle::ypos)
        ;

    emscripten::enum_<Move>("Move")
        .value("STATIONARY", Move::STATIONARY)
        .value("UP", Move::UP)
        .value("DOWN", Move::DOWN);

    emscripten::value_object<GameState>("GameState")
        .field("ball", &GameState::ball)
        .field("left", &GameState::left)
        .field("right", &GameState::right)
        .field("move", &GameState::move)
        .field("leftScore", &GameState::leftScore)
        .field("rightScore", &GameState::rightScore)
        .field("name", &GameState::name)
        ;

    emscripten::function("mainLoop", &mainLoop);
}
