//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Midget.hh
//
// created       julien quintard   [thu jun 23 10:25:08 2011]
// updated       julien quintard   [thu jun 23 13:26:33 2011]
//

#ifndef ELLE_IO_MIDGET_HH
#define ELLE_IO_MIDGET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/radix/Object.hh>

#include <elle/io/Format.hh>
#include <elle/io/Uniquable.hh>
#include <elle/io/Unique.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace radix;

  namespace io
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class takes an object, which inherits the uniquable interface,
    /// and makes it dumpable without exceeding a certain length.
    ///
    class Midget
    {
    public:
      //
      // constants
      //
      static const Natural32			Length = 50;

      //
      // constructors & destructors
      //
      Midget(const Object&,
	     const Natural32 = Length);

      //
      // attributes
      //
      const Object&	object;
      const Natural32	length;
    };

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ostream&		operator<<(
                          ostream&,
			  const elle::io::Midget&);

}

#endif
