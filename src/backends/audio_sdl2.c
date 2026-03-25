/* SDL2 audio bridge (M8 capture -> ring buffer -> playback). Used when M8C_USE_SDL2 is defined. */
#ifndef USE_LIBUSB
#include "audio.h"
#include "m8c_sdl.h"
#include "ringbuffer.h"

#include <SDL2/SDL.h>
#include <string.h>
#include <strings.h>

static SDL_AudioDeviceID audio_dev_in;
static SDL_AudioDeviceID audio_dev_out;
static RingBuffer *audio_ring;
static SDL_mutex *audio_ring_mutex;
static unsigned int audio_paused = 0;
static unsigned int audio_initialized = 0;

static void SDLCALL capture_cb(void *userdata, Uint8 *stream, int len) {
  (void)userdata;
  if (!audio_ring || !audio_ring_mutex) {
    return;
  }
  SDL_LockMutex(audio_ring_mutex);
  ring_buffer_push(audio_ring, stream, (uint32_t)len);
  SDL_UnlockMutex(audio_ring_mutex);
}

static void SDLCALL playback_cb(void *userdata, Uint8 *stream, int len) {
  (void)userdata;
  if (!audio_ring || !audio_ring_mutex) {
    memset(stream, 0, (size_t)len);
    return;
  }
  SDL_LockMutex(audio_ring_mutex);
  uint32_t got = ring_buffer_pop(audio_ring, stream, (uint32_t)len);
  SDL_UnlockMutex(audio_ring_mutex);
  if (got < (uint32_t)len) {
    memset(stream + got, 0, (size_t)len - got);
  }
}

void audio_toggle(const char *output_device_name, unsigned int audio_buffer_size) {
  if (!audio_initialized) {
    audio_initialize(output_device_name, audio_buffer_size);
    return;
  }
  if (audio_paused) {
    SDL_PauseAudioDevice(audio_dev_out, 0);
    SDL_PauseAudioDevice(audio_dev_in, 0);
  } else {
    SDL_PauseAudioDevice(audio_dev_out, 1);
    SDL_PauseAudioDevice(audio_dev_in, 1);
  }
  audio_paused = !audio_paused;
  SDL_Log(audio_paused ? "Audio paused" : "Audio resumed");
}

int audio_initialize(const char *output_device_name, const unsigned int audio_buffer_size) {
  int num_in = SDL_GetNumAudioDevices(1);
  int num_out = SDL_GetNumAudioDevices(0);
  const char *m8_name = NULL;

  if (SDL_Init(SDL_INIT_AUDIO) != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "SDL Audio init failed: %s", SDL_GetError());
    return 0;
  }

  for (int i = 0; i < num_in; i++) {
    const char *name = SDL_GetAudioDeviceName(i, 1);
    if (name && SDL_strstr(name, "M8") != NULL) {
      SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "M8 Audio Input device found: %s", name);
      m8_name = name;
      break;
    }
  }

  const char *out_name = NULL;
  if (output_device_name != NULL) {
    for (int i = 0; i < num_out; i++) {
      const char *name = SDL_GetAudioDeviceName(i, 0);
      if (name != NULL && strcasestr(name, output_device_name) != NULL) {
        SDL_Log("Requested output device found: %s", name);
        out_name = name;
        break;
      }
    }
  }

  if (m8_name == NULL) {
    SDL_Log("Cannot find M8 audio input device");
    return 0;
  }

  SDL_AudioSpec want;
  SDL_zero(want);
  want.freq = 44100;
  want.format = AUDIO_S16SYS;
  want.channels = 2;
  want.samples = audio_buffer_size > 0 ? (Uint16)audio_buffer_size : 512;
  want.callback = capture_cb;
  want.userdata = NULL;

  SDL_AudioSpec have_in;
  audio_dev_in = SDL_OpenAudioDevice(m8_name, 1, &want, &have_in, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
  if (audio_dev_in == 0) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Error opening audio input: %s", SDL_GetError());
    return 0;
  }

  SDL_AudioSpec want_out;
  SDL_zero(want_out);
  want_out.freq = have_in.freq;
  want_out.format = have_in.format;
  want_out.channels = have_in.channels;
  want_out.samples = have_in.samples;
  want_out.callback = playback_cb;
  want_out.userdata = NULL;

  SDL_AudioSpec have_out;
  audio_dev_out =
      SDL_OpenAudioDevice(out_name, 0, &want_out, &have_out, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
  if (audio_dev_out == 0) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Error opening audio output: %s", SDL_GetError());
    SDL_CloseAudioDevice(audio_dev_in);
    return 0;
  }

  audio_ring_mutex = SDL_CreateMutex();
  if (!audio_ring_mutex) {
    SDL_CloseAudioDevice(audio_dev_in);
    SDL_CloseAudioDevice(audio_dev_out);
    return 0;
  }
  audio_ring = ring_buffer_create(1024u * 1024u);
  if (!audio_ring) {
    SDL_DestroyMutex(audio_ring_mutex);
    audio_ring_mutex = NULL;
    SDL_CloseAudioDevice(audio_dev_in);
    SDL_CloseAudioDevice(audio_dev_out);
    return 0;
  }

  SDL_PauseAudioDevice(audio_dev_in, 0);
  SDL_PauseAudioDevice(audio_dev_out, 0);

  audio_paused = 0;
  audio_initialized = 1;
  return 1;
}

void audio_close(void) {
  if (!audio_initialized) {
    return;
  }
  SDL_Log("Closing audio devices");
  SDL_CloseAudioDevice(audio_dev_in);
  SDL_CloseAudioDevice(audio_dev_out);
  audio_dev_in = 0;
  audio_dev_out = 0;
  if (audio_ring) {
    ring_buffer_free(audio_ring);
    audio_ring = NULL;
  }
  if (audio_ring_mutex) {
    SDL_DestroyMutex(audio_ring_mutex);
    audio_ring_mutex = NULL;
  }
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
  audio_initialized = 0;
}

#endif /* !USE_LIBUSB */
