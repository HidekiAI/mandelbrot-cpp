#include "mandelbrot.hpp"

void main()
{
    auto mandelbrot = new mandelbrot<double>(1000, -2.0, 1.0, -1.0, 1.0, 100, 24);
    auto myList = mandelbrot.calculate_mandelbrot();
    mandelbrot.render_mandelbrot(myList);
}

