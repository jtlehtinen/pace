#include "audio.h"

namespace {
  SoundBuffer* CreateSoundBuffer(IXAudio2* xaudio, const std::vector<Stereo>& samples, uint32_t sample_rate) {
    WAVEFORMATEX format = {};
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 2;
    format.nSamplesPerSec = sample_rate;
    format.nAvgBytesPerSec = sample_rate * static_cast<DWORD>(sizeof(Stereo));
    format.nBlockAlign = static_cast<WORD>(sizeof(Stereo));
    format.wBitsPerSample = static_cast<WORD>(sizeof(SoundSample)) * 8;

    SoundBuffer* sb = new SoundBuffer();
    sb->samples = samples;

    if (FAILED(xaudio->CreateSourceVoice(&sb->voice, &format, XAUDIO2_VOICE_NOPITCH, 1.0f))) {
      delete sb;
      return nullptr;
    }

    sb->buffer = {};
    sb->buffer.AudioBytes = static_cast<uint32_t>(sb->samples.size() * sizeof(Stereo));
    sb->buffer.pAudioData = reinterpret_cast<const BYTE*>(sb->samples.data());
    sb->buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

    sb->voice->SubmitSourceBuffer(&sb->buffer);
    sb->voice->Start(0);

    return sb;
  }

  void PlaySoundBuffer(SoundBuffer* sb) {
    if (!sb) return;

    sb->voice->SubmitSourceBuffer(&sb->buffer);
    sb->voice->Start(0);
  }

  void DestroySoundBuffer(SoundBuffer* sb) {
    if (sb && sb->voice) {
      sb->voice->Stop();
      sb->voice->DestroyVoice();
    }
    delete sb;
  }
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

  return true;
}

void AudioSystem::Terminate() {
  DestroySoundBuffer(buffer);

  mastering_voice->DestroyVoice();
  mastering_voice = nullptr;

  xaudio->Release();
  xaudio = nullptr;

  CoUninitialize(); // @TODO: unnecessary
}

bool AudioSystem::Play(const std::vector<Stereo>& samples, uint32_t sample_rate) {
  if (samples.size() * sizeof(Stereo) > XAUDIO2_MAX_BUFFER_BYTES) {
    return false;
  }

  DestroySoundBuffer(buffer);
  buffer = CreateSoundBuffer(xaudio, samples, sample_rate);
  PlaySoundBuffer(buffer);

  return true;
}
