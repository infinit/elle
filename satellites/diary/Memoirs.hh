//
// ---------- header ----------------------------------------------------------
//
// project       diary
//
// license       infinit
//
// author        julien quintard   [fri dec 23 13:27:57 2011]
//

#ifndef DIARY_MEMOIRS_HH
#define DIARY_MEMOIRS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/Fileable.hh>
#include <elle/io/Dumpable.hh>

#include <elle/types.hh>
//#include <elle/radix.hh>

namespace satellite
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents the diary in its archived form.
  ///
  class Memoirs:
    public elle::Object,
    public elle::io::Dumpable,
    public elle::io::Fileable<Memoirs>
  {
  public:
    //
    // interfaces
    //

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // archivable
    //elle::Status        Serialize(elle::Archive&) const;
    //elle::Status        Extract(elle::Archive&);

    // fileable
    //elle::Status        Load(const elle::Path&);
    //elle::Status        Store(const elle::Path&) const;

    //
    // attributes
    //
    elle::Archive               archive;
  };

}

#endif
