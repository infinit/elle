#ifndef  NUCLEUS_DERIVABLE_HXX
# define NUCLEUS_DERIVABLE_HXX

# include <stdexcept>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <nucleus/Derivable.hh>
# include <nucleus/Nucleus.hh>

namespace elle
{
  namespace serialize
  {

    template<> struct StoreClassVersion<nucleus::Derivable>
      { static bool const value = false; };

  }
}

ELLE_SERIALIZE_SPLIT(nucleus::Derivable);

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::Derivable, archive, value, version)
{
  assert(value.kind == nucleus::Derivable::Kind::output);
  assert(version == 0);
  archive >> value._component;

  if (value._dynamic_construct)
    {
      assert(value._block == nullptr);
      auto res = nucleus::Nucleus::Factory.Build(value._component, value._block);
      if (res == elle::Status::Error)
        throw std::runtime_error("Cannot build the block");
    }
  assert(value._block != nullptr);
  typedef typename elle::concept::SerializableFor<Archive>::Type interface_t;
  assert(dynamic_cast<interface_t*>(value._block) != nullptr);
  static_cast<interface_t&>(*value._block).deserialize(archive);
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::Derivable, archive, value, version)
{
  assert(version == 0);
  assert(value._block != nullptr);
  archive << value._component;
  archive << elle::serialize::serializable(*value._block);
}

#endif
