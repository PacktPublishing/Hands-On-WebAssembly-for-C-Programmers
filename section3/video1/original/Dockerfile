FROM ubuntu:latest

RUN apt update && apt install -y git python libxml2
RUN git clone https://github.com/emscripten-core/emsdk.git
WORKDIR emsdk

RUN ./emsdk install latest
RUN ./emsdk activate latest
RUN ln -s emsdk/upstream/emscripten/emcc /usr/bin/emcc 
