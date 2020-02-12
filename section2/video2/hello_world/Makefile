

ifeq ($(DOCKER),1)

build: docker-build
	docker run -v `pwd`:/data:rw -it emscripten ./upstream/emscripten/emcc /data/hello_world.cpp -o /data/test.html --emrun --cache /data/.emscripten --std=c++17

docker-build:
	docker build -t emscripten .
run:
	python3 -m http.server
clean:
	docker run -v `pwd`:/data:rw -it emscripten rm -rf /data/test.html /data/test.wasm /data/test.js /data/.emscripten


else
check-emscripten:
	@emcc --version || (echo "Emscripten not detected, aborting build"; exit 1)

build: check-emscripten
	@echo "Not running in Docker"
	emcc hello_world.cpp -o test.html --emrun --std=c++17
run:
	emrun test.html
clean:
	rm -rf test.* .emscripten
endif

