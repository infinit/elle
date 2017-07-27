#include <elle/filesystem/path.hh>

#include <boost/algorithm/string/classification.hpp> // is_digit
#include <boost/algorithm/string/predicate.hpp> // all

namespace elle
{
  fs::path base(fs::path const& path)
  {
    auto const ext = path.extension();
    if (ext.empty())
      err("base: path without extension %s", path);
    else if (ext == ".")
      err("base: path with empty extension %s", path);
    else if (!boost::all(ext.string().substr(1), boost::is_digit()))
      err("base: path whose extension is not a number: %s", path);
    else
      return path.parent_path() / path.stem();
  }

  namespace serialization
  {
    auto
    Serialize<fs::path>::convert(fs::path& path)
      -> Type
    {
      // `generic_string` always uses `/` (vs `\`).
      return path.generic_string();
    }

    auto
    Serialize<fs::path>::convert(Type const& repr)
      -> fs::path
    {
      return repr;
    }
  }
}
