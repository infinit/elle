#ifndef COLOR_COLOR_HH
# define COLOR_COLOR_HH

# include <ostream>

namespace color
{
  struct Color
  {
    typedef uint8_t Intensity;
    Color(Intensity r = 0,
          Intensity g = 0,
          Intensity b = 0);

    Color(Color const&) = default;

    virtual
    ~Color();

    Intensity r, g, b;
  };
}

std::ostream&
operator <<(std::ostream& out,
            color::Color const& color);

#endif
