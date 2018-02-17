#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <SDL.h>
#include <memory>

#include "buffer.h"

class Application {
  public:
    Application (unsigned int width, unsigned int height);
    ~Application ();

    void run ();
    void update ();

  private:
    void initialize ();
    void handle_events ();
    void update_framerate ();
    void update_title ();

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

#endif // _APPLICATION_H