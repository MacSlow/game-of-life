#include <random>
#include "buffer.h"

using namespace std;

#define NUM_CHANNELS 4

Buffer::Buffer (unsigned width, unsigned height)
    : _width (width), _height (height)
{
    size_t size = _width * _height;
    _pingPongBufferA = CellBuffer (size);
    _pingPongBufferB = CellBuffer (size);
    _bufferSurface = SurfaceBuffer (size);

    reset ();
}

void Buffer::reset ()
{
    random_device rnd;
    uniform_int_distribution<> dist (0, 1);
    float radius = static_cast<float> (_width / 3);

    for (unsigned y = 0; y < _height; ++y) {
        for (unsigned x = 0; x < _width; ++x) {
            float cx = static_cast<float> (x) - static_cast<float> (_width / 2);
            float cy =
                static_cast<float> (y) - static_cast<float> (_height / 2);
            float dst = hypot (cx, cy);

            _pingPongBufferA[x + y * _width] =
                dst < radius ? static_cast<CellState> (dist (rnd)) : 0;

            _pingPongBufferB[x + y * _width] = _pingPongBufferA[x + y * _width];

            _bufferSurface[x + y * _width] =
                _pingPongBufferA[x + y * _width] ? 255 : 0;
        }
    }
}

int getNeighbourhoodSum (const CellBuffer& buffer, unsigned index,
                         unsigned width)
{
    int sum = 0;

    // top row
    sum += buffer[index - width - 1] ? 1 : 0;
    sum += buffer[index - width] ? 1 : 0;
    sum += buffer[index - width + 1] ? 1 : 0;

    // middle row, the center cell is intentionally not taken into accout
    sum += buffer[index - 1] ? 1 : 0;
    sum += buffer[index + 1] ? 1 : 0;

    // bottom row
    sum += buffer[index + width - 1] ? 1 : 0;
    sum += buffer[index + width] ? 1 : 0;
    sum += buffer[index + width + 1] ? 1 : 0;

    return sum;
}

void Buffer::update ()
{
    for (unsigned y = 1; y < _height - 1; ++y) {
        for (unsigned x = 1; x < _width - 1; ++x) {
            int index = x + y * _width;

            _bufferSurface[index] =
                _pingPongBufferA[index]
                    ? 255
                    : _bufferSurface[index] > 2 ? _bufferSurface[index] - 3 : 0;

            int sum = getNeighbourhoodSum (_pingPongBufferA, index, _width);

            // die of underpopulation
            if (sum < 2) {
                _pingPongBufferB[index] = false;
            }

            // sustain life
            if (sum == 2 || sum == 3) {
                _pingPongBufferB[index] =
                    _pingPongBufferA[index] ? true : false;
            }

            // spawn life by reproduction
            if (sum == 3 && !_pingPongBufferA[index]) {
                _pingPongBufferB[index] = true;
            }

            // die of overpopulation
            if (sum > 3) {
                _pingPongBufferB[index] = false;
            }
        }
    }

    swap (_pingPongBufferA, _pingPongBufferB);
}

void Buffer::paint (const SDL_Window* window) const
{
    unsigned pitch = _width * NUM_CHANNELS;
    unsigned size = _height * pitch;
    std::vector<unsigned char> buffer;
    buffer.reserve (size);

    unsigned indexSurface = 0;
    unsigned indexBuffer = 0;
    unsigned char value = 0;

    for (unsigned y = 0; y < _height; ++y) {
        for (unsigned x = 0; x < _width; ++x) {
            indexSurface = x * NUM_CHANNELS + y * pitch;
            indexBuffer = x + y * _width;
            value = _bufferSurface[indexBuffer];
            buffer[indexSurface] = value;
            buffer[indexSurface + 1] = value;
            buffer[indexSurface + 2] = value;
            buffer[indexSurface + 3] = 255;
        }
    }

    SDL_Surface* src = nullptr;
    src = SDL_CreateRGBSurfaceWithFormatFrom (
        buffer.data (), _width, _height, 32, pitch, SDL_PIXELFORMAT_RGB888);
    SDL_Surface* dst = SDL_GetWindowSurface (const_cast<SDL_Window*> (window));
    SDL_BlitSurface (src, NULL, dst, NULL);
    SDL_FreeSurface (src);
    SDL_UpdateWindowSurface (const_cast<SDL_Window*> (window));
}
