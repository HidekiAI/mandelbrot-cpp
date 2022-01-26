#pragma once

// Use either list<T> (lazy) or vector<T> (memory)
//#define USE_ARRAY_INDEX

#include <cstdlib>
#if defined(USE_ARRAY_INDEX)
#include <vector>
#else
#include <list>
#endif
#include <complex>
#include <algorithm> // std::for_each<>
#include <iostream>  // cout

template <class T>
class mandelbrot
{
private:
    size_t max_iters; // how deep we want the mandelbrot to be
    T x_min;
    T x_max;
    T y_min;
    T y_max;
    size_t width;  // maybe rename this to screen_width
    size_t height; // screen_height?

public:
    mandelbrot(
        const size_t &_max_iters,
        const T &_x_min,
        const T &_x_max,
        const T &_y_min,
        const T &_y_max,
        const size_t &_width,
        const size_t &_height)
    {
        max_iters = _max_iters;
        x_min = _x_min;
        x_max = _x_max;
        y_min = _y_min;
        y_max = _y_max;
        width = _width;
        height = _height;
    }

    ~mandelbrot()
    {
    }

public:
#if defined(USE_ARRAY_INDEX)
    const std::vector<std::vector<size_t>> calculate_mandelbrot()
#else
    const std::list<std::list<size_t>> calculate_mandelbrot()
#endif
    {
        // NOTE: The nested iteration probably do not need to look this complicated if lambdas
        // aren't used, but I wanted to (at one point) use std::for_each() with lambdas
        // and it ended up this way; it was just that using std::range (and valarray) made it
        // look more complicated than it should (compared to other functional language syntax)
        // and I felt I was bending over backwards, so I settled for the classic
        // for(for()) nested loop and be done with it...

        const auto calc =
            //[&mandelbrot_at_point]  // capture local lambda, or else you'll get "xyz is not captured"
            [](mandelbrot<T> &pThis, const size_t &_img_x, const size_t &_img_y) // using '[&]' is an alternative to passing pThis, but I prefer this way
        {
            // Still new to C++ (well, coming back after many years) and don't know whether embedding lambdas inside lambdas causes instantiation
            // performance issue or not; in other languages such as F#, I tend to localize multiple nested lambdas specific to its own purpose
            // and am unsure if this is good idea (performance-wise) or not yet...
            const auto mandelbrot_at_point =
                [](const T &cx, const T &cy, const size_t &max_iters)
            {
                std::complex<T> z = {0.0, 0.0};         // just in case we don't trust that default is (0, 0)
                //const auto c = std::complex<T>(cx, cy); // immutable
                const std::complex<T> c = {cx, cy}; // immutable
                const T minNorm = 2.0;
                for (size_t i = 0; i <= max_iters; ++i)
                {
                    // note, can probably use abs(z) instead of std::norm(z) for performance? (though compared to rust, C++ seems to be 2X faster)
                    //if (std::norm(z) > minNorm) // std::norm() is one of those that is static and it's not z.norm() member function...
                    if (abs(z) > minNorm) // using abs() instead of std::norm() results in small/slight diff (shifts image a bit to right)
                    {
                        return i;
                    }
                    z = z * z + c;
                }
                // couldn't find, so just return the max value
                return max_iters;
            };

            const T cx = pThis.x_min + (pThis.x_max - pThis.x_min) * (static_cast<T>(_img_x) / static_cast<T>(pThis.width));
            const T cy = pThis.y_min + (pThis.y_max - pThis.y_min) * (static_cast<T>(_img_y) / static_cast<T>(pThis.height));
            const auto escaped_at = mandelbrot_at_point(cx, cy, pThis.max_iters);
            return escaped_at;
        };

#if defined(USE_ARRAY_INDEX)
        auto all_rows = std::vector<std::vector<size_t>>(height); // NOTE: on rust, this is initialized with width
        auto row = std::vector<size_t>(width);
#else
        std::list<std::list<size_t>> all_rows;            // we'll uses push_back()
        std::list<size_t> row;                            // using push_back()
#endif
        for (auto img_y = 0; img_y < height; ++img_y)
        {
            //const auto result = calcOuter(*this, img_y);
            for (auto img_x = 0; img_x < width; ++img_x)
            {
#if defined(USE_ARRAY_INDEX)
                row[img_x] = calc(*this, img_x, img_y);
#else
                row.push_back(calc(*this, img_x, img_y)); // cannot use push_back (but rather, swap/replace via index) if list is preallocated
#endif
            }
#if defined(USE_ARRAY_INDEX)
            all_rows[img_y] = row;
#else
            all_rows.push_back(row);                      // cannot use push_back (but rather, swap/replace via index) if list is preallocated
            row.clear();                                  // have to clear (unlike with vector) or else it'll keep appending
#endif
        }
        return all_rows;
    };

#if defined(USE_ARRAY_INDEX)
    void render_mandelbrot(const std::vector<std::vector<size_t>> &escape_vals) const
#else
    void render_mandelbrot(const std::list<std::list<size_t>> &escape_vals) const
#endif
    {
        std::for_each(
            escape_vals.begin(),
            escape_vals.end(),
#if defined(USE_ARRAY_INDEX)
            [](const std::vector<size_t> &row)
#else
            [](const std::list<size_t> &row)
#endif
            {
                // we'll use stringstream in hopes for better performance
                std::stringstream line{};
                std::for_each(
                    row.begin(),
                    row.end(),
                    [&](const size_t &column) // there are two ways, one is to let the lambda return char in which we'd stream via 'line<<result' or just pass line (via '[&]') and let it stream inside the lambda
                    {
                        // wish I had pattern matching in C++
                        if (column <= 2)
                            line << " ";
                        else if (column > 2 && column <= 5)
                            line << ".";
                        else if (column > 5 && column <= 10)
                            line << "•";
                        else if (column > 11 && column <= 30)
                            line << "*";
                        else if (column > 30 && column <= 100)
                            line << "+";
                        else if (column > 100 && column <= 200)
                            line << "x";
                        else if (column > 200 && column <= 400)
                            line << "$";
                        else if (column > 400 && column <= 700)
                            line << "#";
                        else
                            line << "%";
                    });
                std::cout << line.str() << std::endl;
                line.flush();
            });
    };
};