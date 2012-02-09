//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri apr 30 17:35:00 2010]
//

#ifndef ELLE_IO_FILEABLE_HH
#define ELLE_IO_FILEABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/radix/Status.hh>

#include <elle/io/Format.hh>
#include <elle/io/Path.hh>

namespace elle
{
  using namespace standalone;
  using namespace radix;

  namespace io
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this interface must be inherited by every class which wishes
    /// to make its instances fileable i.e storable and loadable
    /// to/from files.
    ///
    template <const Format F>
    class Fileable
    {
    public:
      //
      // methods
      //

      ///
      /// this method loads an object and reconstructs it in memory.
      ///
      Status            Load(const Path&)
      {
        ;

        escape("this method should never have been called");
      }

      ///
      /// this method stores the object in the given file.
      ///
      Status            Store(const Path&) const
      {
        ;

        escape("this method should never have been called");
      }

      ///
      /// this method erases the given file.
      ///
      Status            Erase(const Path&) const
      {
        ;

        escape("this method should never have been called");
      }

      ///
      /// this method returns true if the given file exists.
      ///
      Status            Exist(const Path&) const
      {
        ;

        escape("this method should never have been called");
      }
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/io/Fileable.hxx>

#endif
