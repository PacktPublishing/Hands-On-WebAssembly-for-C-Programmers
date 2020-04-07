

ifeq ($(DOCKER),1)

build: docker-build
	docker run -v `pwd`:/data:rw -it emscripten ./upstream/emscripten/emcc /data/string_example.cpp -o /data/strings.html --emrun --cache /data/.emscripten --std=c++17 --bind -s EXTRA_EXPORTED_RUNTIME_METHODS="['getValue']"

docker-build:
	docker build -t emscripten .
run:
	python3 -m http.server
clean:
	docker run -v `pwd`:/data:rw -it emscripten rm -rf /data/strings.js /data/strings.wasm /data/strings.html /data/.emscripten


else
check-emscripten:
	@emcc --version || (echo "Emscripten not detected, aborting build"; exit 1)

build: check-emscripten
	@echo "Not running in Docker"
	emcc string_example.cpp -o strings.html --emrun --std=c++17 --bind -s EXTRA_EXPORTED_RUNTIME_METHODS="['getValue']"
run:
	emrun strings.html
clean:
	rm -rf strings.* .emscripten
endif

