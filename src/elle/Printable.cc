#include <boost/lexical_cast.hpp>

#include <elle/Printable.hh>

namespace elle
{
  std::string
  Printable::stringify() const
  {
    return boost::lexical_cast<std::string>(*this);
  }

  std::ostream&
  operator << (std::ostream& stream,
               Printable const& o)
  {
    o.print(stream);
    return stream;
  }
}
