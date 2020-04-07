

ifeq ($(DOCKER),1)

build: docker-build
	docker run -v `pwd`:/data:rw -it emscripten ./upstream/emscripten/emcc /data/factorial.cpp -o /data/factorial.wasm --cache /data/.emscripten -Oz -s EXPORTED_FUNCTIONS="[_factorial]"
	docker run -v `pwd`:/data:rw -it emscripten ./upstream/bin/wasm2wat /data/factorial.wasm -o /data/factorial.wat --generate-names --fold-exprs

docker-build:
	docker build -t emscripten .
clean:
	docker run -v `pwd`:/data:rw -it emscripten rm -rf /data/factorial.wat /data/factorial.wasm /data/.emscripten


else
check-emscripten:
	@emcc --version || (echo "Emscripten not detected, aborting build"; exit 1)
	@${EMSDK}/upstream/bin/wasm2wat --version || (echo "Wasm2Wat not detected, aborting build"; exit 1)

build: check-emscripten
	@echo "Not running in Docker"
	emcc factorial.cpp -o factorial.wasm -Oz -s EXPORTED_FUNCTIONS="[_factorial]" 
	${EMSDK}/upstream/bin/wasm2wat factorial.wasm -o factorial.wat --generate-names --fold-exprs
clean:
	rm -rf factorial.wa* .emscripten
endif

