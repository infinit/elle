//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Arguments.hxx
//
// created       julien quintard   [wed feb 24 08:36:00 2010]
// updated       julien quintard   [mon jul 18 23:06:18 2011]
//

#ifndef ELLE_RADIX_ARGUMENTS_HXX
#define ELLE_RADIX_ARGUMENTS_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>
#include <elle/radix/Parameters.hh>
#include <elle/radix/Allege.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace standalone;
  using namespace radix;
  using namespace concurrency;

  namespace radix
  {

    ///
    /// these classes specialize the Arguments class depending on the number
    /// of arguments.
    ///
    /// note that the template specializations give meaning to the
    /// parameters i.e P<T...>.
    ///
    /// the Call() method is also provided in constant should the calling
    /// instance have no choice. note however that this would imply every
    /// callback/closure/etc. argument to be constant as well.
    ///

    ///
    /// zero-argument.
    ///
    template <>
    class Arguments< Parameters<> >:
      public Entity
    {
    public:
      //
      // types
      //
      typedef Parameters<>					P;

      //
      // constructors & destructors
      //
      Arguments()
      {
      }

      template <template <typename...> class E,
		typename... U>
      Arguments(const E< Parameters<U...> >&)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<> >&,
		X2< Parameters<> >&)
      {
      }

      //
      // methods
      //

      template <typename C>
      Status		Call(const C&				object)
      {
	return (object.Call());
      }

      template <typename C>
      Status		Call(const C&				object)
	const
      {
	return (object.Call());
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
      {
	return (object.Call(elements...));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
	const
      {
	return (object.Call(elements...));
      }

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32			margin = 0)
	const
      {
	String		alignment(margin, ' ');

	enter();

	std::cout << alignment
		  << "[Arguments " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// single-argument.
    ///
    template <typename T1>
    class Arguments< Parameters<T1> >:
      public Entity
    {
    public:
      //
      // types
      //
      typedef Parameters<T1>					P;

      //
      // attributes
      //
      T1&		element1;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1):
	element1(o1)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Arguments(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1> >&				x1,
		X2< Parameters<> >&):
	element1(x1.element1)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<> >&,
		X2< Parameters<T1> >&				x2):
	element1(x2.element1)
      {
      }

      //
      // methods
      //

      template <typename C>
      Status		Call(const C&				object)
      {
	return (object.Call(this->element1));
      }

      template <typename C>
      Status		Call(const C&				object)
	const
      {
	return (object.Call(this->element1));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
      {
	return (object.Call(this->element1,
			    elements...));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
	const
      {
	return (object.Call(this->element1,
			    elements...));
      }

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32			margin = 0)
	const
      {
	String		alignment(margin, ' ');

	enter();

	std::cout << alignment
		  << "[Arguments " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// two-argument.
    ///
    template <typename T1,
	      typename T2>
    class Arguments< Parameters<T1, T2> >:
      public Entity
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2>				P;

      //
      // attributes
      //
      T1&		element1;
      T2&		element2;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1,
		T2&						o2):
	element1(o1),
	element2(o2)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Arguments(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2> >&			x1,
		X2< Parameters<> >&):
	element1(x1.element1),
	element2(x1.element2)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1> >&				x1,
		X2< Parameters<T2> >&				x2):
	element1(x1.element1),
	element2(x2.element1)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<> >&,
		X2< Parameters<T1, T2> >&			x2):
	element1(x2.element1),
	element2(x2.element2)
      {
      }

      //
      // methods
      //

      template <typename C>
      Status		Call(const C&				object)
      {
	return (object.Call(this->element1,
			    this->element2));
      }

      template <typename C>
      Status		Call(const C&				object)
	const
      {
	return (object.Call(this->element1,
			    this->element2));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
      {
	return (object.Call(this->element1,
			    this->element2,
			    elements...));

      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    elements...));
      }

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32			margin = 0)
	const
      {
	String		alignment(margin, ' ');

	enter();

	std::cout << alignment
		  << "[Arguments " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// three-argument.
    ///
    template <typename T1,
	      typename T2,
	      typename T3>
    class Arguments< Parameters<T1, T2, T3> >:
      public Entity
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2, T3>				P;

      //
      // attributes
      //
      T1&		element1;
      T2&		element2;
      T3&		element3;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1,
		T2&						o2,
		T3&						o3):
	element1(o1),
	element2(o2),
	element3(o3)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Arguments(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2> >&			x1,
		X2< Parameters<T3> >&				x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x2.element1)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1> >&				x1,
		X2< Parameters<T2, T3> >&			x2):
	element1(x1.element1),
	element2(x2.element1),
	element3(x2.element2)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<> >&,
		X2< Parameters<T1, T2, T3> >&			x2):
	element1(x2.element1),
	element2(x2.element2),
	element3(x2.element3)
      {
      }

      //
      // methods
      //

      template <typename C>
      Status		Call(const C&				object)
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3));
      }

      template <typename C>
      Status		Call(const C&				object)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    elements...));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    elements...));
      }

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32			margin = 0)
	const
      {
	String		alignment(margin, ' ');

	enter();

	std::cout << alignment
		  << "[Arguments " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// four-argument.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    class Arguments< Parameters<T1, T2, T3, T4> >:
      public Entity
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2, T3, T4>			P;

      //
      // attributes
      //
      T1&		element1;
      T2&		element2;
      T3&		element3;
      T4&		element4;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1,
		T2&						o2,
		T3&						o3,
		T4&						o4):
	element1(o1),
	element2(o2),
	element3(o3),
	element4(o4)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Arguments(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3),
	element4(ensemble.element4)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4> >&		x1,
		X2< Parameters<> >&):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3> >&			x1,
		X2< Parameters<T4> >&				x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x2.element1)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2> >&			x1,
		X2< Parameters<T3, T4> >&			x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x2.element1),
	element4(x2.element2)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1> >&				x1,
		X2< Parameters<T2, T3, T4> >&			x2):
	element1(x1.element1),
	element2(x2.element1),
	element3(x2.element2),
	element4(x2.element3)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<> >&,
		X2< Parameters<T1, T2, T3, T4> >&		x2):
	element1(x2.element1),
	element2(x2.element2),
	element3(x2.element3),
	element4(x2.element4)
      {
      }

      //
      // methods
      //

      template <typename C>
      Status		Call(const C&				object)
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4));
      }

      template <typename C>
      Status		Call(const C&				object)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    elements...));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    elements...));
      }

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32			margin = 0)
	const
      {
	String		alignment(margin, ' ');

	enter();

	std::cout << alignment
		  << "[Arguments " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// five-argument.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    class Arguments< Parameters<T1, T2, T3, T4, T5> >:
      public Entity
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2, T3, T4, T5>			P;

      //
      // attributes
      //
      T1&		element1;
      T2&		element2;
      T3&		element3;
      T4&		element4;
      T5&		element5;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1,
		T2&						o2,
		T3&						o3,
		T4&						o4,
		T5&						o5):
	element1(o1),
	element2(o2),
	element3(o3),
	element4(o4),
	element5(o5)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Arguments(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3),
	element4(ensemble.element4),
	element5(ensemble.element5)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4, T5> >&		x1,
		X2< Parameters<> >&):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x1.element5)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4> >&		x1,
		X2< Parameters<T5> >&				x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x2.element1)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3> >&			x1,
		X2< Parameters<T4, T5> >&			x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x2.element1),
	element5(x2.element2)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2> >&			x1,
		X2< Parameters<T3, T4, T5> >&			x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x2.element1),
	element4(x2.element2),
	element5(x2.element3)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1> >&				x1,
		X2< Parameters<T2, T3, T4, T5> >&		x2):
	element1(x1.element1),
	element2(x2.element1),
	element3(x2.element2),
	element4(x2.element3),
	element5(x2.element4)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<> >&,
		X2< Parameters<T1, T2, T3, T4, T5> >&		x2):
	element1(x2.element1),
	element2(x2.element2),
	element3(x2.element3),
	element4(x2.element4),
	element5(x2.element5)
      {
      }

      //
      // methods
      //

      template <typename C>
      Status		Call(const C&				object)
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5));
      }

      template <typename C>
      Status		Call(const C&				object)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    elements...));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    elements...));
      }

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32			margin = 0)
	const
      {
	String		alignment(margin, ' ');

	enter();

	std::cout << alignment
		  << "[Arguments " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// six-argument.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    class Arguments< Parameters<T1, T2, T3, T4, T5,
				T6> >:
      public Entity
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2, T3, T4, T5,
			 T6>					P;

      //
      // attributes
      //
      T1&		element1;
      T2&		element2;
      T3&		element3;
      T4&		element4;
      T5&		element5;
      T6&		element6;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1,
		T2&						o2,
		T3&						o3,
		T4&						o4,
		T5&						o5,
		T6&						o6):
	element1(o1),
	element2(o2),
	element3(o3),
	element4(o4),
	element5(o5),
	element6(o6)
      {
      }

      template <template <typename...> class X>
      Arguments(X< Parameters<T1, T2, T3, T4, T5,
			      T6> >&				x):
	element1(x.element1),
	element2(x.element2),
	element3(x.element3),
	element4(x.element4),
	element5(x.element5),
	element6(x.element6)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Arguments(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3),
	element4(ensemble.element4),
	element5(ensemble.element5),
	element6(ensemble.element6)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4, T5> >&		x1,
		X2< Parameters<T6> >&				x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x1.element5),
	element6(x2.element1)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4> >&		x1,
		X2< Parameters<T5, T6> >&			x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x2.element1),
	element6(x2.element2)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3> >&			x1,
		X2< Parameters<T4, T5, T6> >&			x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x2.element1),
	element5(x2.element2),
	element6(x2.element3)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2> >&			x1,
		X2< Parameters<T3, T4, T5, T6> >&		x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x2.element1),
	element4(x2.element2),
	element5(x2.element3),
	element6(x2.element4)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1> >&				x1,
		X2< Parameters<T2, T3, T4, T5, T6> >&		x2):
	element1(x1.element1),
	element2(x2.element1),
	element3(x2.element2),
	element4(x2.element3),
	element5(x2.element4),
	element6(x2.element5)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<> >&,
		X2< Parameters<T1, T2, T3, T4, T5,
			       T6> >&				x2):
	element1(x2.element1),
	element2(x2.element2),
	element3(x2.element3),
	element4(x2.element4),
	element5(x2.element5),
	element6(x2.element6)
      {
      }

      //
      // methods
      //

      template <typename C>
      Status		Call(const C&				object)
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6));
      }

      template <typename C>
      Status		Call(const C&				object)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6,
			    elements...));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6,
			    elements...));
      }

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32			margin = 0)
	const
      {
	String		alignment(margin, ' ');

	enter();

	std::cout << alignment
		  << "[Arguments " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// seven-argument.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    class Arguments< Parameters<T1, T2, T3, T4, T5,
				T6, T7> >:
      public Entity
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2, T3, T4, T5,
			 T6, T7>				P;

      //
      // attributes
      //
      T1&		element1;
      T2&		element2;
      T3&		element3;
      T4&		element4;
      T5&		element5;
      T6&		element6;
      T7&		element7;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1,
		T2&						o2,
		T3&						o3,
		T4&						o4,
		T5&						o5,
		T6&						o6,
		T7&						o7):
	element1(o1),
	element2(o2),
	element3(o3),
	element4(o4),
	element5(o5),
	element6(o6),
	element7(o7)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Arguments(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3),
	element4(ensemble.element4),
	element5(ensemble.element5),
	element6(ensemble.element6),
	element7(ensemble.element7)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4, T5,
			       T6, T7> >&			x1,
		X2< Parameters<> >&):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x1.element5),
	element6(x1.element6),
	element7(x1.element7)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4, T5,
			       T6> >&				x1,
		X2< Parameters<T7> >&				x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x1.element5),
	element6(x1.element6),
	element7(x2.element1)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4, T5> >&		x1,
		X2< Parameters<T6, T7> >&			x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x1.element5),
	element6(x2.element1),
	element7(x2.element2)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4> >&		x1,
		X2< Parameters<T5, T6, T7> >&			x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x2.element1),
	element6(x2.element2),
	element7(x2.element3)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3> >&			x1,
		X2< Parameters<T4, T5, T6, T7> >&		x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x2.element1),
	element5(x2.element2),
	element6(x2.element3),
	element7(x2.element4)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2> >&			x1,
		X2< Parameters<T3, T4, T5, T6, T7> >&		x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x2.element1),
	element4(x2.element2),
	element5(x2.element3),
	element6(x2.element4),
	element7(x2.element5)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1> >&				x1,
		X2< Parameters<T2, T3, T4, T5, T6,
			       T7> >&				x2):
	element1(x1.element1),
	element2(x2.element1),
	element3(x2.element2),
	element4(x2.element3),
	element5(x2.element4),
	element6(x2.element5),
	element7(x2.element6)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<> >&,
		X2< Parameters<T1, T2, T3, T4, T5,
			       T6, T7> >&			x2):
	element1(x2.element1),
	element2(x2.element2),
	element3(x2.element3),
	element4(x2.element4),
	element5(x2.element5),
	element6(x2.element6),
	element7(x2.element7)
      {
      }

      //
      // methods
      //

      template <typename C>
      Status		Call(const C&				object)
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6,
			    this->element7));
      }

      template <typename C>
      Status		Call(const C&				object)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6,
			    this->element7));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6,
			    this->element7,
			    elements...));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6,
			    this->element7,
			    elements...));
      }

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32			margin = 0)
	const
      {
	String		alignment(margin, ' ');

	enter();

	std::cout << alignment
		  << "[Arguments " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// eight-arguments.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    class Arguments< Parameters<T1, T2, T3, T4, T5,
				T6, T7, T8> >:
      public Entity
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2, T3, T4, T5,
			 T6, T7, T8>				P;

      //
      // attributes
      //
      T1&		element1;
      T2&		element2;
      T3&		element3;
      T4&		element4;
      T5&		element5;
      T6&		element6;
      T7&		element7;
      T8&		element8;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1,
		T2&						o2,
		T3&						o3,
		T4&						o4,
		T5&						o5,
		T6&						o6,
		T7&						o7,
		T8&						o8):
	element1(o1),
	element2(o2),
	element3(o3),
	element4(o4),
	element5(o5),
	element6(o6),
	element7(o7),
	element8(o8)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Arguments(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3),
	element4(ensemble.element4),
	element5(ensemble.element5),
	element6(ensemble.element6),
	element7(ensemble.element7),
	element8(ensemble.element8)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4, T5,
			       T6, T7, T8> >&			x1,
		X2< Parameters<> >&):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x1.element5),
	element6(x1.element6),
	element7(x1.element7),
	element8(x1.element8)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4, T5,
			       T6, T7> >&			x1,
		X2< Parameters<T8> >&				x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x1.element5),
	element6(x1.element6),
	element7(x1.element7),
	element8(x2.element1)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4, T5,
			       T6> >&				x1,
		X2< Parameters<T7, T8> >&			x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x1.element5),
	element6(x1.element6),
	element7(x2.element1),
	element8(x2.element2)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4, T5> >&		x1,
		X2< Parameters<T6, T7, T8> >&			x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x1.element5),
	element6(x2.element1),
	element7(x2.element2),
	element8(x2.element3)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4> >&		x1,
		X2< Parameters<T5, T6, T7, T8> >&		x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x2.element1),
	element6(x2.element2),
	element7(x2.element3),
	element8(x2.element4)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3> >&			x1,
		X2< Parameters<T4, T5, T6, T7, T8> >&		x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x2.element1),
	element5(x2.element2),
	element6(x2.element3),
	element7(x2.element4),
	element8(x2.element5)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2> >&			x1,
		X2< Parameters<T3, T4, T5, T6, T7,
			       T8> >&				x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x2.element1),
	element4(x2.element2),
	element5(x2.element3),
	element6(x2.element4),
	element7(x2.element5),
	element8(x2.element6)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1> >&				x1,
		X2< Parameters<T2, T3, T4, T5, T6,
			       T7, T8> >&			x2):
	element1(x1.element1),
	element2(x2.element1),
	element3(x2.element2),
	element4(x2.element3),
	element5(x2.element4),
	element6(x2.element5),
	element7(x2.element6),
	element8(x2.element7)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<> >&,
		X2< Parameters<T1, T2, T3, T4, T5,
			       T6, T7, T8> >&			x2):
	element1(x2.element1),
	element2(x2.element2),
	element3(x2.element3),
	element4(x2.element4),
	element5(x2.element5),
	element6(x2.element6),
	element7(x2.element7),
	element8(x2.element8)
      {
      }

      //
      // methods
      //

      template <typename C>
      Status		Call(const C&				object)
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6,
			    this->element7,
			    this->element8));
      }

      template <typename C>
      Status		Call(const C&				object)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6,
			    this->element7,
			    this->element8));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6,
			    this->element7,
			    this->element8,
			    elements...));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6,
			    this->element7,
			    this->element8,
			    elements...));
      }

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32			margin = 0)
	const
      {
	String		alignment(margin, ' ');

	enter();

	std::cout << alignment
		  << "[Arguments " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// nine-argument.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename T9>
    class Arguments< Parameters<T1, T2, T3, T4, T5,
				T6, T7, T8, T9> >:
      public Entity
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2, T3, T4, T5,
			 T6, T7, T8, T9>			P;

      //
      // attributes
      //
      T1&		element1;
      T2&		element2;
      T3&		element3;
      T4&		element4;
      T5&		element5;
      T6&		element6;
      T7&		element7;
      T8&		element8;
      T9&		element9;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1,
		T2&						o2,
		T3&						o3,
		T4&						o4,
		T5&						o5,
		T6&						o6,
		T7&						o7,
		T8&						o8,
		T9&						o9):
	element1(o1),
	element2(o2),
	element3(o3),
	element4(o4),
	element5(o5),
	element6(o6),
	element7(o7),
	element8(o8),
	element9(o9)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Arguments(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3),
	element4(ensemble.element4),
	element5(ensemble.element5),
	element6(ensemble.element6),
	element7(ensemble.element7),
	element8(ensemble.element8),
	element9(ensemble.element9)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4, T5,
			       T6, T7, T8, T9> >&		x1,
		X2< Parameters<> >&):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x1.element5),
	element6(x1.element6),
	element7(x1.element7),
	element8(x1.element8),
	element9(x1.element9)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4, T5,
			       T6, T7, T8> >&			x1,
		X2< Parameters<T9> >&				x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x1.element5),
	element6(x1.element6),
	element7(x1.element7),
	element8(x1.element8),
	element9(x2.element1)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4, T5,
			       T6, T7> >&			x1,
		X2< Parameters<T8, T9> >&			x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x1.element5),
	element6(x1.element6),
	element7(x1.element7),
	element8(x2.element1),
	element9(x2.element2)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4, T5,
			       T6> >&				x1,
		X2< Parameters<T7, T8, T9> >&			x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x1.element5),
	element6(x1.element6),
	element7(x2.element1),
	element8(x2.element2),
	element9(x2.element3)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4, T5> >&		x1,
		X2< Parameters<T6, T7, T8, T9> >&		x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x1.element5),
	element6(x2.element1),
	element7(x2.element2),
	element8(x2.element3),
	element9(x2.element4)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3, T4> >&		x1,
		X2< Parameters<T5, T6, T7, T8, T9> >&		x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x1.element4),
	element5(x2.element1),
	element6(x2.element2),
	element7(x2.element3),
	element8(x2.element4),
	element9(x2.element5)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2, T3> >&			x1,
		X2< Parameters<T4, T5, T6, T7, T8,
			       T9> >&				x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x1.element3),
	element4(x2.element1),
	element5(x2.element2),
	element6(x2.element3),
	element7(x2.element4),
	element8(x2.element5),
	element9(x2.element6)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1, T2> >&			x1,
		X2< Parameters<T3, T4, T5, T6, T7,
			       T8, T9> >&			x2):
	element1(x1.element1),
	element2(x1.element2),
	element3(x2.element1),
	element4(x2.element2),
	element5(x2.element3),
	element6(x2.element4),
	element7(x2.element5),
	element8(x2.element6),
	element9(x2.element7)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<T1> >&				x1,
		X2< Parameters<T2, T3, T4, T5, T6,
			       T7, T8, T9> >&			x2):
	element1(x1.element1),
	element2(x2.element1),
	element3(x2.element2),
	element4(x2.element3),
	element5(x2.element4),
	element6(x2.element5),
	element7(x2.element6),
	element8(x2.element7),
	element9(x2.element8)
      {
      }

      template <template <typename...> class X1,
		template <typename...> class X2>
      Arguments(X1< Parameters<> >&,
		X2< Parameters<T1, T2, T3, T4, T5,
			       T6, T7, T8, T9> >&		x2):
	element1(x2.element1),
	element2(x2.element2),
	element3(x2.element3),
	element4(x2.element4),
	element5(x2.element5),
	element6(x2.element6),
	element7(x2.element7),
	element8(x2.element8),
	element9(x2.element9)
      {
      }

      //
      // methods
      //

      template <typename C>
      Status		Call(const C&				object)
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6,
			    this->element7,
			    this->element8,
			    this->element9));
      }

      template <typename C>
      Status		Call(const C&				object)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6,
			    this->element7,
			    this->element8,
			    this->element9));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6,
			    this->element7,
			    this->element8,
			    this->element9,
			    elements...));
      }

      template <typename C,
		typename... U>
      Status		Call(const C&				object,
			     U&...				elements)
	const
      {
	return (object.Call(this->element1,
			    this->element2,
			    this->element3,
			    this->element4,
			    this->element5,
			    this->element6,
			    this->element7,
			    this->element8,
			    this->element9,
			    elements...));
      }

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32			margin = 0)
	const
      {
	String		alignment(margin, ' ');

	enter();

	std::cout << alignment
		  << "[Arguments " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

  }
}

#endif
