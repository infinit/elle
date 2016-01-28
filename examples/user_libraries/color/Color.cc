#include <color/Color.hh>

namespace color
{
  Color::Color(Intensity r,
               Intensity g,
               Intensity b)
    : r(r)
    , g(g)
    , b(b)
  {}

  Color::~Color()
  {}
}

std::ostream&
operator <<(std::ostream& out,
            color::Color const& color)
{
  return out << "rgb("
             << static_cast<int>(color.r) << ", "
             << static_cast<int>(color.g) << ", "
             << static_cast<int>(color.b)
             << ")";
}
