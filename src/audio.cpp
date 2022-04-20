#include "audio.h"
#include <assert.h>
#include <math.h>

namespace {

  void FillWithSineWave(std::vector<Stereo>& buffer) {
    constexpr float kpi = 3.14159265359f;
    constexpr float ktau = 2.0f * kpi;

    float freq = 261.63f;
    float t = 0.0f;
    float tstep = ktau / freq;
    float amplitude = 3000.0f;

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
  }

}

void VoiceCallback::OnVoiceProcessingPassStart(UINT32 bytes_required) { }
void VoiceCallback::OnVoiceProcessingPassEnd() { }
void VoiceCallback::OnStreamEnd() { }
void VoiceCallback::OnBufferStart(void* buffer_context) { }
void VoiceCallback::OnBufferEnd(void* buffer_context) { }
void VoiceCallback::OnLoopEnd(void* buffer_context) { }
void VoiceCallback::OnVoiceError(void* buffer_context, HRESULT error) { }

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

  output.resize(sample_rate);
  const uint32_t buffer_size = static_cast<uint32_t>(output.size() * sizeof(Stereo));
  assert(buffer_size <= XAUDIO2_MAX_BUFFER_BYTES);

  FillWithSineWave(output);

  buffer.Flags = 0;
  buffer.AudioBytes = buffer_size;
  buffer.pAudioData = reinterpret_cast<const BYTE*>(output.data());
  buffer.PlayBegin = 0;
  buffer.PlayLength = buffer_size / sizeof(Stereo);
  buffer.LoopBegin = 0;
  buffer.LoopLength = buffer_size / sizeof(Stereo);
  buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
  buffer.pContext = nullptr;

  source_voice->SubmitSourceBuffer(&buffer);
  source_voice->Start(0);

  return true;
}

void AudioSystem::Terminate() {
  source_voice->DestroyVoice();
  source_voice = nullptr;

  mastering_voice->DestroyVoice();
  mastering_voice = nullptr;

  xaudio->Release();
  xaudio = nullptr;

  CoUninitialize(); // @TODO: unnecessary
}
