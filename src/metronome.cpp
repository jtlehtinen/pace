#include "metronome.h"
#include "beat.h"
#include <signal.h>

namespace {
  struct Signal {
    HANDLE event;

    Signal() {
      event = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
    }

    ~Signal() {
      CloseHandle(event);
    }

    void Trigger() {
      SetEvent(event);
    }

    void Wait() {
      WaitForSingleObjectEx(event, INFINITE, false);
    }

    bool Valid() const {
      return event != INVALID_HANDLE_VALUE;
    }
  };

  Signal quit;
}


int Metronome::Run(uint32_t tempo, uint32_t sample_rate) {
  if (!quit.Valid()) return 1;

  if (!audio.Initialize(sample_rate)) return 1;

  auto InterruptHandler = [](int signal) { quit.Trigger(); };
  signal(SIGINT, InterruptHandler);

  auto samples = Beat::Generate(static_cast<double>(sample_rate), tempo);
  audio.Play(samples, sample_rate);

  quit.Wait();

  audio.Terminate();

  return 0;
}
