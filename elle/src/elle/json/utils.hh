#ifndef ELLE_JSON_UTILS_HH
# define ELLE_JSON_UTILS_HH

# include <unordered_map>
# include <vector>

# include <boost/any.hpp>

namespace elle
{
  namespace json
  {
    typedef std::vector<boost::any> Array;
    typedef std::unordered_map<std::string, boost::any> Object;

    class NullType
    {};

    Object
    read_json(std::istream& stream);

    void
    write_json(std::ostream& stream,
               boost::any const& any);

    std::string
    pretty_print_json(boost::any const& any);
  }
}

#endif
