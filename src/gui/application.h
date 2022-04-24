#pragma once

class Application {
  private:
    int tempo = 120;
    int subdivision = 1;
    int emphasis = 4;

  public:
    Application();
    ~Application();

    Application(const Application& other) = delete;
    void operator=(const Application& rhs) = delete;

    bool Init();
    void Render();
};
