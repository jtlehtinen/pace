#include <string.h>
#include <signal.h>
#include "core/metronome.h"

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

void PrintUsage() {
  printf("pace is cli metronome\n\n");
  printf("USAGE:\n");
  printf("\tpace [OPTIONS]\n\n");
  printf("OPTIONS\n");

  printf("  -tempo int\n");
  printf("  \ttempo in beats per minute, range [%u, %u]\n", kMetronomeMinTempo, kMetronomeMaxTempo);

  printf("  -beats int\n");
  printf("  \tinterval between emphasized quarter notes, range [%u, %u]\n", kMetronomeMinBeats, kMetronomeMaxBeats);

  printf("  -subdiv int\n");
  printf("  \tsubdivision of each quarter note, range [%u, %u]\n", kMetronomeMinSubdivision, kMetronomeMaxSubdivision);

  printf("  -help\n");
  printf("  \tdispay this help text\n");
}

int main(int argc, const char* argv[]) {
  if (!quit.Valid()) return 1;

  MetronomeParameters params;
  bool help = false;

  for (int i = 1; i < argc; ++i) {
    // @TODO: Handle invalid args better...
    if (strcmp(argv[i], "-tempo") == 0 && i < (argc - 1)) {
      params.tempo = static_cast<uint32_t>(atoi(argv[++i]));
    } else if (strcmp(argv[i], "-beats") == 0 && i < (argc - 1)) {
      params.beats = static_cast<uint32_t>(atoi(argv[++i]));
    } else if (strcmp(argv[i], "-subdiv") == 0 && i < (argc - 1)) {
      params.subdivision = static_cast<uint32_t>(atoi(argv[++i]));
    } else if (strcmp(argv[i], "-help") == 0) {
      help = true;
    }
  }

  if (help) {
    PrintUsage();
    return 0;
  }

  if (params.tempo < kMetronomeMinTempo || params.tempo > kMetronomeMaxTempo) {
    fprintf(stderr, "ERROR: invalid tempo '%u', must be between %u and %u\n\n",
      params.tempo, kMetronomeMinTempo, kMetronomeMaxTempo);
    return 0;
  }

  if (params.beats < kMetronomeMinBeats || params.beats > kMetronomeMaxBeats) {
    fprintf(stderr, "ERROR: invalid beats '%u', must be between %u and %u\n\n",
      params.beats, kMetronomeMinBeats, kMetronomeMaxBeats);
    return 0;
  }

  if (params.subdivision < kMetronomeMinSubdivision || params.subdivision > kMetronomeMaxSubdivision) {
    fprintf(stderr, "ERROR: invalid subdivision '%u', must be between %u and %u\n\n",
      params.subdivision, kMetronomeMinSubdivision, kMetronomeMaxSubdivision);
    return 0;
  }

  Metronome metronome;
  if (!metronome.Initialize()) {
    return 1;
  }

  auto InterruptHandler = [](int signal) { quit.Trigger(); };
  signal(SIGINT, InterruptHandler);

  metronome.Play(params);

  quit.Wait();

  metronome.Terminate();

  return 0;
}
