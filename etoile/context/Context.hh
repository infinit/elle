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
// updated       julien quintard   [mon may  3 17:36:30 2010]
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
      public elle::Entity
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
      static elle::Status	New(T*&);
      template <typename T>
      static elle::Status	Delete(T*);

      template <typename T>
      static elle::Status	Export(T*);
      template <typename T>
      static elle::Status	Import(T*);

      //
      // constructors & destructors
      //
      Context(const Format&);

      //
      // methods
      //
      elle::Status		Create();

      //
      // interfaces
      //

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Format			format;

      Type			type;

      user::Application*	application;
      elle::Fiber*		fiber;

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

#endif
