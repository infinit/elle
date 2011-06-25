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
// updated       julien quintard   [sat jun 25 13:39:25 2011]
//

#ifndef ETOILE_GEAR_CHRONICLE_HH
#define ETOILE_GEAR_CHRONICLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/gear/Event.hh>

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
      /* XXX
      template <typename... T>
      elle::Status	Record(const Operation,
			       const T&...);
      */

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Container		container;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/gear/Chronicle.hxx>

//
// ---------- includes --------------------------------------------------------
//

// XXX

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
