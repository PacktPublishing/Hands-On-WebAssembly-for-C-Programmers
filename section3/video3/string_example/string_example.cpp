#include <emscripten.h>
#include <string>

EM_JS(void, print, (std::string text), {
	console.log(text);
});

int main(){
	print("ABCDEF");
	return 0;
}
