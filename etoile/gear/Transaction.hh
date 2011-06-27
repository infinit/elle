//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Transaction.hh
//
// created       julien quintard   [fri jun  3 12:19:03 2011]
// updated       julien quintard   [sun jun 26 11:42:45 2011]
//

#ifndef ETOILE_GEAR_TRANSACTION_HH
#define ETOILE_GEAR_TRANSACTION_HH

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

/* XXX
    ///
    /// a transaction represents an atomic event performed on a scope.
    ///
    template <const Operation O,
	      typename... T>
    class Transaction:
      public Event
    {
    };
*/

  }
}

//
// ---------- templates -------------------------------------------------------
//

// XXX #include <etoile/gear/Transaction.hxx>

#endif

/* XXX

   Transaction(automaton::Directory::Load,
               arg1, arg2);

   Transaction.Apply(context);

   Transaction(archive);

   Transaction.Apply(context); // comment connaitre le type de context?


XXX se demerder pour generer une trace des operations => utile pour debug
 plus tard si un probleme chez quelqu'un, suffira d'activer le tracage et
 hop

XXX binaire pour prendre un ensemble de transactions et les relancer
 le faire generique dans elle/utility et l'appeler Diary

XXX Diary: vu que les types sont non Elle-specific (info_t etc.) on
  utiliser de la serialization de region. en fait surement pas possible
  de le faire generique.

XXX faire des options specifiques pour PIG et les autres

 */
