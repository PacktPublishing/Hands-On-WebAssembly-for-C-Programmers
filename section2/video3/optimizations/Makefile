

ifeq ($(DOCKER),1)

build: docker-build
	docker run -v `pwd`:/data:rw -it emscripten ./upstream/emscripten/emcc /data/quicksort.cpp -o /data/quicksort_unoptimized.html --emrun --cache /data/.emscripten --std=c++17
	docker run -v `pwd`:/data:rw -it emscripten ./upstream/emscripten/emcc /data/quicksort.cpp -o /data/quicksort_size.html -Oz --emrun --cache /data/.emscripten --std=c++17
	docker run -v `pwd`:/data:rw -it emscripten ./upstream/emscripten/emcc /data/quicksort.cpp -o /data/quicksort_speed.html -O3 --emrun --cache /data/.emscripten --std=c++17

docker-build:
	docker build -t emscripten .
run:
	python3 -m http.server
clean:
	docker run -v `pwd`:/data:rw -it emscripten rm -rf /data/quicksort_unoptimized.html /data/quicksort_unoptimized.wasm /data/quicksort_unoptimized.js /data/.emscripten
	docker run -v `pwd`:/data:rw -it emscripten rm -rf /data/quicksort_size.html /data/quicksort_size.wasm /data/quicksort_size.js /data/.emscripten
	docker run -v `pwd`:/data:rw -it emscripten rm -rf /data/quicksort_speed.html /data/quicksort_speed.wasm /data/quicksort_speed.js /data/.emscripten


else
check-emscripten:
	@emcc --version || (echo "Emscripten not detected, aborting build"; exit 1)

build: check-emscripten
	@echo "Not running in Docker"
	emcc quicksort.cpp -o quicksort_unoptimized.html --emrun --std=c++17
	emcc quicksort.cpp -o quicksort_size.html -Oz --emrun --std=c++17
	emcc quicksort.cpp -o quicksort_speed.html -O3 --emrun --std=c++17
run:
	emrun guess.html
clean:
	rm -rf quicksort_*.* .emscripten
endif

