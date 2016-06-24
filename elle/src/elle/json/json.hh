#ifndef ELLE_JSON_JSON_HH
# define ELLE_JSON_JSON_HH

# include <map>
# include <unordered_map>
# include <vector>

# include <boost/any.hpp>

# include <elle/compiler.hh>

namespace elle
{
  namespace json
  {
    typedef boost::any Json;
    typedef std::vector<boost::any> Array;
    typedef std::unordered_map<std::string, boost::any> Object;
    typedef std::map<std::string, boost::any> OrderedObject;

    class ELLE_API NullType
    {};

    ELLE_API
    boost::any
    read(std::istream& stream);

    ELLE_API
    void
    write(std::ostream& stream, boost::any const& any, bool with_endl = true);

    ELLE_API
    std::string
    pretty_print(boost::any const& any);
  }
}

namespace std
{
  ELLE_API
  std::ostream&
  operator <<(std::ostream& stream, elle::json::Object const& obj);

  ELLE_API
  std::ostream&
  operator <<(std::ostream& stream, elle::json::OrderedObject const& obj);
}

#endif
