//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/idiom.hh/Open.hh
//
// created       julien quintard   [sat mar 20 15:18:25 2010]
// updated       julien quintard   [sat mar 20 15:24:04 2010]
//

//
// ---------- macro-functions -------------------------------------------------
//

///
/// these macro-functions are used to make using the Etoile functionalities
/// easier.
///

//
// ---------- network ---------------------------------------------------------
//

///
/// this macro-function is used to reply to a request by sending an
/// negative result, along with a report.
///
#define reply(_socket_, _tag_, _text_)					\
  do									\
    {									\
      ::etoile::wall::Result	result(StatusError,			\
				       ::elle::misc::report);		\
									\
      report(elle::misc::Report::TypeError, _text_);			\
									\
      if ((_socket_).Reply(Inputs< _tag_ >(result)) == StatusError)	\
	escape("unable to reply");					\
									\
      leave();								\
    } while (false)
