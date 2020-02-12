# Speed comparisons

Speed comparisons were done on a Quicksort of 100,000 elements, averaged over 1000 runs.

C++ (unoptimized)
g++ quicksort.cpp -o test -std=c++17 && ./test

C++ (optimized)
g++ quicksort.cpp -O3 -o test -std=c++17 && ./test

JavaScript
Copy file into browser (tests were done in Firefox)

WebAssembly (optimized)
emcc quicksort.cpp -O3 -o test.html -std=c++17 --emrun && emrun test.html 
