#include "mandelbrot.hpp"

int main()
{
    auto myMandelbrot = mandelbrot<double>(1000, -2.0, 1.0, -1.0, 1.0, 100, 24);
    auto myList = myMandelbrot.calculate_mandelbrot();
    myMandelbrot.render_mandelbrot(myList);
    return 0;
}
