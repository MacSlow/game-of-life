#include "display.h"

#define WIDTH 512
#define HEIGHT 512

int main (int argc, char** argv)
{
    Application app (WIDTH, HEIGHT);
    app.run ();

    return 0;
}
