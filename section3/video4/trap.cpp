#include <stdexcept>

extern "C" {

	void throw_exception() { 
		throw std::runtime_error("Whoops");
	}

	float divide_1000_by(char num) {
		return 1000 / num;
	}
}
