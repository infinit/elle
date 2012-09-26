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
        archive & value._meta._id;
        archive & value._meta._administrator_K;
        archive & value._meta._model;
        archive & value._meta._root;
        archive & value._meta._everybody.identity;
        archive & value._meta._history;
        archive & value._meta._extent;
        archive & value._meta._signature;

        archive & value._data._name;
        archive & value._data._openness;
        archive & value._data._policy;
        archive & value._data._version;
        archive & value._data._formats.block;
        archive & value._data._formats.content_hash_block;
        archive & value._data._formats.contents;
        archive & value._data._formats.immutable_block;
        archive & value._data._formats.imprint_block;
        archive & value._data._formats.mutable_block;
        archive & value._data._formats.owner_key_block;
        archive & value._data._formats.public_key_block;
        archive & value._data._formats.access;
        archive & value._data._formats.attributes;
        archive & value._data._formats.catalog;
        archive & value._data._formats.data;
        archive & value._data._formats.ensemble;
        archive & value._data._formats.group;
        archive & value._data._formats.object;
        archive & value._data._formats.reference;
        archive & value._data._formats.user;
        archive & value._data._formats.identity;
        archive & value._data._formats.descriptor;
        archive & value._data._signature;

        break;
      }
    default:
      throw std::runtime_error(
        elle::sprintf("unknown format '%s'", version));
    }
}

/* XXX
template <typename T, typename... Args>
struct Upgrader<T, i, Args>
{
  void do_it(int from)
    {
      if (i >= from)
        {
          up()
        }
      Upgrader<i + 1>::do_it(from);
    }
  void up(T, Args... args)
  {
  }

};

upgrade<2>(val);

{
  switch (val.format())
    {
    case 0:
    }
}

ELLE_SERIALIZE_UPGRADE_FORMAT(lune::Descriptor, value, 1, var)
{

}
*/

#endif
