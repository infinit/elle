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
// updated       julien quintard   [tue apr  6 14:34:39 2010]
//

#ifndef ETOILE_CONTEXT_FORMAT_HH
#define ETOILE_CONTEXT_FORMAT_HH

namespace etoile
{
  namespace context
  {

//
// ---------- types -----------------------------------------------------------
//

    typedef Natural8		Format;

//
// ---------- constants -------------------------------------------------------
//

    const Format		FormatObject = 0x1;
    const Format		FormatFile = 0x3;
    const Format		FormatDirectory = 0x5;
    const Format		FormatLink = 0x9;

  }
}

#endif
