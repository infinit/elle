//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Uniquable.hh
//
// created       julien quintard   [sun may  2 12:01:53 2010]
// updated       julien quintard   [thu jul 28 16:16:21 2011]
//

#ifndef ELLE_IO_UNIQUABLE_HH
#define ELLE_IO_UNIQUABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>
#include <elle/standalone/Log.hh>

#include <elle/radix/Status.hh>

#include <elle/package/Archivable.hh>

#include <elle/io/Format.hh>
#include <elle/io/Unique.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace radix;
  using namespace package;

  namespace io
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class enables the inherited class to be represented in
    /// a unique compact string-based form.
    ///
    /// besides, this unique string representation can be used to
    /// save and restore the original object from/to main memory.
    ///
    template <const Format F>
    class Uniquable:
      public virtual Archivable
    {
    public:
      //
      // methods
      //

      ///
      /// this method returns in the Unique, the string-based representation
      /// of the object's archive.
      ///
      Status		Save(Unique&) const
      {
	enter();

	escape("this method should never have been called");
      }

      ///
      /// this method reconstructs the living object based on the
      /// unique string representation.
      ///
      Status		Restore(const Unique&)
      {
	enter();

	escape("this method should never have been called");
      }
    };

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  template <const elle::io::Format F>
  ostream&		operator<<(ostream&,
				   const elle::io::Uniquable<F>&);

}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/io/Uniquable.hxx>

#endif
