//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Maid.hh
//
// created       julien quintard   [sun feb 28 09:00:00 2010]
// updated       julien quintard   [mon mar  1 14:57:05 2010]
//









///////////////////////////// XXX
// renommer en army composee de guards
// faire une unique template pour army qui prend des types, fait le sizeof
// pour connaitre le nombre d'elements et creer un tableau de guard.
//
#ifndef ELLE_MISC_MAID_HH
#define ELLE_MISC_MAID_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/Void.hh>

namespace elle
{
  using namespace core;

  namespace misc
  {

//
// ---------- macro functions -------------------------------------------------
//

///
/// this macro function allocates memory within the current scope through
/// the use of alloca() before creating a Guard.
///
/// only the pointers to variables that must be deallocated automatically
/// should an error occur must be passed to this macro function.
///
#define enter(_objects_...)						\
    Maid::Shell* _maid_ =						\
      Maid::Install((Void*)alloca(Maid::Shell::Size), ##_objects_)

///
/// XXX
///
// XXX utiliser auto au lieu de Void* pour garder le type!
// XXX faire une methode genre Init() qui retourne le type recu en
// pointer et retourne donc NULL pour ce type la. comme ca auto marchera
#define local(_name_)							\
  Void*		_ ## _name_ ## _

///
/// XXX
///
#define track(_name_)							\
  _ ## _name_ ## _ = &_name_

///
/// XXX
///
#define untrack(_name_)							\
  _ ## _name_ ## _ = NULL

///
/// XXX
///
#define waive(_pointer_)						\
  _pointer_ = NULL

///
/// XXX
///
#define keyword(_pointer_)						\
  Maid::Monitor((Void*)alloca(Maid::Object::Size),			\
                _pointer_)

///
/// XXX
///
#define routine(_pointer_, _function_)					\
  Maid::Monitor((Void*)alloca(Maid::Object::Size),			\
                _pointer_, _function_)

///
/// XXX
///
#define structure(_name_, _function_)					\
  Maid::Monitor((Void*)alloca(Maid::Object::Size),			\
                _ ## _name_ ## _, _function_)

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class references pointers that may need deletion.
    ///
    /// XXX expliquer tout le concept general
    ///
    class Maid
    {
    public:
      //
      // constants
      //
      struct		Size
      {
	static const Natural32		Army;
	static const Natural32		Guard;
      };

      //
      // enumerations
      //
      enum Type
	{
	  TypeKeyword,
	  TypeRoutine,
	};

      //
      // classes
      //

      ///
      /// this class represents the base class.
      ///
      /// the new and delete operators are overloaded because instances
      /// of these classes are always allocated on the function/method's
      /// stack via the alloca() builtin. therefore, both the new
      /// and delete operators do not do anything.
      ///
      class Shell
      {
      public:
	//
	// constructors and destructors
	//
	virtual ~Shell()
	{
	}

	//
	// operators
	//
	Void*		operator new(Natural32			size,
				     Void*			memory)
	{
	  // return the same pointer.
	  return (memory);
	}

	Void		operator delete(Void*			memory)
	{
	  // do not release any memory as this will be done automatically.
	}
      };

      ///
      /// this class keeps track of multiple pointers.
      ///
      template <typename... T>
      class Army:
	public Shell
      {
      };

      ///
      /// this class represents an object to be released.
      ///
      class Object
      {
      public:
	//
	// constants
	//
	static const Natural32		Size;
      };

      ///
      /// this class keeps track of an class instance.
      ///
      template <typename T>
      class Keyword:
	public Object
      {
	//
	// constructors & destructors
	//
	Keyword(T&						pointer):
	  pointer(pointer)
	{
	}

	~Keyword()
	{
	  if (this->pointer != NULL)
	    delete this->pointer;
	}

	//
	// attributes
	//
	T		pointer;
      };

      ///
      /// this class represents an object allocated
      /// through a specific function.
      ///
      template <typename T, typename F>
      class Routine:
	public Object
      {
      public:
	//
	// attributes
	//
	T		pointer;
	F		function;

	//
	// constructors & destructors
	//
	Routine(T&						pointer,
		F						function):
	  pointer(pointer),
	  function(function)
	{
	}

	~Routine()
	{
	  if (this->pointer != NULL)
	    (Void)this->function(this->pointer);
	}
      };

      //
      // static methods
      //
      template <typename... T>
      static Shell*		Install(Void*,
					T&...);
      template <typename... T>
      static Shell*		Install(Void*);

      template <typename T>
      static Keyword<T>		Monitor(Void*, T&);
      template <typename T, typename F>
      static Routine<T, F>	Monitor(Void*, T&, F);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/misc/Maid.hxx>

#endif
