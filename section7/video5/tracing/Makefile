

ifeq ($(DOCKER),1)

build: docker-build
	docker run -v `pwd`:/data:rw -it emscripten ./upstream/emscripten/emcc /data/pong.cpp -o /data/pong_wasm.js --cache /data/.emscripten --std=c++17 --bind -lidbfs.js -s USE_BOOST_HEADERS=1 -lboost_headers -s FETCH=1 -s USE_SDL=2 -s USE_SDL_TTF=2 --preload-files /data/fonts -O3 -s USE_SDL_MIXER=2 --preload-files /data/sounds --tracing

docker-build:
	docker build -t emscripten .
run:
	python3 server.py
clean:
	docker run -v `pwd`:/data:rw -it emscripten rm -rf /data/pong_wasm.js /data/pong_wasm.wasm /data/.emscripten


else
check-emscripten:
	@emcc --version || (echo "Emscripten not detected, aborting build"; exit 1)

build: check-emscripten
	@echo "Not running in Docker"
	emcc pong.cpp -o pong_wasm.js --std=c++17 --bind -lidbfs.js -s USE_BOOST_HEADERS=1 -lboost_headers  -s FETCH=1 -s USE_SDL=2 -s USE_SDL_TTF=2 --preload-files fonts  -O3 -s USE_SDL_MIXER=2 --preload-files sounds --tracing

run:
	python3 server.py	
clean:
	rm -rf pong_wasm.* .emscripten
endif
