#pragma once
#include <cstdlib>
#include <list>
#include <vector>
#include <complex>
#include <algorithm>

template <class T>
class mandelbrot
{
private:
    size_t max_iters;
    T x_min;
    T x_max;
    T y_min;
    T y_max;
    size_t width;
    size_t height;

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
        x_max = _x_max;
        y_min = _y_min;
        y_max = _y_max;
        width = _width;
        height = _height;
    }

    ~mandelbrot()
    {
    }

private:
    const size_t mandelbrot_at_point(const T &cx, const T &cy, const size_t &max_iters)
    {
        auto z = std::complex<double>(0.0, 0.0);
        const auto c = std::complex<double>(cx, cy); // immutable
        for (auto i = 0; i <= max_iters; ++i)
        {
            if (std::norm(z) > 2.0) // std::norm() is one of those that is static and it's not z.norm() member function...
            {
                return i;
            }
            z = z * z + c;
        }
        // couldn't find, so just return the max value
        return max_iters;
    };

public:
    const std::list<std::list<size_t>> calculate_mandelbrot()
    {
        // NOTE: The nested iteration probably do not need to look this complicated if lambdas
        // aren't used, but I wanted to (at one point) use std::for_each() with lambdas
        // and it ended up this way; it was just that using std::range (and valarray) made it
        // look more complicated than it should (compared to other functional language syntax)
        // and I felt I was bending over backwards, so I settled for the classic
        // for(for()) nested loop and be done with it...
        auto all_rows = std::list<std::list<size_t>>(width);

        const auto calcOuter =
            [](mandelbrot<T> &pThis, const size_t &img_y) // using '[&]' is an alternative to passing pThis, but I prefer this way
        {
            const auto calc =
                [&](mandelbrot<T> &pThis, const size_t &img_x, const size_t &img_y) // using '[&]' is an alternative to passing pThis, but I prefer this way
            {
                const auto cx = pThis.x_min + (pThis.x_max - pThis.x_min) * (img_x / pThis.width);
                const auto cy = pThis.y_min + (pThis.y_max - pThis.y_min) * (img_y / pThis.height);
                const auto escaped_at = pThis.mandelbrot_at_point(cx, cy, pThis.max_iters);
                return escaped_at;
            };
            auto row = std::list<size_t>(pThis.height);
            //std::list<size_t> row = list(pThis.height);
            for (auto img_x = 0; img_x < pThis.width; ++img_x)
                row.push_back(calc(pThis, img_x, img_y));
            return row;
        };

        for (auto img_y = 0; img_y < height; ++img_y)
        {
            const auto result = calcOuter(*this, img_y);
            all_rows.push_back(result);
        }
        return all_rows;
    };

    void render_mandelbrot(const std::list<std::list<size_t>> &escape_vals) const
    {
        std::for_each(
            escape_vals.begin(),
            escape_vals.end(),
            [](const std::list<size_t> &row)
            {
                auto line = std::string(row.size(), ' ');
                std::for_each(
                    row.begin(),
                    row.end(),
                    [](const size_t &column)
                    {
                        // wish I had pattern matching in C++
                        auto val = u8" ";
                        if (column <= 2)
                            val = u8" ";
                        else if (column > 2 && column <= 5)
                            val = u8".";
                        else if (column > 5 && column <= 10)
                            val = u8"•";
                        else if (column > 11 && column <= 30)
                            val = u8"*";
                        else if (column > 30 && column <= 100)
                            val = u8"+";
                        else if (column > 100 && column <= 200)
                            val = u8"x";
                        else if (column > 200 && column <= 400)
                            val = u8"$";
                        else if (column > 400 && column <= 700)
                            val = u8"#";
                        else
                            val = u8"%";
                    });
            });
    };
};