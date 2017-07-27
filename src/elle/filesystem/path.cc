#include <elle/filesystem/path.hh>

#include <boost/algorithm/string/classification.hpp> // is_digit
#include <boost/algorithm/string/predicate.hpp> // all
#include <boost/filesystem.hpp>
#include <boost/range/algorithm/sort.hpp>

#include <elle/algorithm.hh> // tail
#include <elle/from-string.hh>

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

  std::vector<fs::path>
  entries(fs::path const& d)
  {
    auto res = std::vector<fs::path>{};
    for (auto const& p: fs::directory_iterator(d))
      res.emplace_back(p.path());
    return res;
  }

  std::vector<int>
  rotate_versions(fs::path const& base)
  {
    auto res = std::vector<int>{};
    auto const dir = base.parent_path().empty() ? "." : base.parent_path();
    // Use `generic_string` to avoid gratuitious differences on `/` vs `\`.
    auto const prefix = base.generic_string() + ".";
    for (auto& p: fs::directory_iterator(dir))
      if (auto n = elle::tail(p.path().generic_string(), prefix))
        try
        {
          res.emplace_back(from_string<int>(*n));
        }
        catch (std::invalid_argument)
        {}
    boost::sort(res);
    return res;
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

namespace std
{
  std::size_t
  hash<boost::filesystem::path>::operator()(
    boost::filesystem::path const& p) const
  {
    return hash_value(p);
  }
}
