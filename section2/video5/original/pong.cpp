#include <emscripten.h>
#include <emscripten/bind.h>

enum Move {
    STATIONARY = 0,
    UP = 1,
    DOWN = 2
};


int getAIMove(float ball_xspeed, float ball_yspeed,
              float ball_xpos, float ball_ypos,
              int paddle_ypos) {
    int idealPosition = ball_ypos;
    if(ball_xspeed <= 0){
        auto turns = (ball_xpos - 50) / (-1 * ball_xspeed);
        idealPosition = ball_ypos + (ball_yspeed * turns);
    }


    if(idealPosition > paddle_ypos){
        return Move::DOWN;
    }
    if(idealPosition < paddle_ypos){
        return Move::UP;
    }
    return Move::STATIONARY;
}

EMSCRIPTEN_BINDINGS(pong) {
    emscripten::function("getAIMove", &getAIMove);
}
