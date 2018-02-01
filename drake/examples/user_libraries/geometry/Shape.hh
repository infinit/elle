#ifndef GEOMETRY_COLOR_HH
# define GEOMETRY_COLOR_HH

# include <ostream>

namespace geometry
{
  struct Shape
  {
    Shape();

    virtual
    ~Shape();
  };
}

std::ostream&
operator <<(std::ostream& out,
            geometry::Shape const& color);

#endif
