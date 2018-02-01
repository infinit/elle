#include <geometry/Shape.hh>

namespace geometry
{
  Shape::Shape()
  {}

  Shape::~Shape()
  {}
};

std::ostream&
operator <<(std::ostream& out,
            geometry::Shape const& color)
{
  return out << "random shape";
}
