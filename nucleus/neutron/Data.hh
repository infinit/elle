//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [tue aug  4 06:54:28 2009]
//

#ifndef NUCLEUS_NEUTRON_DATA_HH
#define NUCLEUS_NEUTRON_DATA_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <nucleus/neutron/Offset.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- forward declarations --------------------------------------------
//

    template <typename T>
    class Contents;

  }

  namespace neutron
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a file's content.
    ///
    /// note that the Data does not derive the Block class. indeed, the
    /// Contents class represents the container for genre-specific content:
    /// Catalog for directories, Data for files etc.
    ///
    class Data:
      public elle::Object
    {
    public:
      //
      // constructors & destructors
      //
      Data(proton::Contents<Data>&);

      //
      // methods
      //

      elle::Status      Create();

      elle::Status      Write(const Offset&,
                              const elle::Region&);
      elle::Status      Read(const Offset&,
                             const Size&,
                             elle::Region&) const;
      elle::Status      Adjust(const Size&);

      elle::Status      Capacity(Size&) const;

      //
      // interfaces
      //

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status      Serialize(elle::Archive&) const;
      elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      proton::Contents<Data>&           contents;

      elle::Region                      region;
    };

  }
}

#endif
