//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Manifest.hh
//
// created       julien quintard   [tue feb 23 21:35:44 2010]
// updated       julien quintard   [tue feb 23 21:40:01 2010]
//

#ifndef ELLE_NETWORK_MANIFEST_HH
#define ELLE_NETWORK_MANIFEST_HH

namespace elle
{
  namespace network
  {

//
// ---------- macro functions -------------------------------------------------
//

#define Manifest(_tag_, _parameters_...)				\
  namespace elle							\
  {									\
    namespace network							\
    {									\
      template <>							\
      class Message<_tag_>						\
      {									\
      public:								\
        typedef Parameters<_parameters_>	P;			\
      };								\
    }									\
  }

  }
}

#endif
