#include <iostream>

#include <geometry/Square.hh>
#include <color/Color.hh>

struct ColoredSquare
  : public geometry::Square
  , public color::Color
{
  ColoredSquare(geometry::Square const& square,
                color::Color const& color)
    : geometry::Square(square)
    , color::Color(color)
  {}

  ColoredSquare(int width,
                int height,
                color::Color::Intensity r,
                color::Color::Intensity g,
                color::Color::Intensity b)
    : ColoredSquare(geometry::Square(width, height),
                    color::Color(r, g, b))
  {}
};

std::ostream&
operator <<(std::ostream& out,
            ColoredSquare const& object)
{
  out << static_cast<geometry::Square const&>(object)
      << " colored in "
      << static_cast<color::Color const&>(object);
  return out;
}

int
main(int argc, char** argv)
{
  ColoredSquare some_shape{230, 180, 80, 255, 183};
  geometry::Square square{256, 256};
  color::Color red{255, 0, 0};
  ColoredSquare another_shape{square, red};

  std::cout << "Some shape: " << some_shape << std::endl;
  std::cout << "Another shape: " << another_shape << std::endl;

  return 0;
}
