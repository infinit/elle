#ifndef  LUNE_DESCRIPTOR_HXX
# define LUNE_DESCRIPTOR_HXX

# include <cassert>

# include <elle/cryptography/Signature.hh>
# include <nucleus/proton/Address.hxx>
# include <hole/Model.hh>

ELLE_SERIALIZE_STATIC_FORMAT(lune::Descriptor, 1)

ELLE_SERIALIZE_SIMPLE(lune::Descriptor,
                      archive,
                      value,
                      version)
{
  switch (version)
    {
    case 0:
      {
        archive & value._meta._id;
        archive & value._data._name;
        archive & value._meta._model;
        archive & value._data._openness;
        archive & value._meta._root;
        archive & value._meta._everybody.identity;
        archive & value._meta._history;
        archive & value._meta._extent;

        elle::Real contention(0.5);
        elle::Real balancing(0.2);

        archive & contention;
        archive & balancing;

        archive & value._data._policy;
        archive & value._meta._signature;

        break;
      }
    case 1:
      {
        break;
      }
    default:
      throw std::runtime_error(
        elle::sprintf("unknown format '%s'", version));
    }
}

#endif
