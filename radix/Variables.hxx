//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed feb 24 08:36:00 2010]
//

#ifndef ELLE_RADIX_VARIABLES_HXX
#define ELLE_RADIX_VARIABLES_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>
#include <elle/radix/Parameters.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace standalone;
  using namespace radix;
  using namespace concurrency;

  namespace radix
  {

    ///
    /// these classes specialize the Variables class depending on the number
    /// of variables.
    ///
    /// note that the template specializations give meaning to the
    /// parameters i.e Parameters<T...>.
    ///

    ///
    /// zero-variable.
    ///
    template <>
    class Variables< Parameters<> >:
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

      Variables()
      {
      }

      template <template <typename...> class E,
		typename... U>
      Variables(E< Parameters<U...> >&				ensemble)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class E1,
		template <typename...> class E2>
      Variables(E1< Parameters<> >&,
		E2< Parameters<> >&)
      {
      }

      //
      // methods 
      //

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

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
      {
	object.Call(elements...);
      }

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
	const
      {
	object.Call(elements...);
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
		  << "[Variables " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// single-variable.
    ///
    template <typename T1>
    class Variables< Parameters<T1> >:
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
      T1		element1;

      //
      // constructors & destructors
      //

      Variables()
      {
      }

      Variables(T1&						element1):
	element1(element1)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Variables(E< Parameters<U...> >&				ensemble):
	element1(ensemble.element1)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1>
      Variables(E1< Parameters<U1> >&				ensemble1,
		E2< Parameters<> >&):
	element1(ensemble1.element1)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename V1>
      Variables(E1< Parameters<> >&,
		E2< Parameters<V1> >&				ensemble2):
	element1(ensemble2.element1)
      {
      }

      //
      // methods
      //

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

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
      {
	object.Call(this->element1,
		    elements...);
      }

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
	const
      {
	object.Call(this->element1,
		    elements...);
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
		  << "[Variables " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// two-variable.
    ///
    template <typename T1,
	      typename T2>
    class Variables< Parameters<T1, T2> >:
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
      T1		element1;
      T2		element2;

      //
      // constructors & destructors
      //

      Variables()
      {
      }

      Variables(T1&						element1,
		T2&						element2):
	element1(element1),
	element2(element2)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Variables(E< Parameters<U...> >&				ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2>
      Variables(E1< Parameters<U1, U2> >&			ensemble1,
		E2< Parameters<> >&):
	element1(ensemble1.element1),
	element2(ensemble1.element2)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename V2>
      Variables(E1< Parameters<U1> >&				ensemble1,
		E2< Parameters<V2> >&				ensemble2):
	element1(ensemble1.element1),
	element2(ensemble2.element1)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename V1,
		typename V2>
      Variables(E1< Parameters<> >&,
		E2< Parameters<V1, V2> >&			ensemble2):
	element1(ensemble2.element1),
	element2(ensemble2.element2)
      {
      }

      //
      // methods
      //

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

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
      {
	object.Call(this->element1,
		    this->element2,
		    elements...);

      }

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
	const
      {
	object.Call(this->element1,
		    this->element2,
		    elements...);
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
		  << "[Variables " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// three-variable.
    ///
    template <typename T1,
	      typename T2,
	      typename T3>
    class Variables< Parameters<T1, T2, T3> >:
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
      T1		element1;
      T2		element2;
      T3		element3;

      //
      // constructors & destructors
      //

      Variables()
      {
      }

      Variables(T1&						element1,
		T2&						element2,
		T3&						element3):
	element1(element1),
	element2(element2),
	element3(element3)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Variables(E< Parameters<U...> >&				ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3>
      Variables(E1< Parameters<U1, U2, U3> >&			ensemble1,
		E2< Parameters<> >&):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename V3>
      Variables(E1< Parameters<U1, U2> >&			ensemble1,
		E2< Parameters<V3> >&				ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble2.element1)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename V2,
		typename V3>
      Variables(E1< Parameters<U1> >&				ensemble1,
		E2< Parameters<V2, V3> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble2.element1),
	element3(ensemble2.element2)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename V1,
		typename V2,
		typename V3>
      Variables(E1< Parameters<> >&,
		E2< Parameters<V1, V2, V3> >&			ensemble2):
	element1(ensemble2.element1),
	element2(ensemble2.element2),
	element3(ensemble2.element3)
      {
      }

      //
      // methods
      //

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

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
      {
	object.Call(this->element1,
		    this->element2,
		    this->element3,
		    elements...);
      }

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
	const
      {
	object.Call(this->element1,
		    this->element2,
		    this->element3,
		    elements...);
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
		  << "[Variables " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// four-variable.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    class Variables< Parameters<T1, T2, T3, T4> >:
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
      T1		element1;
      T2		element2;
      T3		element3;
      T4		element4;

      //
      // constructors & destructors
      //

      Variables()
      {
      }

      Variables(T1&						element1,
		T2&						element2,
		T3&						element3,
		T4&						element4):
	element1(element1),
	element2(element2),
	element3(element3),
	element4(element4)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Variables(E< Parameters<U...> >&				ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3),
	element4(ensemble.element4)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4>
      Variables(E1< Parameters<U1, U2, U3, U4> >&		ensemble1,
		E2< Parameters<> >&):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename V4>
      Variables(E1< Parameters<U1, U2, U3> >&			ensemble1,
		E2< Parameters<V4> >&				ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble2.element1)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename V3,
		typename V4>
      Variables(E1< Parameters<U1, U2> >&			ensemble1,
		E2< Parameters<V3, V4> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble2.element1),
	element4(ensemble2.element2)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename V2,
		typename V3,
		typename V4>
      Variables(E1< Parameters<U1> >&				ensemble1,
		E2< Parameters<V2, V3, V4> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble2.element1),
	element3(ensemble2.element2),
	element4(ensemble2.element3)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename V1,
		typename V2,
		typename V3,
		typename V4>
      Variables(E1< Parameters<> >&,
		E2< Parameters<V1, V2, V3, V4> >&		ensemble2):
	element1(ensemble2.element1),
	element2(ensemble2.element2),
	element3(ensemble2.element3),
	element4(ensemble2.element4)
      {
      }

      //
      // methods
      //

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

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
      {
	object.Call(this->element1,
		    this->element2,
		    this->element3,
		    this->element4,
		    elements...);
      }

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
	const
      {
	object.Call(this->element1,
		    this->element2,
		    this->element3,
		    this->element4,
		    elements...);
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
		  << "[Variables " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// five-variable.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    class Variables< Parameters<T1, T2, T3, T4, T5> >:
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
      T1		element1;
      T2		element2;
      T3		element3;
      T4		element4;
      T5		element5;

      //
      // constructors & destructors
      //

      Variables()
      {
      }

      Variables(T1&						element1,
		T2&						element2,
		T3&						element3,
		T4&						element4,
		T5&						element5):
	element1(element1),
	element2(element2),
	element3(element3),
	element4(element4),
	element5(element5)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Variables(E< Parameters<U...> >&				ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3),
	element4(ensemble.element4),
	element5(ensemble.element5)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5>
      Variables(E1< Parameters<U1, U2, U3, U4, U5> >&		ensemble1,
		E2< Parameters<> >&):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename V5>
      Variables(E1< Parameters<U1, U2, U3, U4> >&		ensemble1,
		E2< Parameters<V5> >&				ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble2.element1)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename V4,
		typename V5>
      Variables(E1< Parameters<U1, U2, U3> >&			ensemble1,
		E2< Parameters<V4, V5> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble2.element1),
	element5(ensemble2.element2)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename V3,
		typename V4,
		typename V5>
      Variables(E1< Parameters<U1, U2> >&			ensemble1,
		E2< Parameters<V3, V4, V5> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble2.element1),
	element4(ensemble2.element2),
	element5(ensemble2.element3)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename V2,
		typename V3,
		typename V4,
		typename V5>
      Variables(E1< Parameters<U1> >&				ensemble1,
		E2< Parameters<V2, V3, V4, V5> >&		ensemble2):
	element1(ensemble1.element1),
	element2(ensemble2.element1),
	element3(ensemble2.element2),
	element4(ensemble2.element3),
	element5(ensemble2.element4)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename V1,
		typename V2,
		typename V3,
		typename V4,
		typename V5>
      Variables(E1< Parameters<> >&,
		E2< Parameters<V1, V2, V3, V4, V5> >&		ensemble2):
	element1(ensemble2.element1),
	element2(ensemble2.element2),
	element3(ensemble2.element3),
	element4(ensemble2.element4),
	element5(ensemble2.element5)
      {
      }

      //
      // methods
      //

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

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
      {
	object.Call(this->element1,
		    this->element2,
		    this->element3,
		    this->element4,
		    this->element5,
		    elements...);
      }

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
	const
      {
	object.Call(this->element1,
		    this->element2,
		    this->element3,
		    this->element4,
		    this->element5,
		    elements...);
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
		  << "[Variables " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// six-variable.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    class Variables< Parameters<T1, T2, T3, T4, T5,
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
      T1		element1;
      T2		element2;
      T3		element3;
      T4		element4;
      T5		element5;
      T6		element6;

      //
      // constructors & destructors
      //

      Variables()
      {
      }

      Variables(T1&						element1,
		T2&						element2,
		T3&						element3,
		T4&						element4,
		T5&						element5,
		T6&						element6):
	element1(element1),
	element2(element2),
	element3(element3),
	element4(element4),
	element5(element5),
	element6(element6)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Variables(E< Parameters<U...> >&				ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3),
	element4(ensemble.element4),
	element5(ensemble.element5),
	element6(ensemble.element6)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5,
		typename U6>
      Variables(E1< Parameters<U1, U2, U3, U4, U5,
			       U6> >&				ensemble1,
		E2< Parameters<> >&):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5),
	element6(ensemble1.element6)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5,
		typename V6>
      Variables(E1< Parameters<U1, U2, U3, U4, U5> >&		ensemble1,
		E2< Parameters<V6> >&				ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5),
	element6(ensemble2.element1)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename V5,
		typename V6>
      Variables(E1< Parameters<U1, U2, U3, U4> >&		ensemble1,
		E2< Parameters<V5, V6> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble2.element1),
	element6(ensemble2.element2)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename V4,
		typename V5,
		typename V6>
      Variables(E1< Parameters<U1, U2, U3> >&			ensemble1,
		E2< Parameters<V4, V5, V6> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble2.element1),
	element5(ensemble2.element2),
	element6(ensemble2.element3)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename V3,
		typename V4,
		typename V5,
		typename V6>
      Variables(E1< Parameters<U1, U2> >&			ensemble1,
		E2< Parameters<V3, V4, V5, V6> >&		ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble2.element1),
	element4(ensemble2.element2),
	element5(ensemble2.element3),
	element6(ensemble2.element4)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename V2,
		typename V3,
		typename V4,
		typename V5,
		typename V6>
      Variables(E1< Parameters<U1> >&				ensemble1,
		E2< Parameters<V2, V3, V4, V5, V6> >&		ensemble2):
	element1(ensemble1.element1),
	element2(ensemble2.element1),
	element3(ensemble2.element2),
	element4(ensemble2.element3),
	element5(ensemble2.element4),
	element6(ensemble2.element5)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename V1,
		typename V2,
		typename V3,
		typename V4,
		typename V5,
		typename V6>
      Variables(E1< Parameters<> >&,
		E2< Parameters<V1, V2, V3, V4, V5,
			       V6> >&				ensemble2):
	element1(ensemble2.element1),
	element2(ensemble2.element2),
	element3(ensemble2.element3),
	element4(ensemble2.element4),
	element5(ensemble2.element5),
	element6(ensemble2.element6)
      {
      }

      //
      // methods
      //

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

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
      {
	object.Call(this->element1,
		    this->element2,
		    this->element3,
		    this->element4,
		    this->element5,
		    this->element6,
		    elements...);
      }

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
	const
      {
	object.Call(this->element1,
		    this->element2,
		    this->element3,
		    this->element4,
		    this->element5,
		    this->element6,
		    elements...);
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
		  << "[Variables " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// seven-variable.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    class Variables< Parameters<T1, T2, T3, T4, T5,
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
      T1		element1;
      T2		element2;
      T3		element3;
      T4		element4;
      T5		element5;
      T6		element6;
      T7		element7;

      //
      // constructors & destructors
      //

      Variables()
      {
      }

      Variables(T1&						element1,
		T2&						element2,
		T3&						element3,
		T4&						element4,
		T5&						element5,
		T6&						element6,
		T7&						element7):
	element1(element1),
	element2(element2),
	element3(element3),
	element4(element4),
	element5(element5),
	element6(element6),
	element7(element7)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Variables(E< Parameters<U...> >&				ensemble):
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

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5,
		typename U6,
		typename U7>
      Variables(E1< Parameters<U1, U2, U3, U4, U5,
			       U6, U7> >&			ensemble1,
		E2< Parameters<> >&):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5),
	element6(ensemble1.element6),
	element7(ensemble1.element7)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5,
		typename U6,
		typename V7>
      Variables(E1< Parameters<U1, U2, U3, U4, U5,
			       U6> >&				ensemble1,
		E2< Parameters<V7> >&				ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5),
	element6(ensemble1.element6),
	element7(ensemble2.element1)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5,
		typename V6,
		typename V7>
      Variables(E1< Parameters<U1, U2, U3, U4, U5> >&		ensemble1,
		E2< Parameters<V6, V7> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5),
	element6(ensemble2.element1),
	element7(ensemble2.element2)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename V5,
		typename V6,
		typename V7>
      Variables(E1< Parameters<U1, U2, U3, U4> >&		ensemble1,
		E2< Parameters<V5, V6, V7> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble2.element1),
	element6(ensemble2.element2),
	element7(ensemble2.element3)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename V4,
		typename V5,
		typename V6,
		typename V7>
      Variables(E1< Parameters<U1, U2, U3> >&			ensemble1,
		E2< Parameters<V4, V5, V6, V7> >&		ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble2.element1),
	element5(ensemble2.element2),
	element6(ensemble2.element3),
	element7(ensemble2.element4)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename V3,
		typename V4,
		typename V5,
		typename V6,
		typename V7>
      Variables(E1< Parameters<U1, U2> >&			ensemble1,
		E2< Parameters<V3, V4, V5, V6, V7> >&		ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble2.element1),
	element4(ensemble2.element2),
	element5(ensemble2.element3),
	element6(ensemble2.element4),
	element7(ensemble2.element5)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename V2,
		typename V3,
		typename V4,
		typename V5,
		typename V6,
		typename V7>
      Variables(E1< Parameters<U1> >&				ensemble1,
		E2< Parameters<V2, V3, V4, V5, V6,
			       V7> >&				ensemble2):
	element1(ensemble1.element1),
	element2(ensemble2.element1),
	element3(ensemble2.element2),
	element4(ensemble2.element3),
	element5(ensemble2.element4),
	element6(ensemble2.element5),
	element7(ensemble2.element6)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename V1,
		typename V2,
		typename V3,
		typename V4,
		typename V5,
		typename V6,
		typename V7>
      Variables(E1< Parameters<> >&,
		E2< Parameters<V1, V2, V3, V4, V5,
			       V6, V7> >&			ensemble2):
	element1(ensemble2.element1),
	element2(ensemble2.element2),
	element3(ensemble2.element3),
	element4(ensemble2.element4),
	element5(ensemble2.element5),
	element6(ensemble2.element6),
	element7(ensemble2.element7)
      {
      }

      //
      // methods
      //

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

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
      {
	object.Call(this->element1,
		    this->element2,
		    this->element3,
		    this->element4,
		    this->element5,
		    this->element6,
		    this->element7,
		    elements...);
      }

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
	const
      {
	object.Call(this->element1,
		    this->element2,
		    this->element3,
		    this->element4,
		    this->element5,
		    this->element6,
		    this->element7,
		    elements...);
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
		  << "[Variables " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// eight-variables.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    class Variables< Parameters<T1, T2, T3, T4, T5,
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
      T1		element1;
      T2		element2;
      T3		element3;
      T4		element4;
      T5		element5;
      T6		element6;
      T7		element7;
      T8		element8;

      //
      // constructors & destructors
      //

      Variables()
      {
      }

      Variables(T1&						element1,
		T2&						element2,
		T3&						element3,
		T4&						element4,
		T5&						element5,
		T6&						element6,
		T7&						element7,
		T8&						element8):
	element1(element1),
	element2(element2),
	element3(element3),
	element4(element4),
	element5(element5),
	element6(element6),
	element7(element7),
	element8(element8)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Variables(E< Parameters<U...> >&				ensemble):
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

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5,
		typename U6,
		typename U7,
		typename U8>
      Variables(E1< Parameters<U1, U2, U3, U4, U5,
			       U6, U7, U8> >&			ensemble1,
		E2< Parameters<> >&):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5),
	element6(ensemble1.element6),
	element7(ensemble1.element7),
	element8(ensemble1.element8)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5,
		typename U6,
		typename U7,
		typename V8>
      Variables(E1< Parameters<U1, U2, U3, U4, U5,
			       U6, U7> >&			ensemble1,
		E2< Parameters<V8> >&				ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5),
	element6(ensemble1.element6),
	element7(ensemble1.element7),
	element8(ensemble2.element1)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5,
		typename U6,
		typename V7,
		typename V8>
      Variables(E1< Parameters<U1, U2, U3, U4, U5,
			       U6> >&				ensemble1,
		E2< Parameters<V7, V8> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5),
	element6(ensemble1.element6),
	element7(ensemble2.element1),
	element8(ensemble2.element2)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5,
		typename V6,
		typename V7,
		typename V8>
      Variables(E1< Parameters<U1, U2, U3, U4, U5> >&		ensemble1,
		E2< Parameters<V6, V7, V8> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5),
	element6(ensemble2.element1),
	element7(ensemble2.element2),
	element8(ensemble2.element3)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename V5,
		typename V6,
		typename V7,
		typename V8>
      Variables(E1< Parameters<U1, U2, U3, U4> >&		ensemble1,
		E2< Parameters<V5, V6, V7, V8> >&		ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble2.element1),
	element6(ensemble2.element2),
	element7(ensemble2.element3),
	element8(ensemble2.element4)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename V4,
		typename V5,
		typename V6,
		typename V7,
		typename V8>
      Variables(E1< Parameters<U1, U2, U3> >&			ensemble1,
		E2< Parameters<V4, V5, V6, V7, V8> >&		ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble2.element1),
	element5(ensemble2.element2),
	element6(ensemble2.element3),
	element7(ensemble2.element4),
	element8(ensemble2.element5)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename V3,
		typename V4,
		typename V5,
		typename V6,
		typename V7,
		typename V8>
      Variables(E1< Parameters<U1, U2> >&			ensemble1,
		E2< Parameters<V3, V4, V5, V6, V7,
			       V8> >&				ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble2.element1),
	element4(ensemble2.element2),
	element5(ensemble2.element3),
	element6(ensemble2.element4),
	element7(ensemble2.element5),
	element8(ensemble2.element6)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename V2,
		typename V3,
		typename V4,
		typename V5,
		typename V6,
		typename V7,
		typename V8>
      Variables(E1< Parameters<U1> >&				ensemble1,
		E2< Parameters<V2, V3, V4, V5, V6,
			       V7, V8> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble2.element1),
	element3(ensemble2.element2),
	element4(ensemble2.element3),
	element5(ensemble2.element4),
	element6(ensemble2.element5),
	element7(ensemble2.element6),
	element8(ensemble2.element7)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename V1,
		typename V2,
		typename V3,
		typename V4,
		typename V5,
		typename V6,
		typename V7,
		typename V8>
      Variables(E1< Parameters<> >&,
		E2< Parameters<V1, V2, V3, V4, V5,
			       V6, V7, V8> >&			ensemble2):
	element1(ensemble2.element1),
	element2(ensemble2.element2),
	element3(ensemble2.element3),
	element4(ensemble2.element4),
	element5(ensemble2.element5),
	element6(ensemble2.element6),
	element7(ensemble2.element7),
	element8(ensemble2.element8)
      {
      }

      //
      // methods
      //

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

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
      {
	object.Call(this->element1,
		    this->element2,
		    this->element3,
		    this->element4,
		    this->element5,
		    this->element6,
		    this->element7,
		    this->element8,
		    elements...);
      }

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
	const
      {
	object.Call(this->element1,
		    this->element2,
		    this->element3,
		    this->element4,
		    this->element5,
		    this->element6,
		    this->element7,
		    this->element8,
		    elements...);
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
		  << "[Variables " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

    ///
    /// nine-variable.
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
    class Variables< Parameters<T1, T2, T3, T4, T5,
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
      T1		element1;
      T2		element2;
      T3		element3;
      T4		element4;
      T5		element5;
      T6		element6;
      T7		element7;
      T8		element8;
      T9		element9;

      //
      // constructors & destructors
      //

      Variables()
      {
      }

      Variables(T1&						element1,
		T2&						element2,
		T3&						element3,
		T4&						element4,
		T5&						element5,
		T6&						element6,
		T7&						element7,
		T8&						element8,
		T9&						element9):
	element1(element1),
	element2(element2),
	element3(element3),
	element4(element4),
	element5(element5),
	element6(element6),
	element7(element7),
	element8(element8),
	element9(element9)
      {
      }

      template <template <typename...> class E,
		typename... U>
      Variables(E< Parameters<U...> >&				ensemble):
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

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5,
		typename U6,
		typename U7,
		typename U8,
		typename U9>
      Variables(E1< Parameters<U1, U2, U3, U4, U5,
			       U6, U7, U8, U9> >&		ensemble1,
		E2< Parameters<> >&):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5),
	element6(ensemble1.element6),
	element7(ensemble1.element7),
	element8(ensemble1.element8),
	element9(ensemble1.element9)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5,
		typename U6,
		typename U7,
		typename U8,
		typename V9>
      Variables(E1< Parameters<U1, U2, U3, U4, U5,
			       U6, U7, U8> >&			ensemble1,
		E2< Parameters<V9> >&				ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5),
	element6(ensemble1.element6),
	element7(ensemble1.element7),
	element8(ensemble1.element8),
	element9(ensemble2.element1)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5,
		typename U6,
		typename U7,
		typename V8,
		typename V9>
      Variables(E1< Parameters<U1, U2, U3, U4, U5,
			       U6, U7> >&			ensemble1,
		E2< Parameters<V8, V9> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5),
	element6(ensemble1.element6),
	element7(ensemble1.element7),
	element8(ensemble2.element1),
	element9(ensemble2.element2)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5,
		typename U6,
		typename V7,
		typename V8,
		typename V9>
      Variables(E1< Parameters<U1, U2, U3, U4, U5,
			       U6> >&				ensemble1,
		E2< Parameters<V7, V8, V9> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5),
	element6(ensemble1.element6),
	element7(ensemble2.element1),
	element8(ensemble2.element2),
	element9(ensemble2.element3)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename U5,
		typename V6,
		typename V7,
		typename V8,
		typename V9>
      Variables(E1< Parameters<U1, U2, U3, U4, U5> >&		ensemble1,
		E2< Parameters<V6, V7, V8, V9> >&		ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble1.element5),
	element6(ensemble2.element1),
	element7(ensemble2.element2),
	element8(ensemble2.element3),
	element9(ensemble2.element4)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename U4,
		typename V5,
		typename V6,
		typename V7,
		typename V8,
		typename V9>
      Variables(E1< Parameters<U1, U2, U3, U4> >&		ensemble1,
		E2< Parameters<V5, V6, V7, V8, V9> >&		ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble1.element4),
	element5(ensemble2.element1),
	element6(ensemble2.element2),
	element7(ensemble2.element3),
	element8(ensemble2.element4),
	element9(ensemble2.element5)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename U3,
		typename V4,
		typename V5,
		typename V6,
		typename V7,
		typename V8,
		typename V9>
      Variables(E1< Parameters<U1, U2, U3> >&			ensemble1,
		E2< Parameters<V4, V5, V6, V7, V8,
			       V9> >&				ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble1.element3),
	element4(ensemble2.element1),
	element5(ensemble2.element2),
	element6(ensemble2.element3),
	element7(ensemble2.element4),
	element8(ensemble2.element5),
	element9(ensemble2.element6)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename U2,
		typename V3,
		typename V4,
		typename V5,
		typename V6,
		typename V7,
		typename V8,
		typename V9>
      Variables(E1< Parameters<U1, U2> >&			ensemble1,
		E2< Parameters<V3, V4, V5, V6, V7,
			       V8, V9> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble1.element2),
	element3(ensemble2.element1),
	element4(ensemble2.element2),
	element5(ensemble2.element3),
	element6(ensemble2.element4),
	element7(ensemble2.element5),
	element8(ensemble2.element6),
	element9(ensemble2.element7)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename U1,
		typename V2,
		typename V3,
		typename V4,
		typename V5,
		typename V6,
		typename V7,
		typename V8,
		typename V9>
      Variables(E1< Parameters<U1> >&				ensemble1,
		E2< Parameters<V2, V3, V4, V5, V6,
			       V7, V8, V9> >&			ensemble2):
	element1(ensemble1.element1),
	element2(ensemble2.element1),
	element3(ensemble2.element2),
	element4(ensemble2.element3),
	element5(ensemble2.element4),
	element6(ensemble2.element5),
	element7(ensemble2.element6),
	element8(ensemble2.element7),
	element9(ensemble2.element8)
      {
      }

      template <template <typename...> class E1,
		template <typename...> class E2,
		typename V1,
		typename V2,
		typename V3,
		typename V4,
		typename V5,
		typename V6,
		typename V7,
		typename V8,
		typename V9>
      Variables(E1< Parameters<> >&,
		E2< Parameters<V1, V2, V3, V4, V5,
			       V6, V7, V8, V9> >&		ensemble2):
	element1(ensemble2.element1),
	element2(ensemble2.element2),
	element3(ensemble2.element3),
	element4(ensemble2.element4),
	element5(ensemble2.element5),
	element6(ensemble2.element6),
	element7(ensemble2.element7),
	element8(ensemble2.element8),
	element9(ensemble2.element9)
      {
      }

      //
      // methods
      //

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

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
      {
	object.Call(this->element1,
		    this->element2,
		    this->element3,
		    this->element4,
		    this->element5,
		    this->element6,
		    this->element7,
		    this->element8,
		    this->element9,
		    elements...);
      }

      template <typename C,
		typename... U>
      Void		Trigger(const C&			object,
				U&...				elements)
	const
      {
	object.Call(this->element1,
		    this->element2,
		    this->element3,
		    this->element4,
		    this->element5,
		    this->element6,
		    this->element7,
		    this->element8,
		    this->element9,
		    elements...);
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
		  << "[Variables " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

//
// ---------- methods ---------------------------------------------------------
//
 
    ///
    /// this method merges an ensemble with another hence returning
    /// an Variables class referencing the inner elements of the two
    /// ensembles.
    ///
    template <template <typename...> class E1,
	      template <typename...> class E2,
	      typename... U,
	      typename... V>
    Variables< Parameters<U..., V...> >
    Variables<>::Union(E1< Parameters<U...> >&			ensemble1,
		       E2< Parameters<V...> >&			ensemble2)
    {
      return (Variables< Parameters<U..., V...> >(ensemble1, ensemble2));
    }

  }
}

#endif
