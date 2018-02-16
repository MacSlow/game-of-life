#include <iostream>
#include <sstream>

#include "display.h"

using namespace std;

#define WIN_TITLE "Conway's Game-Of-Life by MacSlow"

void Display::initialize ()
{
    if (_initialized) {
        return;
    }

    int result = 0;
    SDL_ClearError ();
    result = SDL_Init (SDL_INIT_VIDEO);
    if (result != 0) {
        std::cout << "SDL_Init() failed: " << SDL_GetError () << std::endl;
        _initialized = false;
        return;
    }

    _initialized = true;
}

Display::Display (unsigned int width, unsigned int height)
    : _initialized (false), _window (NULL), _running (false), _paused (false),
      _seconds (0), _allFrames (0), _min (0), _avg (0), _max (0)
{
    initialize ();

    SDL_ClearError ();
    _window = SDL_CreateWindow (WIN_TITLE, SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    if (!_window) {
        std::cout << "window creation failed: " << SDL_GetError () << std::endl;
        return;
    }

    _buffer = make_unique<Buffer> (width, height);
}

Display::~Display ()
{
    SDL_DestroyWindow (_window);
    SDL_Quit ();
}

bool Display::run ()
{
    if (!_initialized) {
        return false;
    }

    _running = true;

    while (_running) {
        SDL_Event event;
        while (SDL_PollEvent (&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_RETURN) {
                    _buffer->reset ();
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    _running = false;
                } else if (event.key.keysym.sym == SDLK_SPACE) {
                    _paused = !_paused;
                    if (_paused) {
                        std::stringstream title;
                        title << WIN_TITLE << " - paused";
                        std::string str (title.str ());
                        SDL_SetWindowTitle (_window, str.c_str ());
                    } else {
                        SDL_SetWindowTitle (_window, WIN_TITLE);
                    }
                }
                break;

            case SDL_QUIT:
                _running = false;
                break;
            }
        }

        update ();
    }

    return true;
}

bool Display::update ()
{
    if (!_initialized) {
        return false;
    }

    if (_paused) {
        return true;
    }

    static unsigned int fps = 0;
    static unsigned int lastTick = 0;
    static unsigned int currentTick = 0;

    // spit out frame-rate and frame-time
    fps++;
    currentTick = SDL_GetTicks ();
    if (currentTick - lastTick > 1000) {
        std::stringstream title;
        title << WIN_TITLE << " - " << _min << "/" << _avg << "/" << _max
              << " fps";
        std::string str (title.str ());
        SDL_SetWindowTitle (_window, str.c_str ());
        _seconds++;
        lastTick = currentTick;

        if (_max < fps) {
            _max = fps;
        }
        if (_seconds > 0) {
            _allFrames += fps;
            _avg = _allFrames / _seconds;
        }
        if (_min > fps || _min == 0) {
            _min = fps;
        }

        fps = 0;
    }

    _buffer->update ();
    _buffer->paint (_window);

    return true;
}
