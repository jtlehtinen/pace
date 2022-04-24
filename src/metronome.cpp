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


int Metronome::Run(Parameters params) {
  if (!quit.Valid()) return 1;

  if (!audio.Initialize(params.sample_rate)) return 1;

  auto InterruptHandler = [](int signal) { quit.Trigger(); };
  signal(SIGINT, InterruptHandler);

  auto samples = Beat::Generate(params.sample_rate, params.tempo, params.subdivision, params.emphasis);
  audio.Play(samples, params.sample_rate);

  quit.Wait();

  audio.Terminate();

  return 0;
}
