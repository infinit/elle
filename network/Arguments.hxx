//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Arguments.hxx
//
// created       julien quintard   [wed feb 24 08:36:00 2010]
// updated       julien quintard   [wed feb 24 08:53:46 2010]
//

#ifndef ELLE_NETWORK_ARGUMENTS_HXX
#define ELLE_NETWORK_ARGUMENTS_HXX

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// these classes specialize the Arguments class depending on the number
    /// of arguments.
    ///

    ///
    /// zero-argument.
    ///
    template <const Tag G>
    class Arguments<G>:
      public Archivable
    {
    public:
      //
      // attributes
      //
      const Tag		tag;

      //
      // constructors & destructors
      //
      Arguments():
	tag(G)
      {
      }

      //
      // interface
      //

      // archivable

      Status		Serialize(Archive&			archive) const
      {
	// pack the arguments according to the parameters.
	if (Message<G>::P::Pack(archive) == StatusError)
	  escape("unable to pack the arguments");

	leave();
      }

      Status		Extract(Archive&			archive)
      {
	// unpack the arguments according to the parameters.
	if (Message<G>::P::Unpack(archive) == StatusError)
	  escape("unable to unpack the arguments");

	leave();
      }
    };

    ///
    /// signle-argument.
    ///
    template <const Tag G,
	      typename T1>
    class Arguments<G,
		    T1>
    {
    public:
      //
      // attributes
      //
      const Tag		tag;

      const T1&		argument1;

      //
      // constructors & destructors
      //
      Arguments(const T1&					o1):
	tag(G),
	argument1(o1)
      {
      }

      //
      // interfaces
      //

      // archivable

      Status		Serialize(Archive&			archive) const
      {
	// pack the arguments according to the parameters.
	if (Message<G>::P::Pack(this->argument1,
				archive) == StatusError)
	  escape("unable to pack the arguments");

	leave();
      }

      Status		Extract(Archive&			archive)
      {
	// unpack the arguments according to the parameters.
	if (Message<G>::P::Unpack(archive,
				  this->argument1) == StatusError)
	  escape("unable to unpack the arguments");

	leave();
      }
    };

    ///
    /// two-argument.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2>
    class Arguments<G,
		    T1, T2>
    {
    public:
      //
      // attributes
      //
      const Tag		tag;

      const T1&		argument1;
      const T2&		argument2;

      //
      // constructors & destructors
      //
      Arguments(const T1&					o1,
		const T2&					o2):
	tag(G),
	argument1(o1),
	argument2(o2)
      {
      }

      //
      // interfaces
      //

      // archivable

      Status		Serialize(Archive&			archive) const
      {
	// pack the arguments according to the parameters.
	if (Message<G>::P::Pack(this->argument1,
				this->argument2,
				archive) == StatusError)
	  escape("unable to pack the arguments");

	leave();
      }

      Status		Extract(Archive&			archive)
      {
	// unpack the arguments according to the parameters.
	if (Message<G>::P::Unpack(archive,
				  this->argument1,
				  this->argument2) == StatusError)
	  escape("unable to unpack the arguments");

	leave();
      }
    };

    ///
    /// three-argument.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2,
	      typename T3>
    class Arguments<G,
		    T1, T2, T3>
    {
    public:
      //
      // attributes
      //
      const Tag		tag;

      const T1&		argument1;
      const T2&		argument2;
      const T3&		argument3;

      //
      // constructors & destructors
      //
      Arguments(const T1&					o1,
		const T2&					o2,
		const T3&					o3):
	tag(G),
	argument1(o1),
	argument2(o2),
	argument3(o3)
      {
      }

      //
      // interfaces
      //

      // archivable

      Status		Serialize(Archive&			archive) const
      {
	// pack the arguments according to the parameters.
	if (Message<G>::P::Pack(this->argument1,
				this->argument2,
				this->argument3,
				archive) == StatusError)
	  escape("unable to pack the arguments");

	leave();
      }

      Status		Extract(Archive&			archive)
      {
	// unpack the arguments according to the parameters.
	if (Message<G>::P::Unpack(archive,
				  this->argument1,
				  this->argument2,
				  this->argument3) == StatusError)
	  escape("unable to unpack the arguments");

	leave();
      }
    };

    ///
    /// four-argument.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    class Arguments<G,
		    T1, T2, T3, T4>
    {
    public:
      //
      // attributes
      //
      const Tag		tag;

      const T1&		argument1;
      const T2&		argument2;
      const T3&		argument3;
      const T4&		argument4;

      //
      // constructors & destructors
      //
      Arguments(const T1&					o1,
		const T2&					o2,
		const T3&					o3,
		const T4&					o4):
	tag(G),
	argument1(o1),
	argument2(o2),
	argument3(o3),
	argument4(o4)
      {
      }

      //
      // interfaces
      //

      // archivable

      Status		Serialize(Archive&			archive) const
      {
	// pack the arguments according to the parameters.
	if (Message<G>::P::Pack(this->argument1,
				this->argument2,
				this->argument3,
				this->argument4,
				archive) == StatusError)
	  escape("unable to pack the arguments");

	leave();
      }

      Status		Extract(Archive&			archive)
      {
	// unpack the arguments according to the parameters.
	if (Message<G>::P::Unpack(archive,
				  this->argument1,
				  this->argument2,
				  this->argument3,
				  this->argument4) == StatusError)
	  escape("unable to unpack the arguments");

	leave();
      }
    };

    ///
    /// five-argument.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    class Arguments<G,
		    T1, T2, T3, T4, T5>
    {
    public:
      //
      // attributes
      //
      const Tag		tag;

      const T1&		argument1;
      const T2&		argument2;
      const T3&		argument3;
      const T4&		argument4;
      const T5&		argument5;

      //
      // constructors & destructors
      //
      Arguments(const T1&					o1,
		const T2&					o2,
		const T3&					o3,
		const T4&					o4,
		const T5&					o5):
	tag(G),
	argument1(o1),
	argument2(o2),
	argument3(o3),
	argument4(o4),
	argument5(o5)
      {
      }

      //
      // interfaces
      //

      // archivable

      Status		Serialize(Archive&			archive) const
      {
	// pack the arguments according to the parameters.
	if (Message<G>::P::Pack(this->argument1,
				this->argument2,
				this->argument3,
				this->argument4,
				this->argument5,
				archive) == StatusError)
	  escape("unable to pack the arguments");

	leave();
      }

      Status		Extract(Archive&			archive)
      {
	// unpack the arguments according to the parameters.
	if (Message<G>::P::Unpack(archive,
				  this->argument1,
				  this->argument2,
				  this->argument3,
				  this->argument4,
				  this->argument5) == StatusError)
	  escape("unable to unpack the arguments");

	leave();
      }
    };

    ///
    /// six-argument.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    class Arguments<G,
		    T1, T2, T3, T4, T5, T6>
    {
    public:
      //
      // attributes
      //
      const Tag		tag;

      const T1&		argument1;
      const T2&		argument2;
      const T3&		argument3;
      const T4&		argument4;
      const T5&		argument5;
      const T6&		argument6;

      //
      // constructors & destructors
      //
      Arguments(const T1&					o1,
		const T2&					o2,
		const T3&					o3,
		const T4&					o4,
		const T5&					o5,
		const T6&					o6):
	tag(G),
	argument1(o1),
	argument2(o2),
	argument3(o3),
	argument4(o4),
	argument5(o5),
	argument6(o6)
      {
      }

      //
      // interfaces
      //

      // archivable

      Status		Serialize(Archive&			archive) const
      {
	// pack the arguments according to the parameters.
	if (Message<G>::P::Pack(this->argument1,
				this->argument2,
				this->argument3,
				this->argument4,
				this->argument5,
				this->argument6,
				archive) == StatusError)
	  escape("unable to pack the arguments");

	leave();
      }

      Status		Extract(Archive&			archive)
      {
	// unpack the arguments according to the parameters.
	if (Message<G>::P::Unpack(archive,
				  this->argument1,
				  this->argument2,
				  this->argument3,
				  this->argument4,
				  this->argument5,
				  this->argument6) == StatusError)
	  escape("unable to unpack the arguments");

	leave();
      }
    };

    ///
    /// seven-argument.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    class Arguments<G,
		    T1, T2, T3, T4, T5, T6, T7>
    {
    public:
      //
      // attributes
      //
      const Tag		tag;

      const T1&		argument1;
      const T2&		argument2;
      const T3&		argument3;
      const T4&		argument4;
      const T5&		argument5;
      const T6&		argument6;
      const T7&		argument7;

      //
      // constructors & destructors
      //
      Arguments(const T1&					o1,
		const T2&					o2,
		const T3&					o3,
		const T4&					o4,
		const T5&					o5,
		const T6&					o6,
		const T7&					o7):
	tag(G),
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
      // interfaces
      //

      // archivable

      Status		Serialize(Archive&			archive) const
      {
	// pack the arguments according to the parameters.
	if (Message<G>::P::Pack(this->argument1,
				this->argument2,
				this->argument3,
				this->argument4,
				this->argument5,
				this->argument6,
				this->argument7,
				archive) == StatusError)
	  escape("unable to pack the arguments");

	leave();
      }

      Status		Extract(Archive&			archive)
      {
	// unpack the arguments according to the parameters.
	if (Message<G>::P::Unpack(archive,
				  this->argument1,
				  this->argument2,
				  this->argument3,
				  this->argument4,
				  this->argument5,
				  this->argument6,
				  this->argument7) == StatusError)
	  escape("unable to unpack the arguments");

	leave();
      }
    };

    ///
    /// eight-argument.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    class Arguments<G,
		    T1, T2, T3, T4, T5, T6, T7, T8>
    {
    public:
      //
      // attributes
      //
      const Tag		tag;

      const T1&		argument1;
      const T2&		argument2;
      const T3&		argument3;
      const T4&		argument4;
      const T5&		argument5;
      const T6&		argument6;
      const T7&		argument7;
      const T8&		argument8;

      //
      // constructors & destructors
      //
      Arguments(const T1&					o1,
		const T2&					o2,
		const T3&					o3,
		const T4&					o4,
		const T5&					o5,
		const T6&					o6,
		const T7&					o7,
		const T8&					o8):
	tag(G),
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
      // interfaces
      //

      // archivable

      Status		Serialize(Archive&			archive) const
      {
	// pack the arguments according to the parameters.
	if (Message<G>::P::Pack(this->argument1,
				this->argument2,
				this->argument3,
				this->argument4,
				this->argument5,
				this->argument6,
				this->argument7,
				this->argument8,
				archive) == StatusError)
	  escape("unable to pack the arguments");

	leave();
      }

      Status		Extract(Archive&			archive)
      {
	// unpack the arguments according to the parameters.
	if (Message<G>::P::Unpack(archive,
				  this->argument1,
				  this->argument2,
				  this->argument3,
				  this->argument4,
				  this->argument5,
				  this->argument6,
				  this->argument7,
				  this->argument8) == StatusError)
	  escape("unable to unpack the arguments");

	leave();
      }
    };

    ///
    /// nine-argument.
    ///
    template <const Tag G,
	      typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename T9>
    class Arguments<G,
		    T1, T2, T3, T4, T5, T6, T7, T8, T9>
    {
    public:
      //
      // attributes
      //
      const Tag		tag;

      const T1&		argument1;
      const T2&		argument2;
      const T3&		argument3;
      const T4&		argument4;
      const T5&		argument5;
      const T6&		argument6;
      const T7&		argument7;
      const T8&		argument8;
      const T9&		argument9;

      //
      // constructors & destructors
      //
      Arguments(const T1&					o1,
		const T2&					o2,
		const T3&					o3,
		const T4&					o4,
		const T5&					o5,
		const T6&					o6,
		const T7&					o7,
		const T8&					o8,
		const T9&					o9):
	tag(G),
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
      // interfaces
      //

      // archivable

      Status		Serialize(Archive&			archive) const
      {
	// pack the arguments according to the parameters.
	if (Message<G>::P::Pack(this->argument1,
				this->argument2,
				this->argument3,
				this->argument4,
				this->argument5,
				this->argument6,
				this->argument7,
				this->argument8,
				this->argument9,
				archive) == StatusError)
	  escape("unable to pack the arguments");

	leave();
      }

      Status		Extract(Archive&			archive)
      {
	// unpack the arguments according to the parameters.
	if (Message<G>::P::Unpack(archive,
				  this->argument1,
				  this->argument2,
				  this->argument3,
				  this->argument4,
				  this->argument5,
				  this->argument6,
				  this->argument7,
				  this->argument8,
				  this->argument9) == StatusError)
	  escape("unable to unpack the arguments");

	leave();
      }
    };

  }
}

#endif
