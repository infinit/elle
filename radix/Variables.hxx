//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Variables.hxx
//
// created       julien quintard   [wed feb 24 08:36:00 2010]
// updated       julien quintard   [mon jul 18 15:58:28 2011]
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
    /// parameters i.e P<T...>.
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
      Variables(const E< Parameters<U...> >&			ensemble)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
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

      template <template <typename...> class E,
		typename... U>
      Variables(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
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

      template <template <typename...> class E,
		typename... U>
      Variables(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
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

      template <template <typename...> class E,
		typename... U>
      Variables(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
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

      template <template <typename...> class E,
		typename... U>
      Variables(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3),
	element4(ensemble.element4)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
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

      template <template <typename...> class E,
		typename... U>
      Variables(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3),
	element4(ensemble.element4),
	element5(ensemble.element5)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
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

      template <template <typename...> class E,
		typename... U>
      Variables(const E< Parameters<U...> >&			ensemble):
	element1(ensemble.element1),
	element2(ensemble.element2),
	element3(ensemble.element3),
	element4(ensemble.element4),
	element5(ensemble.element5),
	element6(ensemble.element6)
      {
	allege(Parameters<U...>::Quantum == P::Quantum);
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

      template <template <typename...> class E,
		typename... U>
      Variables(const E< Parameters<U...> >&			ensemble):
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

      template <template <typename...> class E,
		typename... U>
      Variables(const E< Parameters<U...> >&			ensemble):
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

      template <template <typename...> class E,
		typename... U>
      Variables(const E< Parameters<U...> >&			ensemble):
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
		  << "[Variables " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

  }
}

#endif
