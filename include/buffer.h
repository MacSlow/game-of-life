#ifndef _BUFFER_H
#define _BUFFER_H

#include <SDL.h>
#include <memory>
#include <tuple>
#include <vector>

using CellBufferType = std::vector<bool>;
using SurfaceBufferType = std::vector<unsigned char>;

class Buffer {
  public:
    Buffer (unsigned width, unsigned height);
    ~Buffer ();

    void reset ();
    void update ();
    void paint (const SDL_Window* window) const;

  private:
    unsigned _width = 0;
    unsigned _height = 0;
    CellBufferType _pingPongBufferA;
    CellBufferType _pingPongBufferB;
    SurfaceBufferType _bufferSurface;
};

#endif // _BUFFER_H
