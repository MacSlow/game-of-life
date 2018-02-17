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
    Buffer (unsigned width, unsigned height);

    void reset ();
    void update ();
    void paint (const SDL_Window* window) const;

  private:
    unsigned _width = 0;
    unsigned _height = 0;
    CellBuffer _pingPongBufferA;
    CellBuffer _pingPongBufferB;
    SurfaceBuffer _bufferSurface;
};

#endif // _BUFFER_H
