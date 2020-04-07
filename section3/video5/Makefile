

ifeq ($(DOCKER),1)

build: docker-build
	docker run -v `pwd`:/data:rw -it emscripten ./upstream/emscripten/emcc /data/test.cpp -o /data/test.js --emrun --cache /data/.emscripten --std=c++17 --bind -s EXPORTED_FUNCTIONS="['_add']" --pre-js /data/pre.js

docker-build:
	docker build -t emscripten .
run:
	python3 -m http.server
clean:
	docker run -v `pwd`:/data:rw -it emscripten rm -rf /data/test.js /data/test.wasm /data/test.html /data/.emscripten


else
check-emscripten:
	@emcc --version || (echo "Emscripten not detected, aborting build"; exit 1)

build: check-emscripten
	@echo "Not running in Docker"
	emcc test.cpp -o test.html --emrun --std=c++17 --bind -s EXPORTED_FUNCTIONS="['_add']" --pre-js pre.js
run:
	emrun test.html
clean:
	rm -rf test.js test.wasm test.html .emscripten
endif

