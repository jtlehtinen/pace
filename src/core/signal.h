#pragma once

#include <Windows.h>

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
