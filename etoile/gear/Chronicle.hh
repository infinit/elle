//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Chronicle.hh
//
// created       julien quintard   [fri jun  3 11:54:04 2011]
// updated       julien quintard   [fri jun  3 14:35:41 2011]
//

#ifndef ETOILE_GEAR_CHRONICLE_HH
#define ETOILE_GEAR_CHRONICLE_HH

//
// ---------- includes --------------------------------------------------------
//

// XXX

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// a chronicle represents a sequence of operations.
    ///
    class Chronicle:
      public elle::Object
    {
    public:
      //
      // types
      //
      typedef std::list<Event*>		Container;
      typedef Container::iterator	Iterator;
      typedef Container::const_iterator	Scoutor;

      //
      // template methods
      //
      template <typename... T>
      elle::Status	Record(const Operation,
			       const T&...);

      //
      // attributes
      //
      Container		diary;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/gear/Chronicle.hxx>

#endif

/* XXX

   Network
   -------

   - method(tag, inputs)
   -> stub[archive]
   -> network/storage
   -> skeleton[archive]
   => method(inputs)

   Chronicle
   ---------

   - method(operation, inputs)
   -> stub[archive]
   -> network/storage
   -> skeleton[archive]
   => method(inputs)

donc:

Parameters est generique
Arguments devrait etre modifie pour l'etre
Network::Register devrait etre generique et utilise par Network::Dispatch
 -> cette classe generique devrait prendre un Int32 (tag dans le cas de
    network, Operation pour Chronicle) mais egalement un char* pour
    differencier network de chronicle

 */
