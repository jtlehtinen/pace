#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include "audio.h"

class Application {
private:
  AudioSystem audio;

public:
  bool Initialize();
  void Terminate();

  void Run();
};

bool Application::Initialize() {
  if (audio.Initialize(44100)) return false;

  return true;
}

void Application::Terminate() {
  audio.Terminate();
}

void Application::Run() {
  // @TODO: do something...
}

int main() {
  Application app;
  if (app.Initialize()) {
    app.Run();
    app.Terminate();
  }
  return 0;
}
