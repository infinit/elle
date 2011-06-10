//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Transaction.hxx
//
// created       julien quintard   [fri jun  3 13:06:41 2011]
// updated       julien quintard   [fri jun  3 13:57:17 2011]
//

#ifndef ETOILE_GEAR_TRANSACTION_HXX
#define ETOILE_GEAR_TRANSACTION_HXX

//
// ---------- includes --------------------------------------------------------
//

// XXX

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// these classes specialize the Transaction class depending on the number
    /// of arguments.
    ///

    ///
    /// zero-argument.
    ///
    template <const Operation O>
    class Transaction<>:
      public Event
    {
    public:
      //
      // attributes
      //
      const Operation	operation;

      //
      // constructors & destructors
      //
      Transaction():
	operation(O)
      {
      }

      //
      // interface
      //

      // archivable

      elle::Status	Serialize(elle::Archive&		archive) const
      {
	enter();

	// serialize the arguments.
	if (archive.Serialize((const elle::Natural8&)O) == elle::StatusError)
	  escape("unable to serialize the arguments");

	leave();
      }

      elle::Status	Extract(elle::Archive&			archive)
      {
	enter();


	leave();
      }
    };

    ///
    /// single-argument.
    ///
    template <typename T1>
    class Transaction<T1>:
      public Event
    {
    public:
      //
      // attributes
      //
      T1		argument1;

      //
      // constructors & destructors
      //
      Transaction(const Operation				operation,
		  T1&						o1):
	Event(operation),

	argument1(o1)
      {
      }

      //
      // interfaces
      //

      // archivable

      elle::Status	Serialize(elle::Archive&		archive) const
      {
	enter();

	// call the parent method.
	if (Event::Serialize(archive) == elle::StatusError)
	  escape("unable to serialize the parent");

	// serialize the arguments.
	if (archive.Serialize(this->argument1) == elle::StatusError)
	  escape("unable to serialize the arguments");

	leave();
      }

      elle::Status	Extract(elle::Archive&			archive)
      {
	enter();

	// extract the arguments.
	if (archive.Extract(this->argument1) == elle::StatusError)
	  escape("unable to unpack the arguments");

	leave();
      }
    };

    ///
    /// two-argument.
    ///
    template <typename T1,
	      typename T2>
    class Transaction<T1, T2>:
      public Event
    {
    public:
      //
      // attributes
      //
      T1&		argument1;
      T2&		argument2;

      //
      // constructors & destructors
      //
      Transaction(const Operation				operation,
		  T1&						o1,
		  T2&						o2):
	Event(operation),

	argument1(o1),
	argument2(o2)
      {
      }

      //
      // interfaces
      //

      // archivable

      elle::Status	Serialize(elle::Archive&		archive) const
      {
	enter();

	// call the parent method.
	if (Event::Serialize(archive) == elle::StatusError)
	  escape("unable to serialize the parent");

	// serialize the arguments.
	if (archive.Serialize(this->argument1,
			      this->argument2) == elle::StatusError)
	  escape("unable to serialize the arguments");

	leave();
      }

      elle::Status	Extract(elle::Archive&			archive)
      {
	enter();

	// extract the arguments.
	if (archive.Extract(this->argument1,
			    this->argument2) == elle::StatusError)
	  escape("unable to unpack the arguments");

	leave();
      }
    };

    ///
    /// three-argument.
    ///
    template <typename T1,
	      typename T2,
	      typename T3>
    class Transaction<T1, T2, T3>:
      public Event
    {
    public:
      //
      // attributes
      //
      T1&		argument1;
      T2&		argument2;
      T3&		argument3;

      //
      // constructors & destructors
      //
      Transaction(const Operation				operation,
		  T1&						o1,
		  T2&						o2,
		  T3&						o3):
	Event(operation),

	argument1(o1),
	argument2(o2),
	argument3(o3)
      {
      }

      //
      // interfaces
      //

      // archivable

      elle::Status	Serialize(elle::Archive&		archive) const
      {
	enter();

	// call the parent method.
	if (Event::Serialize(archive) == elle::StatusError)
	  escape("unable to serialize the parent");

	// serialize the arguments.
	if (archive.Serialize(this->argument1,
			      this->argument2,
			      this->argument3) == elle::StatusError)
	  escape("unable to serialize the arguments");

	leave();
      }

      elle::Status	Extract(elle::Archive&			archive)
      {
	enter();

	// extract the arguments.
	if (archive.Extract(this->argument1,
			    this->argument2,
			    this->argument3) == elle::StatusError)
	  escape("unable to unpack the arguments");

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
    class Transaction<T1, T2, T3, T4>:
      public Event
    {
    public:
      //
      // attributes
      //
      T1&		argument1;
      T2&		argument2;
      T3&		argument3;
      T4&		argument4;

      //
      // constructors & destructors
      //
      Transaction(const Operation				operation,
		  T1&						o1,
		  T2&						o2,
		  T3&						o3,
		  T4&						o4):
	Event(operation),

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

      elle::Status	Serialize(elle::Archive&		archive) const
      {
	enter();

	// call the parent method.
	if (Event::Serialize(archive) == elle::StatusError)
	  escape("unable to serialize the parent");

	// serialize the arguments.
	if (archive.Serialize(this->argument1,
			      this->argument2,
			      this->argument3,
			      this->argument4) == elle::StatusError)
	  escape("unable to serialize the arguments");

	leave();
      }

      elle::Status	Extract(elle::Archive&			archive)
      {
	enter();

	// extract the arguments.
	if (archive.Extract(this->argument1,
			    this->argument2,
			    this->argument3,
			    this->argument4) == elle::StatusError)
	  escape("unable to unpack the arguments");

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
    class Transaction<T1, T2, T3, T4, T5>:
      public Event
    {
    public:
      //
      // attributes
      //
      T1&		argument1;
      T2&		argument2;
      T3&		argument3;
      T4&		argument4;
      T5&		argument5;

      //
      // constructors & destructors
      //
      Transaction(const Operation				operation,
		  T1&						o1,
		  T2&						o2,
		  T3&						o3,
		  T4&						o4,
		  T5&						o5):
	Event(operation),

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

      elle::Status	Serialize(elle::Archive&		archive) const
      {
	enter();

	// call the parent method.
	if (Event::Serialize(archive) == elle::StatusError)
	  escape("unable to serialize the parent");

	// serialize the arguments.
	if (archive.Serialize(this->argument1,
			      this->argument2,
			      this->argument3,
			      this->argument4,
			      this->argument5) == elle::StatusError)
	  escape("unable to serialize the arguments");

	leave();
      }

      elle::Status	Extract(elle::Archive&			archive)
      {
	enter();

	// extract the arguments.
	if (archive.Extract(this->argument1,
			    this->argument2,
			    this->argument3,
			    this->argument4,
			    this->argument5) == elle::StatusError)
	  escape("unable to unpack the arguments");

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
    class Transaction<T1, T2, T3, T4, T5, T6>:
      public Event
    {
    public:
      //
      // attributes
      //
      T1&		argument1;
      T2&		argument2;
      T3&		argument3;
      T4&		argument4;
      T5&		argument5;
      T6&		argument6;

      //
      // constructors & destructors
      //
      Transaction(const Operation				operation,
		  T1&						o1,
		  T2&						o2,
		  T3&						o3,
		  T4&						o4,
		  T5&						o5,
		  T6&						o6):
	Event(operation),

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

      elle::Status	Serialize(elle::Archive&		archive) const
      {
	enter();

	// call the parent method.
	if (Event::Serialize(archive) == elle::StatusError)
	  escape("unable to serialize the parent");

	// serialize the arguments.
	if (archive.Serialize(this->argument1,
			      this->argument2,
			      this->argument3,
			      this->argument4,
			      this->argument5,
			      this->argument6) == elle::StatusError)
	  escape("unable to serialize the arguments");

	leave();
      }

      elle::Status	Extract(elle::Archive&			archive)
      {
	enter();

	// extract the arguments.
	if (archive.Extract(this->argument1,
			    this->argument2,
			    this->argument3,
			    this->argument4,
			    this->argument5,
			    this->argument6) == elle::StatusError)
	  escape("unable to unpack the arguments");

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
    class Transaction<T1, T2, T3, T4, T5, T6, T7>:
      public Event
    {
    public:
      //
      // attributes
      //
      T1&		argument1;
      T2&		argument2;
      T3&		argument3;
      T4&		argument4;
      T5&		argument5;
      T6&		argument6;
      T7&		argument7;

      //
      // constructors & destructors
      //
      Transaction(const Operation				operation,
		  T1&						o1,
		  T2&						o2,
		  T3&						o3,
		  T4&						o4,
		  T5&						o5,
		  T6&						o6,
		  T7&						o7):
	Event(operation),

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

      elle::Status	Serialize(elle::Archive&		archive) const
      {
	enter();

	// call the parent method.
	if (Event::Serialize(archive) == elle::StatusError)
	  escape("unable to serialize the parent");

	// serialize the arguments.
	if (archive.Serialize(this->argument1,
			      this->argument2,
			      this->argument3,
			      this->argument4,
			      this->argument5,
			      this->argument6,
			      this->argument7) == elle::StatusError)
	  escape("unable to serialize the arguments");

	leave();
      }

      elle::Status	Extract(elle::Archive&			archive)
      {
	enter();

	// extract the arguments.
	if (archive.Extract(this->argument1,
			    this->argument2,
			    this->argument3,
			    this->argument4,
			    this->argument5,
			    this->argument6,
			    this->argument7) == elle::StatusError)
	  escape("unable to unpack the arguments");

	leave();
      }
    };

    ///
    /// eight-argument.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    class Transaction<T1, T2, T3, T4, T5, T6, T7, T8>:
      public Event
    {
    public:
      //
      // attributes
      //
      T1&		argument1;
      T2&		argument2;
      T3&		argument3;
      T4&		argument4;
      T5&		argument5;
      T6&		argument6;
      T7&		argument7;
      T8&		argument8;

      //
      // constructors & destructors
      //
      Transaction(const Operation				operation,
		  T1&						o1,
		  T2&						o2,
		  T3&						o3,
		  T4&						o4,
		  T5&						o5,
		  T6&						o6,
		  T7&						o7,
		  T8&						o8):
	Event(operation),

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

      elle::Status	Serialize(elle::Archive&		archive) const
      {
	enter();

	// call the parent method.
	if (Event::Serialize(archive) == elle::StatusError)
	  escape("unable to serialize the parent");

	// serialize the arguments.
	if (archive.Serialize(this->argument1,
			      this->argument2,
			      this->argument3,
			      this->argument4,
			      this->argument5,
			      this->argument6,
			      this->argument7,
			      this->argument8) == elle::StatusError)
	  escape("unable to serialize the arguments");

	leave();
      }

      elle::Status	Extract(elle::Archive&			archive)
      {
	enter();

	// extract the arguments.
	if (archive.Extract(this->argument1,
			    this->argument2,
			    this->argument3,
			    this->argument4,
			    this->argument5,
			    this->argument6,
			    this->argument7,
			    this->argument8) == elle::StatusError)
	  escape("unable to unpack the arguments");

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
    class Transaction<T1, T2, T3, T4, T5, T6, T7, T8, T9>:
      public Event
    {
    public:
      //
      // attributes
      //
      T1&		argument1;
      T2&		argument2;
      T3&		argument3;
      T4&		argument4;
      T5&		argument5;
      T6&		argument6;
      T7&		argument7;
      T8&		argument8;
      T9&		argument9;

      //
      // constructors & destructors
      //
      Transaction(const Operation				operation,
		  T1&						o1,
		  T2&						o2,
		  T3&						o3,
		  T4&						o4,
		  T5&						o5,
		  T6&						o6,
		  T7&						o7,
		  T8&						o8,
		  T9&						o9):
	Event(operation),

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

      elle::Status	Serialize(elle::Archive&		archive) const
      {
	enter();

	// call the parent method.
	if (Event::Serialize(archive) == elle::StatusError)
	  escape("unable to serialize the parent");

	// serialize the arguments.
	if (archive.Serialize(this->argument1,
			      this->argument2,
			      this->argument3,
			      this->argument4,
			      this->argument5,
			      this->argument6,
			      this->argument7,
			      this->argument8,
			      this->argument9) == elle::StatusError)
	  escape("unable to serialize the arguments");

	leave();
      }

      elle::Status	Extract(elle::Archive&			archive)
      {
	enter();

	// extract the arguments.
	if (archive.Extract(this->argument1,
			    this->argument2,
			    this->argument3,
			    this->argument4,
			    this->argument5,
			    this->argument6,
			    this->argument7,
			    this->argument8,
			    this->argument9) == elle::StatusError)
	  escape("unable to unpack the arguments");

	leave();
      }
    };

  }
}

#endif
