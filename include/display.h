#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <SDL.h>
#include <memory>

#include "buffer.h"

class Display {
  public:
    Display (unsigned int width, unsigned int height);
    ~Display ();

    bool run ();
    bool update ();

  private:
    void initialize ();

  private:
    bool _initialized = false;
    SDL_Window* _window = nullptr;
    bool _running = true;
    bool _paused = false;
    std::unique_ptr<Buffer> _buffer;
    unsigned _seconds = 0;
    unsigned _allFrames = 0;
    unsigned _min = 0;
    unsigned _avg = 0;
    unsigned _max = 0;
};

#endif // _DISPLAY_H
