#ifndef  ELLE_NETWORK_LOCUS_HXX
# define ELLE_NETWORK_LOCUS_HXX

# include <cassert>
# include <functional>

# include <elle/idiom/Close.hh>
# include <boost/functional/hash.hpp>
# include <elle/idiom/Open.hh>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(elle::network::Locus,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.host;
  archive & value.port;
}

namespace std
{
  template<>
  class hash<elle::network::Locus> {
  public:
    size_t operator()(elle::network::Locus const& locus) const
    {
      std::string host;
      locus.host.Convert(host);
      size_t res(0);
      boost::hash_combine(res, std::hash<std::string>()(host));
      boost::hash_combine(res, std::hash<elle::network::Port>()(locus.port));
      return res;
    }
  };
}

#endif
