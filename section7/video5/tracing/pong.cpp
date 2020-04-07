#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
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
#include <emscripten/trace.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>


SDL_Window* window;
SDL_Renderer* renderer;

TTF_Font* font;
Mix_Chunk *wave;
Mix_Music *music;


int fps = 0;
int frames = 0;
int totalFrames = 0;
std::array<int, 1000> timings;


std::chrono::milliseconds getCurrentTime() { 
    return std::chrono::duration_cast< std::chrono::milliseconds >(
        std::chrono::system_clock::now().time_since_epoch());

}
std::chrono::milliseconds lastTime = getCurrentTime();
std::chrono::milliseconds lastTiming = getCurrentTime();

void calculateFps() {
    frames += 1;
    totalFrames += 1;
    auto now = getCurrentTime();
    if (now.count() - lastTime.count() > 1000) {
        fps = frames/((now.count() - lastTime.count()) / 1000);
        frames = 0;
        lastTime = now;
    }
}


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
        
        Mix_PlayChannel(-1, wave, 0);
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

void drawPaddles(SDL_Renderer* renderer, const Paddle& left, const Paddle& right) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect leftRect = {(int)left.xpos, (int)left.ypos - 50, 25, 100};
    SDL_RenderFillRect(renderer, &leftRect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect rightRect = {(int)right.xpos, (int)right.ypos - 50, 25, 100};
    SDL_RenderFillRect(renderer, &rightRect);
}

void drawBall(SDL_Renderer* renderer, const Ball& ball) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect ballRect = {(int)ball.xpos, (int)ball.ypos , 10, 10};
    SDL_RenderFillRect(renderer, &ballRect);
}

void drawDottedLine(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for(int i = 0; i < 20; ++i){
        SDL_Rect rect = { 397, i*30+7, 5, 15};
        SDL_RenderFillRect(renderer, &rect);
    }
}

void drawDebugInformation(SDL_Renderer* renderer) {
    auto fpsSurface = TTF_RenderText_Blended(font, (std::to_string(fps) + " Frames / Second").c_str(), {255, 255, 255, 255});
    auto fpsTexture = SDL_CreateTextureFromSurface(renderer, fpsSurface);
    SDL_Rect fpsRect = {650, 560, 150, 20};
    SDL_RenderCopy(renderer, fpsTexture, nullptr, &fpsRect);

    auto averageTiming = std::accumulate(timings.begin(), timings.end(), 0.0) / timings.size();
    auto timingSurface = TTF_RenderText_Blended(font, (std::to_string(averageTiming) + " ms render").c_str(), {255, 255, 255, 255});
    auto timingTexture = SDL_CreateTextureFromSurface(renderer, timingSurface);
    SDL_Rect timingRect = {650, 580, 150, 20};
    SDL_RenderCopy(renderer, timingTexture, nullptr, &timingRect);
    
    SDL_FreeSurface(fpsSurface);
    SDL_FreeSurface(timingSurface);

}

void drawScores(SDL_Renderer* renderer, int leftScore, int rightScore) {
    auto leftSurface = TTF_RenderText_Blended(font, std::to_string(leftScore).c_str(), {255, 0, 0, 255});
    auto leftTexture = SDL_CreateTextureFromSurface(renderer, leftSurface);
    SDL_Rect leftScoreRect = {250, 20, 50, 50};
    SDL_RenderCopy(renderer, leftTexture, nullptr, &leftScoreRect);

    auto rightSurface = TTF_RenderText_Blended(font, std::to_string(rightScore).c_str(), {0, 0, 255, 255});
    auto rightTexture = SDL_CreateTextureFromSurface(renderer, rightSurface);
    SDL_Rect rightScoreRect = {550, 20, 50, 50};
    SDL_RenderCopy(renderer, rightTexture, nullptr, &rightScoreRect);

    SDL_FreeSurface(leftSurface);
    SDL_FreeSurface(rightSurface);

}

void render(const GameState& gameState) {
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    drawPaddles(renderer, gameState.left, gameState.right);
    drawBall(renderer, gameState.ball);
    drawDottedLine(renderer);
    drawScores(renderer, gameState.leftScore, gameState.rightScore);
    drawDebugInformation(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

Move getMove(){
    SDL_Event event;
    static Move move = Move::STATIONARY;
    if( SDL_PollEvent( &event ) ){
        if( event.type == SDL_KEYDOWN ){
            if (event.key.keysym.sym == SDLK_UP) {
                move = Move::UP;
            }
            if (event.key.keysym.sym == SDLK_DOWN) {
                move = Move::DOWN;
            }
            if(event.key.keysym.sym == SDLK_SPACE) {
                if (!Mix_PausedMusic()){
                    Mix_PauseMusic();
                }
                else {
                    Mix_ResumeMusic();
                }
            }
        }

        if (event.type == SDL_KEYUP) {
            move = Move::STATIONARY;
        }
    }
    return move;
}

void loopIteration(void* gameStateVoidPtr) {
    emscripten_trace_record_frame_start();
    if(gameStateVoidPtr){

        GameState& gameState = *reinterpret_cast<GameState*>(gameStateVoidPtr);
        lastTiming = getCurrentTime();
        calculateFps();
        gameState.move = getMove(); 
        emscripten_trace_enter_context("UpdatePosition");
        updatePosition(gameState);
        emscripten_trace_exit_context();
        emscripten_trace_enter_context("Render");
        render(gameState);
        emscripten_trace_exit_context();
        timings[totalFrames%timings.size()] = getCurrentTime().count() - lastTiming.count();
    }
    if(frames % 60 == 0) {
        emscripten_trace_report_memory_layout();
        emscripten_trace_report_off_heap_data();
    }
    emscripten_trace_record_frame_end();
}

void mainLoop(const std::string& name){
    static GameState gameState = createInitialGameState(name);
    emscripten_trace_configure("http://127.0.0.1:5000/", "Pong");
    if( SDL_Init( SDL_INIT_VIDEO ) < 0){
        std::cerr << "Can't init video " << SDL_GetError() << "\n";
    }
    TTF_Init();
    font = TTF_OpenFont("fonts/Arial.ttf", 30);
    emscripten_trace_annotate_address_type(font, "TTF_FONT");
    
    SDL_Init(SDL_INIT_AUDIO);

    Mix_OpenAudio( 22050, AUDIO_F32, 2, 4096 );
    wave = Mix_LoadWAV("sounds/beep.wav");
    emscripten_trace_annotate_address_type(wave, "WAV file");
    music = Mix_LoadMUS("sounds/music.ogg");
    emscripten_trace_annotate_address_type(music, "music file");
    Mix_FadeInMusic(music, -1, 10000);

    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_BORDERLESS, &window, &renderer);
    SDL_RenderSetLogicalSize(renderer, 800, 600);
    emscripten_set_main_loop_arg(loopIteration, &gameState, 60, false);
}


EMSCRIPTEN_BINDINGS(gameState) {

    emscripten::function("mainLoop", &mainLoop);
}
