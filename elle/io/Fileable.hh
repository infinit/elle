//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Fileable.hh
//
// created       julien quintard   [fri apr 30 17:35:00 2010]
// updated       julien quintard   [thu may 27 16:24:28 2010]
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
    /// to make the objects fileable i.e capable to be stored and loaded
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
      Status		Load(const String&)
      {
	enter();

	escape("this method should never have been called");
      }

      ///
      /// this method stores the object in the given file.
      ///
      Status		Store(const String&)
      {
	enter();

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
