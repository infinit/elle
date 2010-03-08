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
// updated       julien quintard   [fri mar  5 15:29:10 2010]
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

///
/// this macro-function defines an inward message part of the
/// interface.
///
#define Inward(_tag_, _parameters_)					\
  namespace elle							\
  {									\
    namespace network							\
    {									\
      template <>							\
      class Message< _tag_ >						\
      {									\
      public:								\
        typedef Parameters< _parameters_ >	P;			\
      };								\
    }									\
  }

///
/// this macro-function defines an outward message i.e response
/// to a previously received inward message.
///
/// note that outward messages' tag is the negative counterpart
/// of the corresponding inward message.
///
#define Outward(_tag_, _parameters_)					\
  namespace elle							\
  {									\
    namespace network							\
    {									\
      template <>							\
      class Message< _tag_ >						\
      {									\
      public:								\
        typedef Parameters< _parameters_ >	P;			\
      };								\
    }									\
  }

///
/// syntaxic sugar.
///
#define Parameters(_parameters_...)					\
  _parameters_

  }
}

#endif
