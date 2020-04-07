#include <SDL2/SDL.h>
#include <SDL_audio.h>
#include <queue>
#include <cmath>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#include "emscripten/emscripten.h"


const int tone_duration = 100;

struct BeepObject {
  double toneFrequency;
  int samplesLeft;
};

class Beeper {
private:
  double phase = 0.0;
  int frequency;

  std::queue<BeepObject> beeps;
public:
  Beeper();
  ~Beeper();
  void beep(double toneFrequency, int durationMSecs);
  void generateSamples(float *stream, int length);
};

void audio_callback(void*, Uint8*, int);

Beeper::Beeper() {

  SDL_AudioSpec desiredSpec;

  desiredSpec.freq = 48000;
  desiredSpec.format = AUDIO_F32;
  desiredSpec.channels = 1;
  desiredSpec.samples = 1024; // This is samples per channel.
  desiredSpec.callback = audio_callback;
  desiredSpec.userdata = this;

  SDL_AudioSpec obtainedSpec;

  // you might want to look for errors here
  SDL_OpenAudio(&desiredSpec, &obtainedSpec);

  frequency = obtainedSpec.freq;

  // Immediately start producing audio.
  SDL_PauseAudio(0);
}

Beeper::~Beeper() {
  SDL_CloseAudio();
}

void Beeper::generateSamples(float *stream, int length) {
  const float AMPLITUDE = 1.0f;
  const int offset = 0; 

  int i = 0;
  while (i < length) {
    if (beeps.empty()) {
      memset(stream + i, 0, sizeof(float)*(length-i));
      return;
    }
    BeepObject& bo = beeps.front();

    int samplesToDo = std::min(i + bo.samplesLeft, length);
    bo.samplesLeft -= samplesToDo - i;

    while (i < samplesToDo) {
        stream[i] = (float)(offset +
          (AMPLITUDE * std::sin(phase * 2 * M_PI / frequency)));
        phase += bo.toneFrequency;
        i++;
    }

    if (bo.samplesLeft == 0) {
      beeps.pop();
    }
  }
}

void Beeper::beep(double toneFrequency, int durationMSecs) {
  SDL_LockAudio();
  beeps.push({ toneFrequency, durationMSecs * frequency / 1000});
  SDL_UnlockAudio();
}

void audio_callback(void *_beeper, Uint8 *_stream, int _length) {
    Beeper* beeper = (Beeper*) _beeper;
    float *stream = (float*) _stream;
    int length = _length / sizeof(float);
    beeper->generateSamples(stream, length);
}

Beeper *beep = new Beeper();
int main(int argc, char** argv) {
  SDL_Init(SDL_INIT_AUDIO);
  beep->beep(783.99, 200);
  beep->beep(440, 400);
  return 0;
}
