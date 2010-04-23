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
// updated       julien quintard   [thu apr 22 11:05:09 2010]
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
    const Format		FormatFile = FormatObject | 0x2;
    const Format		FormatDirectory = FormatObject | 0x4;
    const Format		FormatLink = FormatObject | 0x8;

  }
}

#endif
