# include <elle/filesystem.hh>

namespace std
{
  std::size_t
  hash<boost::filesystem::path>::operator()(
    boost::filesystem::path const& p) const
  {
    return hash_value(p);
  }
}
