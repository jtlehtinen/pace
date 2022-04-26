#include "audio.h"

namespace {
  void sound_thread_func(SoundOutput* so) {
    while (!so->quit) {
      so->callback.buffer_end.Wait();

      {
        std::lock_guard<std::mutex> lock(so->mutex);

        size_t sample_count = so->output_buffer.size();

        size_t max_sample_index = so->samples.size();
        size_t sample_index = so->sample_index;

        for (size_t i = 0; i < sample_count; ++i) {
          so->output_buffer[i] = so->samples[sample_index];
          sample_index = (sample_index + 1) % max_sample_index;
        }
        so->sample_index = sample_index;
      }

      so->voice->SubmitSourceBuffer(&so->buffer);
    }
  }
}

void VoiceCallback::OnBufferEnd(void* buffer_context) {
  buffer_end.Trigger();
}

bool AudioSystem::Initialize(uint32_t sample_rate) {
  if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
    return false;
  }

  if (FAILED(XAudio2Create(&xaudio, 0))) {
    CoUninitialize();
    return false;
  }

  #if METRONOME_DEBUG
  XAUDIO2_DEBUG_CONFIGURATION debug = {};
  debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
  debug.BreakMask = XAUDIO2_LOG_ERRORS;
  xaudio->SetDebugConfiguration(&debug, 0);
  #endif

  if (FAILED(xaudio->CreateMasteringVoice(&mastering_voice, 2,  sample_rate))) {
    xaudio->Release();
    CoUninitialize();
    return false;
  }

  WAVEFORMATEX format = {};
  format.wFormatTag = WAVE_FORMAT_PCM;
  format.nChannels = 2;
  format.nSamplesPerSec = sample_rate;
  format.nAvgBytesPerSec = sample_rate * static_cast<DWORD>(sizeof(Stereo));
  format.nBlockAlign = static_cast<WORD>(sizeof(Stereo));
  format.wBitsPerSample = static_cast<WORD>(sizeof(SoundSample)) * 8;

  sound_output.samples.resize(1, Stereo{});
  sound_output.sample_index = 0;

  sound_output.output_buffer.resize(sample_rate / 10, Stereo{});
  sound_output.buffer = {};
  sound_output.buffer.AudioBytes = static_cast<uint32_t>(sound_output.output_buffer.size() * sizeof(Stereo));
  sound_output.buffer.pAudioData = reinterpret_cast<const BYTE*>(sound_output.output_buffer.data());
  sound_output.buffer.LoopCount = XAUDIO2_NO_LOOP_REGION;

  if (FAILED(xaudio->CreateSourceVoice(&sound_output.voice, &format, XAUDIO2_VOICE_NOPITCH, 1.0f, &sound_output.callback))) {
    mastering_voice->DestroyVoice();
    xaudio->Release();
    CoUninitialize();
    return false;
  }

  sound_output.voice->SubmitSourceBuffer(&sound_output.buffer);
  sound_output.voice->Start(0);

  sound_output_thread = std::thread(sound_thread_func, &sound_output);

  return true;
}

void AudioSystem::Terminate() {
  sound_output.quit = true;
  sound_output_thread.join();

  sound_output.voice->Stop();
  sound_output.voice->DestroyVoice();

  mastering_voice->DestroyVoice();
  mastering_voice = nullptr;

  xaudio->Release();
  xaudio = nullptr;

  CoUninitialize();
}

bool AudioSystem::Play(const std::vector<Stereo>& samples) {
  std::lock_guard<std::mutex> lock(sound_output.mutex);
  sound_output.samples = samples;
  sound_output.sample_index = 0;
  return true;
}

void AudioSystem::Stop() {
  std::lock_guard<std::mutex> lock(sound_output.mutex);
  sound_output.samples.resize(1, Stereo{});
  sound_output.sample_index = 0;
}
