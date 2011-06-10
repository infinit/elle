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
// updated       julien quintard   [fri jun 10 12:57:55 2011]
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
#include <elle/radix/Base.hh>

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
      public Entity,
      public Base<Variables>
    {
    public:
      //
      // types
      //
      typedef Parameters<>					P;

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger() == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger() == StatusError)
	  escape("unable to trigger the object");

	leave();
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
      public Entity,
      public Base<Variables>
    {
    public:
      //
      // types
      //
      typedef Parameters<T1>					P;

      //
      // attributes
      //
      T1		variable1;

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1) == StatusError)
	  escape("unable to trigger the object");

	leave();
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
      public Entity,
      public Base<Variables>
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2>				P;

      //
      // attributes
      //
      T1		variable1;
      T2		variable2;

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2) == StatusError)
	  escape("unable to trigger the object");

	leave();
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
      public Entity,
      public Base<Variables>
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2, T3>				P;

      //
      // attributes
      //
      T1		variable1;
      T2		variable2;
      T3		variable3;

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3) == StatusError)
	  escape("unable to trigger the object");

	leave();
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
      public Entity,
      public Base<Variables>
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2, T3, T4>			P;

      //
      // attributes
      //
      T1		variable1;
      T2		variable2;
      T3		variable3;
      T4		variable4;

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4) == StatusError)
	  escape("unable to trigger the object");

	leave();
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
      public Entity,
      public Base<Variables>
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2, T3, T4, T5>			P;

      //
      // attributes
      //
      T1		variable1;
      T2		variable2;
      T3		variable3;
      T4		variable4;
      T5		variable5;

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5) == StatusError)
	  escape("unable to trigger the object");

	leave();
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
      public Entity,
      public Base<Variables>
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
      T1		variable1;
      T2		variable2;
      T3		variable3;
      T4		variable4;
      T5		variable5;
      T6		variable6;

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6) == StatusError)
	  escape("unable to trigger the object");

	leave();
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
      public Entity,
      public Base<Variables>
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
      T1		variable1;
      T2		variable2;
      T3		variable3;
      T4		variable4;
      T5		variable5;
      T6		variable6;
      T7		variable7;

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6,
			   this->variable7) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6,
			   this->variable7) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6,
			   this->variable7) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6,
			   this->variable7) == StatusError)
	  escape("unable to trigger the object");

	leave();
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
      public Entity,
      public Base<Variables>
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
      T1		variable1;
      T2		variable2;
      T3		variable3;
      T4		variable4;
      T5		variable5;
      T6		variable6;
      T7		variable7;
      T8		variable8;

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6,
			   this->variable7,
			   this->variable8) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6,
			   this->variable7,
			   this->variable8) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6,
			   this->variable7,
			   this->variable8) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6,
			   this->variable7,
			   this->variable8) == StatusError)
	  escape("unable to trigger the object");

	leave();
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
      public Entity,
      public Base<Variables>
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
      T1		variable1;
      T2		variable2;
      T3		variable3;
      T4		variable4;
      T5		variable5;
      T6		variable6;
      T7		variable7;
      T8		variable8;
      T9		variable9;

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6,
			   this->variable7,
			   this->variable8,
			   this->variable9) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6,
			   this->variable7,
			   this->variable8,
			   this->variable9) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6,
			   this->variable7,
			   this->variable8,
			   this->variable9) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<
				  typename Trait::Constant<P>::Type
				  >&				object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->variable1,
			   this->variable2,
			   this->variable3,
			   this->variable4,
			   this->variable5,
			   this->variable6,
			   this->variable7,
			   this->variable8,
			   this->variable9) == StatusError)
	  escape("unable to trigger the object");

	leave();
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
