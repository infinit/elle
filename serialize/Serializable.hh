#ifndef  ELLE_SERIALIZE_SERIALIZABLE_HH
# define ELLE_SERIALIZE_SERIALIZABLE_HH

# include <elle/serialize/fwd.hh>
# include <elle/serialize/ArchiveMode.hh>

namespace elle
{
  namespace serialize
  {

    template<template<ArchiveMode> class Archive = BinaryArchive>
    class Serializable
    {
    public:
      typedef Archive<ArchiveMode::Input>   InputArchive;
      typedef Archive<ArchiveMode::Output>  OutputArchive;

    public:
      virtual void LoadFrom(InputArchive& archive) = 0;
      virtual void SaveTo(OutputArchive& archive) const = 0;
    };

  }
}

#endif


