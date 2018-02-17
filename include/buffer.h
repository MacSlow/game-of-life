#ifndef _BUFFER_H
#define _BUFFER_H

#include <SDL.h>
#include <memory>
#include <tuple>
#include <vector>

using CellState = bool;
using CellBuffer = std::vector<CellState>;
using SurfaceBuffer = std::vector<unsigned char>;

class Buffer {
  public:
    Buffer (size_t width, size_t height);

    void reset ();
    void update ();
    void paint (const SDL_Window* window) const;

  private:
    size_t _width = 0;
    size_t _height = 0;
    CellBuffer _pingPongBufferA;
    CellBuffer _pingPongBufferB;
    SurfaceBuffer _bufferSurface;
};

#endif // _BUFFER_H
