#include "mandelbrot.hpp"

int main()
{
    const auto width = 100;
    const auto height = 24;
    auto myMandelbrot = mandelbrot<double>(1000, -2.0, 1.0, -1.0, 1.0, width, height);
    auto myList = myMandelbrot.calculate_mandelbrot();
    myMandelbrot.render_mandelbrot(myList);
    return 0;
}
