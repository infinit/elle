#include <geometry/Square.hh>

namespace geometry
{
  Square::Square(int width,
                 int height)
    : width(width)
    , height(height)
  {}

  Square::~Square()
  {}
}

std::ostream&
operator <<(std::ostream& out,
            geometry::Square const& square)
{
  return out << square.width
             << " x "
             << square.height
             << " square";
}
