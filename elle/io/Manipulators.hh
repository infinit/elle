//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Manipulators.hh
//
// created       julien quintard   [mon jul  4 16:34:01 2011]
// updated       julien quintard   [mon jul  4 19:28:14 2011]
//

#ifndef ELLE_IO_MANIPULATORS_HH
#define ELLE_IO_MANIPULATORS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <elle/io/Unique.hh>
#include <elle/io/Uniquable.hh>
#include <elle/io/Format.hh>

#include <elle/idiom/Close.hh>
# include <iostream>
# include <sstream>
# include <string>
#include <elle/idiom/Open.hh>

namespace std
{

//
// ---------- functions -------------------------------------------------------
//

  elle::core::String	chop(const elle::core::String&,
			     const elle::core::Natural32 = 50);
  template <const elle::io::Format F>
  elle::core::String	chop(const elle::io::Uniquable<F>&,
			     const elle::core::Natural32 = 50);

}

#endif
