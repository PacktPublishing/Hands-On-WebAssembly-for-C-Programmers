

ifeq ($(DOCKER),1)

build: docker-build
	docker run -v `pwd`:/data:rw -it emscripten ./upstream/emscripten/emcc /data/guessing.cpp -o /data/guess.html --emrun --cache /data/.emscripten --std=c++17

docker-build:
	docker build -t emscripten .
run:
	python3 -m http.server
clean:
	docker run -v `pwd`:/data:rw -it emscripten rm -rf /data/guess.html /data/guess.wasm /data/guess.js /data/.emscripten


else
check-emscripten:
	@emcc --version || (echo "Emscripten not detected, aborting build"; exit 1)

build: check-emscripten
	@echo "Not running in Docker"
	emcc guessing.cpp -o guess.html --emrun --std=c++17
run:
	emrun guess.html
clean:
	rm -rf guess.* .emscripten
endif

