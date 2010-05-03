//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/Format.hh
//
// created       julien quintard   [tue apr  6 14:32:46 2010]
// updated       julien quintard   [mon may  3 12:49:35 2010]
//

#ifndef ETOILE_CONTEXT_FORMAT_HH
#define ETOILE_CONTEXT_FORMAT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- types -----------------------------------------------------------
//

    typedef elle::Natural8	Format;

//
// ---------- constants -------------------------------------------------------
//

    const Format		FormatObject = 0x1;
    const Format		FormatFile = FormatObject | 0x2;
    const Format		FormatDirectory = FormatObject | 0x4;
    const Format		FormatLink = FormatObject | 0x8;

  }
}

#endif
