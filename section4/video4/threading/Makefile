

ifeq ($(DOCKER),1)

build: docker-build
	docker run -v `pwd`:/data:rw -it emscripten ./upstream/emscripten/emcc /data/pong.cpp -o /data/pong_wasm.js --emrun --cache /data/.emscripten --std=c++17 --bind -lidbfs.js -s USE_BOOST_HEADERS=1 -lboost_headers -s USE_PTHREADS=1

docker-build:
	docker build -t emscripten .
run:
	python3 -m http.server
clean:
	docker run -v `pwd`:/data:rw -it emscripten rm -rf /data/pong_wasm.js /data/pong_wasm.wasm /data/.emscripten


else
check-emscripten:
	@emcc --version || (echo "Emscripten not detected, aborting build"; exit 1)

build: check-emscripten
	@echo "Not running in Docker"
	emcc pong.cpp -o pong_wasm.js --emrun --std=c++17 --bind -lidbfs.js -s USE_BOOST_HEADERS=1 -lboost_headers -s USE_PTHREADS=1
run:
	emrun pong.html
clean:
	rm -rf pong_wasm.* .emscripten
endif
