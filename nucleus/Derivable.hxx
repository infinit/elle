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

    template<> struct StoreClassVersion<nucleus::InputDerivable>
      { static bool const value = false; };
    template<> struct StoreClassVersion<nucleus::OutputDerivable>
      { static bool const value = false; };
    template<> struct StoreClassVersion<nucleus::Derivable>
      { static bool const value = false; };

  }
}

ELLE_SERIALIZE_SPLIT(nucleus::InputDerivable);
ELLE_SERIALIZE_SPLIT(nucleus::OutputDerivable);


ELLE_SERIALIZE_SPLIT_LOAD(nucleus::InputDerivable, archive, value, version)
{
  throw std::runtime_error("Cannot load an input derivable");
}

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::OutputDerivable, archive, value, version)
{
  assert(version == 0);
  archive >> value._component;

  if (value._dynamic_construct)
    {
      assert(value._block == nullptr);
      auto res = nucleus::Nucleus::Factory.Build(value._component, value._block);
      if (res == elle::Status::Error)
        throw std::runtime_error("Cannot build the block");
    }
  else
    {
      assert(value._block != nullptr);
      typedef typename elle::concept::SerializableFor<Archive>::Type interface_t;
      assert(dynamic_cast<interface_t*>(value._block) != nullptr);
      static_cast<interface_t&>(*value._block).deserialize(archive);
    }
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::OutputDerivable, archive, value, version)
{
  throw std::runtime_error("Cannot save an output derivable");
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::InputDerivable, archive, value, version)
{
  assert(version == 0);
  archive << value._component;
  archive << elle::serialize::serializable(value._block);
}

ELLE_SERIALIZE_SIMPLE(nucleus::Derivable, archive, value, version)
{
  if (value.kind == nucleus::Derivable::Kind::input)
    {
      archive & static_cast<nucleus::InputDerivable&>(value);
    }
  else if (value.kind == nucleus::Derivable::Kind::output)
    {
      archive & static_cast<nucleus::OutputDerivable&>(value);
    }
}


#endif
