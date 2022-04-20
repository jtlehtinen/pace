#include "audio.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

namespace {
  void FillWithSineWave(std::vector<Stereo>& buffer, SineWave& sine_wave) {
    constexpr float kpi = 3.14159265359f;
    constexpr float ktau = 2.0f * kpi;

    float t = sine_wave.t;
    float frequency = sine_wave.frequency;
    float tstep = ktau / frequency;
    float amplitude = sine_wave.amplitude;

    for (size_t i = 0; i < buffer.size(); ++i) {
      auto value = static_cast<int16_t>(amplitude * sinf(t));
      buffer[i].left = value;
      buffer[i].right = value;
      t += tstep;

      // float precision
      while (t > ktau) {
        t -= ktau;
      }
    }

    sine_wave.t = t;
  }

  void sound_thread_func(SoundThreadContext* ctx) {
    // @TODO: Ensure sound thread does not block on
    // WaitForSingleObjectEx on exit.
    while (!ctx->quit) {
      // @TODO: timeout...
      if (WaitForSingleObjectEx(ctx->voice_callback->buffer_end_event, INFINITE, false) == WAIT_TIMEOUT) {
        break;
      }

      // @TODO: Do sound buffers require double buffering?
      // Can't hear anything wrong in the sine wave output,
      // so maybe not.

      FillWithSineWave(*ctx->samples, ctx->sine_wave);
      ctx->source_voice->SubmitSourceBuffer(ctx->buffer);
    }
  }
}

void VoiceCallback::OnBufferEnd(void* buffer_context) {
  SetEvent(buffer_end_event);
}

bool AudioSystem::Initialize(uint32_t sample_rate) {
  // @TODO: unnecessary?
  if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
    return false;
  }

  if (FAILED(XAudio2Create(&xaudio, 0))) {
    CoUninitialize(); // @TODO: unnecessary?
    return false;
  }

  #if METRONOME_DEBUG
  XAUDIO2_DEBUG_CONFIGURATION debug = {};
  debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
  debug.BreakMask = XAUDIO2_LOG_ERRORS;
  xaudio->SetDebugConfiguration(&debug, 0);
  #endif

  // @NOTE:
  // Use a mastering voice to represent the audio
  // output device. You cannot submit data buffers
  // directly to mastering voices, but data submitted
  // to other types of voices must go to a mastering
  // voice to be heard.
  if (FAILED(xaudio->CreateMasteringVoice(&mastering_voice, 2,  sample_rate))) {
    xaudio->Release();
    CoUninitialize();
    return false;
  }

  format = {};
  format.wFormatTag = WAVE_FORMAT_PCM;
  format.nChannels = 2;
  format.nSamplesPerSec = sample_rate;
  format.nAvgBytesPerSec = sample_rate * sizeof(Stereo);
  format.nBlockAlign = sizeof(Stereo);
  format.wBitsPerSample = sizeof(SoundSample) * 8;
  format.cbSize = 0;

  // @NOTE:
  // Use source voices to submit audio data into
  // the XAudio2 processing pipeline. They are
  // the entry points into the XAudio2 Audio Graph.
  // You must send voice data to a mastering voice
  // to be heard, either directly or through
  // intermediate submix voices.
  if (FAILED(xaudio->CreateSourceVoice(&source_voice, &format, XAUDIO2_VOICE_NOPITCH, 1.0f, &voice_callback))) {
    mastering_voice->DestroyVoice();
    xaudio->Release();
    return false;
  }

  output_samples.resize(sample_rate / 10);
  const size_t buffer_size = output_samples.size() * sizeof(Stereo);
  assert(buffer_size <= XAUDIO2_MAX_BUFFER_BYTES);

  output_buffer.Flags = 0;
  output_buffer.AudioBytes = static_cast<uint32_t>(buffer_size);
  output_buffer.pAudioData = reinterpret_cast<const BYTE*>(output_samples.data());
  output_buffer.PlayBegin = 0;
  output_buffer.PlayLength = 0;
  output_buffer.LoopBegin = XAUDIO2_NO_LOOP_REGION;
  output_buffer.LoopLength = 0;
  output_buffer.LoopCount = 0;
  output_buffer.pContext = nullptr;

  source_voice->SubmitSourceBuffer(&output_buffer);
  source_voice->Start(0);

  // start the sound thread
  sound_thread_context.voice_callback = &voice_callback;
  sound_thread_context.source_voice = source_voice;
  sound_thread_context.buffer = &output_buffer;
  sound_thread_context.samples = &output_samples;
  sound_thread_context.sine_wave.frequency = 261.63f;
  sound_thread_context.sine_wave.amplitude = 3000.0f;
  sound_thread_context.sine_wave.t = 0.0f;

  sound_thread = std::thread(sound_thread_func, &sound_thread_context);

  return true;
}

void AudioSystem::Terminate() {
  sound_thread_context.quit = true;
  sound_thread.join();

  source_voice->Stop();

  source_voice->DestroyVoice();
  source_voice = nullptr;

  mastering_voice->DestroyVoice();
  mastering_voice = nullptr;

  xaudio->Release();
  xaudio = nullptr;

  CoUninitialize(); // @TODO: unnecessary
}
