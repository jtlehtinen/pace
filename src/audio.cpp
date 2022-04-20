#include "audio.h"

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
