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
// updated       julien quintard   [mon may  3 00:21:11 2010]
//

#ifndef ELLE_IO_UNIQUABLE_HH
#define ELLE_IO_UNIQUABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/archive/Archivable.hh>

#include <elle/io/Format.hh>
#include <elle/io/Unique.hh>

namespace elle
{
  using namespace core;
  using namespace archive;

  namespace io
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class enables the inherited class to be represented in
    /// a unique compact string-based form.
    ///
    template <const Format F>
    class Uniquable:
      public virtual Archivable
    {
    public:
      //
      // operators
      //
      virtual Void	operator>>(Unique&			unique) const
      {
	enter();

	alert("this method should never have been called");
      }

      virtual Void	operator<<(const Unique&		unique)
      {
	enter();

	alert("this method should never have been called");
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
  std::ostream&		operator<<(
                          std::ostream&				stream,
			  const
			    elle::io::Uniquable<F>&		object);

}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/io/Uniquable.hxx>

#endif
