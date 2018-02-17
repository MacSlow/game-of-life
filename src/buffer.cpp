#include <random>
#include "buffer.h"

using namespace std;

#define NUM_CHANNELS 4
#define NO_INTENSITY 0
#define FULL_INTENSITY 255

Buffer::Buffer (size_t width, size_t height)
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

    for (size_t y = 0; y < _height; ++y) {
        for (size_t x = 0; x < _width; ++x) {
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

size_t getNeighbourhoodPopulation (const CellBuffer& buffer, size_t index,
                                   size_t width)
{
    size_t population = 0;

    // top row
    population += buffer[index - width - 1] ? 1 : 0;
    population += buffer[index - width] ? 1 : 0;
    population += buffer[index - width + 1] ? 1 : 0;

    // middle row, the center cell is intentionally not taken into accout
    population += buffer[index - 1] ? 1 : 0;
    population += buffer[index + 1] ? 1 : 0;

    // bottom row
    population += buffer[index + width - 1] ? 1 : 0;
    population += buffer[index + width] ? 1 : 0;
    population += buffer[index + width + 1] ? 1 : 0;

    return population;
}

void Buffer::update ()
{
    for (size_t y = 1; y < _height - 1; ++y) {
        for (size_t x = 1; x < _width - 1; ++x) {
            size_t index = x + y * _width;

            _bufferSurface[index] = _pingPongBufferA[index]
                                        ? FULL_INTENSITY
                                        : _bufferSurface[index] > 2
                                              ? _bufferSurface[index] - 3
                                              : NO_INTENSITY;

            auto population =
                getNeighbourhoodPopulation (_pingPongBufferA, index, _width);

            // die of underpopulation
            if (population < 2) {
                _pingPongBufferB[index] = false;
            }

            // sustain life
            if (population == 2 || population == 3) {
                _pingPongBufferB[index] =
                    _pingPongBufferA[index] ? true : false;
            }

            // spawn life by reproduction
            if (population == 3 && !_pingPongBufferA[index]) {
                _pingPongBufferB[index] = true;
            }

            // die of overpopulation
            if (population > 3) {
                _pingPongBufferB[index] = false;
            }
        }
    }

    swap (_pingPongBufferA, _pingPongBufferB);
}

void Buffer::paint (const SDL_Window* window) const
{
    size_t pitch = _width * NUM_CHANNELS;
    size_t size = _height * pitch;
    std::vector<unsigned char> buffer;
    buffer.reserve (size);

    size_t indexSurface = 0;
    size_t indexBuffer = 0;
    unsigned char value = 0;

    for (size_t y = 0; y < _height; ++y) {
        for (size_t x = 0; x < _width; ++x) {
            indexSurface = x * NUM_CHANNELS + y * pitch;
            indexBuffer = x + y * _width;
            value = _bufferSurface[indexBuffer];
            buffer[indexSurface] = value;
            buffer[indexSurface + 1] = value;
            buffer[indexSurface + 2] = value;
            buffer[indexSurface + 3] = FULL_INTENSITY;
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
