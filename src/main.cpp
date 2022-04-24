#include <string.h>
#include "metronome.h"

void PrintUsage() {
  printf("pace is cli metronome\n\n");
  printf("USAGE:\n");
  printf("\tpace [OPTIONS]\n\n");
  printf("OPTIONS\n");

  printf("  -tempo int\n");
  printf("  \ttempo in beats per minute, range [60, 300]\n");

  printf("  -emp int\n");
  printf("  \tinterval between emphasized quarter notes, range [1, 36]\n");

  printf("  -subdiv int\n");
  printf("  \tsubdivision of each quarter note, range [1, 4]\n");

  printf("  -help\n");
  printf("  \tdispay this help text\n");
}

int main(int argc, const char* argv[]) {
  Parameters params;
  bool help = false;

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-tempo") == 0 && i < (argc - 1)) {
      params.tempo = static_cast<uint32_t>(atoi(argv[++i]));
    } else if (strcmp(argv[i], "-help") == 0) {
      help = true;
    } else if (strcmp(argv[i], "-emp") == 0 && i < (argc - 1)) {
      params.emphasis = static_cast<uint32_t>(atoi(argv[++i]));
    } else if (strcmp(argv[i], "-subdiv") == 0 && i < (argc - 1)) {
      // @TODO: Validate before casting...
      params.subdivision = static_cast<uint32_t>(atoi(argv[++i]));
    }
  }

  if (help) {
    PrintUsage();
    return 0;
  }

  constexpr uint32_t kMinTempo = 60;
  constexpr uint32_t kMaxTempo = 300;

  constexpr uint32_t kMinEmphasis = 1;
  constexpr uint32_t kMaxEmphasis = 36;

  constexpr uint32_t kMinSubdivision = 1;
  constexpr uint32_t kMaxSubdivision = 4;

  if (params.tempo < kMinTempo || params.tempo > kMaxTempo) {
    fprintf(stderr, "ERROR: invalid tempo '%d', must be between %d and %d\n\n",
      params.tempo, kMinTempo, kMaxTempo);
    return 0;
  }

  if (params.emphasis < kMinEmphasis || params.emphasis > kMaxEmphasis) {
    fprintf(stderr, "ERROR: invalid emphasis '%d', must be between %d and %d\n\n",
      params.emphasis, kMinEmphasis, kMaxEmphasis);
    return 0;
  }

  if (params.subdivision < kMinSubdivision || params.subdivision > kMaxSubdivision) {
    fprintf(stderr, "ERROR: invalid subdivision '%d', must be between %d and %d\n\n",
      params.subdivision, kMinSubdivision, kMaxSubdivision);
    return 0;
  }

  Metronome metronome;
  return metronome.Run(params);
}
