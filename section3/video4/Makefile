

ifeq ($(DOCKER),1)

build: docker-build
	docker run -v `pwd`:/data:rw -it emscripten ./upstream/emscripten/emcc /data/trap.cpp -o /data/trap.js --emrun --cache /data/.emscripten --std=c++17 --bind -s EXPORTED_FUNCTIONS="['_divide_1000_by', '_throw_exception']"

docker-build:
	docker build -t emscripten .
run:
	python3 -m http.server
clean:
	docker run -v `pwd`:/data:rw -it emscripten rm -rf /data/trap.js /data/trap.wasm /data/.emscripten


else
check-emscripten:
	@emcc --version || (echo "Emscripten not detected, aborting build"; exit 1)

build: check-emscripten
	@echo "Not running in Docker"
	emcc trap.cpp -o trap.js --emrun --std=c++17 --bind -s EXPORTED_FUNCTIONS="['_divide_1000_by', '_throw_exception']"
run:
	emrun index.html
clean:
	rm -rf trap.js trap.wasm .emscripten
endif

