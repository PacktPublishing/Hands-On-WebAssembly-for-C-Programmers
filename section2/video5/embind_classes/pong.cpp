#include <cstdlib>
#include <emscripten.h>
#include <emscripten/bind.h>

enum class Move {
    STATIONARY = 0,
    UP = 1,
    DOWN = 2
};

struct Paddle {
	float xpos;
	float ypos;	
};
class Ball { 
public:
	Ball(): xspeed(0), yspeed(0), xpos(395), ypos(295) {}
	float xspeed;
	float yspeed;
	float xpos;
	float ypos;
};


Move getAIMove(Ball ball, Paddle paddle) {
    int idealPosition = ball.ypos;
    if(ball.xspeed <= 0){
        auto turns = (ball.xpos - 50) / (-1 * ball.xspeed);
        idealPosition = ball.ypos + (ball.yspeed * turns);
    }


    if(idealPosition > paddle.ypos){
        return Move::DOWN;
    }
    if(idealPosition < paddle.ypos){
        return Move::UP;
    }
    return Move::STATIONARY;
}

EMSCRIPTEN_BINDINGS(pong) {
    emscripten::function("getAIMove", &getAIMove);
    emscripten::enum_<Move>("Move")
	    .value("STATIONARY", Move::STATIONARY)
	    .value("UP", Move::UP)
	    .value("DOWN", Move::DOWN);
    emscripten::value_object<Paddle>("Paddle")
	    .field("ypos", &Paddle::ypos)
	    .field("xpos", &Paddle::xpos);
    emscripten::class_<Ball>("Ball")
	    .constructor<>()
	    .property("ypos", &Ball::ypos)
	    .property("xpos", &Ball::xpos)
	    .property("yspeed", &Ball::yspeed)
	    .property("xspeed", &Ball::xspeed);
}
