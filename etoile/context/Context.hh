//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/Context.hh
//
// created       julien quintard   [fri aug 14 22:36:10 2009]
// updated       julien quintard   [thu apr 22 12:59:59 2010]
//

#ifndef ETOILE_CONTEXT_CONTEXT_HH
#define ETOILE_CONTEXT_CONTEXT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/context/Identifier.hh>
#include <etoile/context/Format.hh>

#include <etoile/journal/Bucket.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Application;

  }

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
    class Context:
      public Dumpable
    {
    public:
      //
      // enumerations
      //
      enum Type
	{
	  TypeInternal,
	  TypeExternal
	};

      //
      // static methods
      //
      template <typename T>
      static Status	New(T*&);
      template <typename T>
      static Status	Delete(T*);

      template <typename T>
      static Status	Export(T*);
      template <typename T>
      static Status	Import(T*);

      //
      // constructors & destructors
      //
      Context(const Format&);

      //
      // methods
      //
      Status		Create();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Format			format;

      Type			type;

      user::Application*	application;
      Fiber*			fiber;

      Identifier		identifier;

      journal::Bucket		bucket;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Application.hh>

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/context/Context.hxx>

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Object.hh>
#include <etoile/context/Directory.hh>
#include <etoile/context/Link.hh>
#include <etoile/context/Rights.hh>

#include <etoile/user/User.hh>

#endif
