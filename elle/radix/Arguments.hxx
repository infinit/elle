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
// updated       julien quintard   [fri jun 10 13:02:55 2011]
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
    /// these classes specialize the Arguments class depending on the number
    /// of arguments.
    ///
    /// note that the template specializations give meaning to the
    /// parameters i.e P<T...>.
    ///

    ///
    /// zero-argument.
    ///
    template <>
    class Arguments< Parameters<> >:
      public Entity,
      public Base<Arguments>
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
      public Entity,
      public Base<Arguments>
    {
    public:
      //
      // types
      //
      typedef Parameters<T1>					P;

      //
      // attributes
      //
      T1		argument1;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1):
	argument1(o1)
      {
      }

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1) == StatusError)
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
      public Entity,
      public Base<Arguments>
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2>				P;

      //
      // attributes
      //
      T1		argument1;
      T2		argument2;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1,
		T2&						o2):
	argument1(o1),
	argument2(o2)
      {
      }

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2) == StatusError)
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
      public Entity,
      public Base<Arguments>
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2, T3>				P;

      //
      // attributes
      //
      T1		argument1;
      T2		argument2;
      T3		argument3;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1,
		T2&						o2,
		T3&						o3):
	argument1(o1),
	argument2(o2),
	argument3(o3)
      {
      }

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2,
			   this->argument3) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2,
			   this->argument3) == StatusError)
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
      public Entity,
      public Base<Arguments>
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2, T3, T4>			P;

      //
      // attributes
      //
      T1		argument1;
      T2		argument2;
      T3		argument3;
      T4		argument4;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1,
		T2&						o2,
		T3&						o3,
		T4&						o4):
	argument1(o1),
	argument2(o2),
	argument3(o3),
	argument4(o4)
      {
      }

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2,
			   this->argument3,
			   this->argument4) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2,
			   this->argument3,
			   this->argument4) == StatusError)
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
      public Entity,
      public Base<Arguments>
    {
    public:
      //
      // types
      //
      typedef Parameters<T1, T2, T3, T4, T5>			P;

      //
      // attributes
      //
      T1		argument1;
      T2		argument2;
      T3		argument3;
      T4		argument4;
      T5		argument5;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1,
		T2&						o2,
		T3&						o3,
		T4&						o4,
		T5&						o5):
	argument1(o1),
	argument2(o2),
	argument3(o3),
	argument4(o4),
	argument5(o5)
      {
      }

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2,
			   this->argument3,
			   this->argument4,
			   this->argument5) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2,
			   this->argument3,
			   this->argument4,
			   this->argument5) == StatusError)
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
      public Entity,
      public Base<Arguments>
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
      T1		argument1;
      T2		argument2;
      T3		argument3;
      T4		argument4;
      T5		argument5;
      T6		argument6;

      //
      // constructors & destructors
      //
      Arguments(T1&						o1,
		T2&						o2,
		T3&						o3,
		T4&						o4,
		T5&						o5,
		T6&						o6):
	argument1(o1),
	argument2(o2),
	argument3(o3),
	argument4(o4),
	argument5(o5),
	argument6(o6)
      {
      }

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2,
			   this->argument3,
			   this->argument4,
			   this->argument5,
			   this->argument6) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2,
			   this->argument3,
			   this->argument4,
			   this->argument5,
			   this->argument6) == StatusError)
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
      public Entity,
      public Base<Arguments>
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
      T1		argument1;
      T2		argument2;
      T3		argument3;
      T4		argument4;
      T5		argument5;
      T6		argument6;
      T7		argument7;

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
	argument1(o1),
	argument2(o2),
	argument3(o3),
	argument4(o4),
	argument5(o5),
	argument6(o6),
	argument7(o7)
      {
      }

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2,
			   this->argument3,
			   this->argument4,
			   this->argument5,
			   this->argument6,
			   this->argument7) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2,
			   this->argument3,
			   this->argument4,
			   this->argument5,
			   this->argument6,
			   this->argument7) == StatusError)
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
      public Entity,
      public Base<Arguments>
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
      T1		argument1;
      T2		argument2;
      T3		argument3;
      T4		argument4;
      T5		argument5;
      T6		argument6;
      T7		argument7;
      T8		argument8;

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
	argument1(o1),
	argument2(o2),
	argument3(o3),
	argument4(o4),
	argument5(o5),
	argument6(o6),
	argument7(o7),
	argument8(o8)
      {
      }

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2,
			   this->argument3,
			   this->argument4,
			   this->argument5,
			   this->argument6,
			   this->argument7,
			   this->argument8) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2,
			   this->argument3,
			   this->argument4,
			   this->argument5,
			   this->argument6,
			   this->argument7,
			   this->argument8) == StatusError)
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
      public Entity,
      public Base<Arguments>
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
      T1		argument1;
      T2		argument2;
      T3		argument3;
      T4		argument4;
      T5		argument5;
      T6		argument6;
      T7		argument7;
      T8		argument8;
      T9		argument9;

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
	argument1(o1),
	argument2(o2),
	argument3(o3),
	argument4(o4),
	argument5(o5),
	argument6(o6),
	argument7(o7),
	argument8(o8),
	argument9(o9)
      {
      }

      //
      // methods
      //

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object)
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2,
			   this->argument3,
			   this->argument4,
			   this->argument5,
			   this->argument6,
			   this->argument7,
			   this->argument8,
			   this->argument9) == StatusError)
	  escape("unable to trigger the object");

	leave();
      }

      template <template <typename...> class C, typename...>
      Status		Trigger(const C<P>&			object) const
      {
	enter();

	// trigger the object.
	if (object.Trigger(this->argument1,
			   this->argument2,
			   this->argument3,
			   this->argument4,
			   this->argument5,
			   this->argument6,
			   this->argument7,
			   this->argument8,
			   this->argument9) == StatusError)
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
		  << "[Arguments " << P::Quantum << "]"
		  << std::endl;

	leave();
      }
    };

  }
}

#endif
