#include "display.h"

#define WIDTH 512
#define HEIGHT 512

int main (int argc, char** argv)
{
    Display display (WIDTH, HEIGHT);
    display.run ();

    return 0;
}
