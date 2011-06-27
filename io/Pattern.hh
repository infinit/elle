//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Pattern.hh
//
// created       julien quintard   [mon apr 25 10:59:05 2011]
// updated       julien quintard   [mon jun 27 21:23:23 2011]
//

#ifndef ELLE_IO_PATTERN_HH
#define ELLE_IO_PATTERN_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace io
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a path pattern.
    ///
    /// although the common syntax considers the string %name% as
    /// representing a variable component, programmers are welcome to use
    /// the syntax they prefer.
    ///
    class Pattern:
      public Object
    {
    public:
      //
      // methods
      //
      Status		Create(const String&);

      //
      // interfaces
      //

      // object
      declare(Pattern);
      Boolean		operator==(const Pattern&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable: nothing

      //
      // attributes
      //
      String		string;
    };

  }
}

#endif
