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

constexpr uint32_t kMinTempo = 60;
constexpr uint32_t kMaxTempo = 300;

constexpr uint32_t kMinEmphasis = 1;
constexpr uint32_t kMaxEmphasis = 36;

constexpr uint32_t kMinSubdivision = 1;
constexpr uint32_t kMaxSubdivision = 4;

void PrintUsage() {
  printf("pace is cli metronome\n\n");
  printf("USAGE:\n");
  printf("\tpace [OPTIONS]\n\n");
  printf("OPTIONS\n");

  printf("  -tempo int\n");
  printf("  \ttempo in beats per minute, range [%u, %u]\n", kMinTempo, kMaxTempo);

  printf("  -emp int\n");
  printf("  \tinterval between emphasized quarter notes, range [%u, %u]\n", kMinEmphasis, kMaxEmphasis);

  printf("  -subdiv int\n");
  printf("  \tsubdivision of each quarter note, range [%u, %u]\n", kMinSubdivision, kMaxSubdivision);

  printf("  -help\n");
  printf("  \tdispay this help text\n");
}

int main(int argc, const char* argv[]) {
  if (!quit.Valid()) return 1;

  Parameters params;
  bool help = false;

  for (int i = 1; i < argc; ++i) {
    // @TODO: Handle invalid args better...
    if (strcmp(argv[i], "-tempo") == 0 && i < (argc - 1)) {
      params.tempo = static_cast<uint32_t>(atoi(argv[++i]));
    } else if (strcmp(argv[i], "-emp") == 0 && i < (argc - 1)) {
      params.emphasis = static_cast<uint32_t>(atoi(argv[++i]));
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

  if (params.tempo < kMinTempo || params.tempo > kMaxTempo) {
    fprintf(stderr, "ERROR: invalid tempo '%u', must be between %u and %u\n\n",
      params.tempo, kMinTempo, kMaxTempo);
    return 0;
  }

  if (params.emphasis < kMinEmphasis || params.emphasis > kMaxEmphasis) {
    fprintf(stderr, "ERROR: invalid emphasis '%u', must be between %u and %u\n\n",
      params.emphasis, kMinEmphasis, kMaxEmphasis);
    return 0;
  }

  if (params.subdivision < kMinSubdivision || params.subdivision > kMaxSubdivision) {
    fprintf(stderr, "ERROR: invalid subdivision '%u', must be between %u and %u\n\n",
      params.subdivision, kMinSubdivision, kMaxSubdivision);
    return 0;
  }

  Metronome metronome;
  if (!metronome.Initialize(params.sample_rate)) {
    return 1;
  }

  auto InterruptHandler = [](int signal) { quit.Trigger(); };
  signal(SIGINT, InterruptHandler);

  metronome.Play(params);

  quit.Wait();

  metronome.Terminate();

  return 0;
}
