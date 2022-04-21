#include <string.h>
#include "metronome.h"

void PrintUsage() {
  printf("pace is cli metronome\n\n");
  printf("USAGE:\n");
  printf("\tpace [OPTIONS]\n\n");
  printf("OPTIONS\n");

  printf("  -tempo int\n");
  printf("  \ttempo in beats per minute, range [60, 300]\n");
  printf("  -help\n");
  printf("  \tdispay this help text\n");
}

int main(int argc, const char* argv[]) {
  bool help = false;
  int tempo = 120;

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-tempo") == 0 && i < (argc - 1)) {
      tempo = atoi(argv[++i]);
    } else if (strcmp(argv[i], "-help") == 0) {
      help = true;
    }
  }

  if (help) {
    PrintUsage();
    return 0;
  }

  constexpr int kMinTempo = 60;
  constexpr int kMaxTempo = 300;

  if (tempo < kMinTempo || tempo > kMaxTempo) {
    fprintf(stderr, "ERROR: invalid tempo '%d', must be between %d and %d\n\n", tempo, kMinTempo, kMaxTempo);
    return 0;
  }

  Metronome metronome;
  return metronome.Run(static_cast<uint32_t>(tempo));
}
