//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [wed jul  6 09:12:10 2011]
//

#ifndef NUCLEUS_PROTON_HISTORY_HH
#define NUCLEUS_PROTON_HISTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/Nucleus.hh>

#include <nucleus/proton/Version.hh>

#include <elle/idiom/Close.hh>
# include <vector>
#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains all the version numbers associated with
    /// a given mutable block.
    ///
    class History:
      public elle::radix::Object,
      public elle::io::Fileable<History>
    {
    public:
      //
      // constants
      //
      static const elle::String         Extension;

      //
      // types
      //
      typedef std::vector<Version>                      Container;
      typedef typename Container::iterator              Iterator;
      typedef typename Container::const_iterator        Scoutor;

      //
      // methods
      //
      elle::Status      Register(const Version&);

      elle::Status      Select(const Version::Type,
                               Version&) const;

      elle::Status      Size(Version::Type&) const;

      //
      // interfaces
      //

      // object
      declare(History);
      elle::Boolean     operator==(const History&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      //elle::Status      Serialize(elle::Archive&) const;
      //elle::Status      Extract(elle::Archive&);

      // fileable
      elle::Status      Load(const Network&,
                             const Address&);
      elle::Status      Store(const Network&,
                              const Address&) const;
      elle::Status      Erase(const Network&,
                              const Address&) const;
      elle::Status      Exist(const Network&,
                              const Address&) const;

      //
      // attributes
      //
      Container         container;
    };

  }
}

#endif
