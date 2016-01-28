#ifndef GEOMETRY_SQUARE_HH
# define GEOMETRY_SQUARE_HH

# include <ostream>

# include <geometry/Shape.hh>

namespace geometry
{
  struct Square
    : public Shape
  {
    Square(int width = 1,
           int height = 1);

    Square(Square const&) = default;

    virtual
    ~Square();

    int width, height;
  };
}

std::ostream&
operator <<(std::ostream& out,
            geometry::Square const& square);

#endif
