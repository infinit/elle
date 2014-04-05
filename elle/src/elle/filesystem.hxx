#ifndef ELLE_FILESYSTEM_HXX
# define ELLE_FILESYSTEM_HXX

namespace std
{
  std::size_t
  hash<boost::filesystem::path>::operator()(
    boost::filesystem::path const& p) const
  {
    return hash_value(p);
  }
}

#endif
