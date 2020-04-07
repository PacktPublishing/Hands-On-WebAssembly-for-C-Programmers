#include <iostream>

extern "C" int add(int a, int b){
	std::cout << a << " " <<  b << " " << a + b << "\n";
	return a + b;
}
