//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/context/Context.hh
//
// created       julien quintard   [fri aug 14 22:36:10 2009]
// updated       julien quintard   [fri apr 16 09:37:50 2010]
//

#ifndef ETOILE_CONTEXT_CONTEXT_HH
#define ETOILE_CONTEXT_CONTEXT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/user/Client.hh>

#include <etoile/context/Identifier.hh>
#include <etoile/context/Format.hh>

#include <etoile/journal/Bucket.hh>

namespace etoile
{
  ///
  /// this namespace contains classes for manipulating contexts i.e
  /// state related to a sequence of operations.
  ///
  namespace context
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// a context keeps the state required by operations to sequentially
    /// execute.
    ///
    /// every context is identified by an identifier which is used by
    /// applications. however, in order to prevent attacks, the client,
    /// hence the user, is also associated with the context.
    ///
    /// this way, anoter used cannot attack the identifiers through
    /// brute force while the proper user can very well pass the identifiers
    /// between its processes, thus benefiting from improved performance.
    ///
    class Context:
      public Dumpable
    {
    public:
      //
      // types
      //
      typedef std::pair<user::Client*, Context*>	Value;
      typedef std::map<const Identifier, Value>		Container;
      typedef Container::iterator			Iterator;
      typedef Container::const_iterator			Scoutor;

      //
      // static methods
      //
      template <typename T>
      static Status	Add(T*);
      template <typename T>
      static Status	Retrieve(const Identifier&,
				 T*&);
      template <typename T>
      static Status	Remove(T*);

      //
      // static attributes
      //
      static Container		Contexts;

      //
      // constructors & destructors
      //
      Context(const Format&);

      //
      // methods
      //
      Status		Identify(const Identifier&);

      //
      // attributes
      //
      Format		format;
      Identifier	identifier;

      journal::Bucket	bucket;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/context/Context.hxx>

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Object.hh>
#include <etoile/context/Directory.hh>
#include <etoile/context/Rights.hh>

#include <etoile/user/User.hh>

#endif
