//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/network/Parameters.hxx
//
// created       julien quintard   [sun feb 21 15:29:32 2010]
// updated       julien quintard   [mon apr 26 18:30:03 2010]
//

#ifndef ELLE_NETWORK_PARAMETERS_HXX
#define ELLE_NETWORK_PARAMETERS_HXX

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// zero parameter.
    ///
    template <>
    class Parameters<>:
      public Meta
    {
    public:
      //
      // constants
      //
      static const Natural32	Quantum = 0;

      //
      // methods
      //
      static Status	Pack(Archive&)
      {
	enter();
	leave();
      }

      static Status	Unpack(Archive&)
      {
	enter();
	leave();
      }
    };

    ///
    /// one parameter.
    ///
    template <typename T1>
    class Parameters<const T1>:
      public Meta
    {
    public:
      //
      // constants
      //
      static const Natural32	Quantum = 1;

      //
      // types
      //
      typedef T1		P1;

      //
      // methods
      //
      static Status	Pack(const P1&				o1,
			     Archive&				archive)
      {
	enter();

	// serialize the objects.
	if (archive.Serialize(o1) == StatusError)
	  escape("unable to serialize the objects");

	leave();
      }

      static Status	Unpack(Archive&				archive,
			       P1&				o1)
      {
	enter();

	// extract the objects.
	if (archive.Extract(o1) == StatusError)
	  escape("unable to extract the objects");

	leave();
      }
    };

    ///
    /// two parameter.
    ///
    template <typename T1,
	      typename T2>
    class Parameters<const T1,
		     const T2>:
      public Meta
    {
    public:
      //
      // constants
      //
      static const Natural32	Quantum = 2;

      //
      // types
      //
      typedef T1		P1;
      typedef T2		P2;

      //
      // methods
      //
      static Status	Pack(const P1&				o1,
			     const P2&				o2,
			     Archive&				archive)
      {
	enter();

	// serialize the objects.
	if (archive.Serialize(o1, o2) == StatusError)
	  escape("unable to serialize the objects");

	leave();
      }

      static Status	Unpack(Archive&				archive,
			       P1&				o1,
			       P2&				o2)
      {
	enter();

	// extract the objects.
	if (archive.Extract(o1, o2) == StatusError)
	  escape("unable to extract the objects");

	leave();
      }
    };

    ///
    /// three parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3>
    class Parameters<const T1,
		     const T2,
		     const T3>:
      public Meta
    {
    public:
      //
      // constants
      //
      static const Natural32	Quantum = 3;

      //
      // types
      //
      typedef T1		P1;
      typedef T2		P2;
      typedef T3		P3;

      //
      // methods
      //
      static Status	Pack(const P1&				o1,
			     const P2&				o2,
			     const P3&				o3,
			     Archive&				archive)
      {
	enter();

	// serialize the objects.
	if (archive.Serialize(o1, o2, o3) == StatusError)
	  escape("unable to serialize the objects");

	leave();
      }

      static Status	Unpack(Archive&				archive,
			       P1&				o1,
			       P2&				o2,
			       P3&				o3)
      {
	enter();

	// extract the objects.
	if (archive.Extract(o1, o2, o3) == StatusError)
	  escape("unable to extract the objects");

	leave();
      }
    };

    ///
    /// four parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    class Parameters<const T1,
		     const T2,
		     const T3,
		     const T4>:
      public Meta
    {
    public:
      //
      // constants
      //
      static const Natural32	Quantum = 4;

      //
      // types
      //
      typedef T1		P1;
      typedef T2		P2;
      typedef T3		P3;
      typedef T4		P4;

      //
      // methods
      //
      static Status	Pack(const P1&				o1,
			     const P2&				o2,
			     const P3&				o3,
			     const P4&				o4,
			     Archive&				archive)
      {
	enter();

	// serialize the objects.
	if (archive.Serialize(o1, o2, o3, o4) == StatusError)
	  escape("unable to serialize the objects");

	leave();
      }

      static Status	Unpack(Archive&				archive,
			       P1&				o1,
			       P2&				o2,
			       P3&				o3,
			       P4&				o4)
      {
	enter();

	// extract the objects.
	if (archive.Extract(o1, o2, o3, o4) == StatusError)
	  escape("unable to extract the objects");

	leave();
      }
    };

    ///
    /// five parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    class Parameters<const T1,
		     const T2,
		     const T3,
		     const T4,
		     const T5>:
      public Meta
    {
    public:
      //
      // constants
      //
      static const Natural32	Quantum = 5;

      //
      // types
      //
      typedef T1		P1;
      typedef T2		P2;
      typedef T3		P3;
      typedef T4		P4;
      typedef T5		P5;

      //
      // methods
      //
      static Status	Pack(const P1&				o1,
			     const P2&				o2,
			     const P3&				o3,
			     const P4&				o4,
			     const P5&				o5,
			     Archive&				archive)
      {
	enter();

	// serialize the objects.
	if (archive.Serialize(o1, o2, o3, o4, o5) == StatusError)
	  escape("unable to serialize the objects");

	leave();
      }

      static Status	Unpack(Archive&				archive,
			       P1&				o1,
			       P2&				o2,
			       P3&				o3,
			       P4&				o4,
			       P5&				o5)
      {
	enter();

	// extract the objects.
	if (archive.Extract(o1, o2, o3, o4, o5) == StatusError)
	  escape("unable to extract the objects");

	leave();
      }
    };

    ///
    /// six parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    class Parameters<const T1,
		     const T2,
		     const T3,
		     const T4,
		     const T5,
		     const T6>:
      public Meta
    {
    public:
      //
      // constants
      //
      static const Natural32	Quantum = 6;

      //
      // types
      //
      typedef T1		P1;
      typedef T2		P2;
      typedef T3		P3;
      typedef T4		P4;
      typedef T5		P5;
      typedef T6		P6;

      //
      // methods
      //
      static Status	Pack(const P1&				o1,
			     const P2&				o2,
			     const P3&				o3,
			     const P4&				o4,
			     const P5&				o5,
			     const P6&				o6,
			     Archive&				archive)
      {
	enter();

	// serialize the objects.
	if (archive.Serialize(o1, o2, o3, o4, o5,
			      o6) == StatusError)
	  escape("unable to serialize the objects");

	leave();
      }

      static Status	Unpack(Archive&				archive,
			       P1&				o1,
			       P2&				o2,
			       P3&				o3,
			       P4&				o4,
			       P5&				o5,
			       P6&				o6)
      {
	enter();

	// extract the objects.
	if (archive.Extract(o1, o2, o3, o4, o5,
			    o6) == StatusError)
	  escape("unable to extract the objects");

	leave();
      }
    };

    ///
    /// seven parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    class Parameters<const T1,
		     const T2,
		     const T3,
		     const T4,
		     const T5,
		     const T6,
		     const T7>:
      public Meta
    {
    public:
      //
      // constants
      //
      static const Natural32	Quantum = 7;

      //
      // types
      //
      typedef T1		P1;
      typedef T2		P2;
      typedef T3		P3;
      typedef T4		P4;
      typedef T5		P5;
      typedef T6		P6;
      typedef T7		P7;

      //
      // methods
      //
      static Status	Pack(const P1&				o1,
			     const P2&				o2,
			     const P3&				o3,
			     const P4&				o4,
			     const P5&				o5,
			     const P6&				o6,
			     const P7&				o7,
			     Archive&				archive)
      {
	enter();

	// serialize the objects.
	if (archive.Serialize(o1, o2, o3, o4, o5,
			      o6, o7) == StatusError)
	  escape("unable to serialize the objects");

	leave();
      }

      static Status	Unpack(Archive&				archive,
			       P1&				o1,
			       P2&				o2,
			       P3&				o3,
			       P4&				o4,
			       P5&				o5,
			       P6&				o6,
			       P7&				o7)
      {
	enter();

	// extract the objects.
	if (archive.Extract(o1, o2, o3, o4, o5,
			    o6, o7) == StatusError)
	  escape("unable to extract the objects");

	leave();
      }
    };

    ///
    /// eight parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    class Parameters<const T1,
		     const T2,
		     const T3,
		     const T4,
		     const T5,
		     const T6,
		     const T7,
		     const T8>:
      public Meta
    {
    public:
      //
      // constants
      //
      static const Natural32	Quantum = 8;

      //
      // types
      //
      typedef T1		P1;
      typedef T2		P2;
      typedef T3		P3;
      typedef T4		P4;
      typedef T5		P5;
      typedef T6		P6;
      typedef T7		P7;
      typedef T8		P8;

      //
      // methods
      //
      static Status	Pack(const P1&				o1,
			     const P2&				o2,
			     const P3&				o3,
			     const P4&				o4,
			     const P5&				o5,
			     const P6&				o6,
			     const P7&				o7,
			     const P8&				o8,
			     Archive&				archive)
      {
	enter();

	// serialize the objects.
	if (archive.Serialize(o1, o2, o3, o4, o5,
			      o6, o7, o8) == StatusError)
	  escape("unable to serialize the objects");

	leave();
      }

      static Status	Unpack(Archive&				archive,
			       P1&				o1,
			       P2&				o2,
			       P3&				o3,
			       P4&				o4,
			       P5&				o5,
			       P6&				o6,
			       P7&				o7,
			       P8&				o8)
      {
	enter();

	// extract the objects.
	if (archive.Extract(o1, o2, o3, o4, o5,
			    o6, o7, o8) == StatusError)
	  escape("unable to extract the objects");

	leave();
      }
    };

    ///
    /// nine parameter.
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
    class Parameters<const T1,
		     const T2,
		     const T3,
		     const T4,
		     const T5,
		     const T6,
		     const T7,
		     const T8,
		     const T9>:
      public Meta
    {
    public:
      //
      // constants
      //
      static const Natural32	Quantum = 9;

      //
      // types
      //
      typedef T1		P1;
      typedef T2		P2;
      typedef T3		P3;
      typedef T4		P4;
      typedef T5		P5;
      typedef T6		P6;
      typedef T7		P7;
      typedef T8		P8;
      typedef T9		P9;

      //
      // methods
      //
      static Status	Pack(const P1&				o1,
			     const P2&				o2,
			     const P3&				o3,
			     const P4&				o4,
			     const P5&				o5,
			     const P6&				o6,
			     const P7&				o7,
			     const P8&				o8,
			     const P9&				o9,
			     Archive&				archive)
      {
	enter();

	// serialize the objects.
	if (archive.Serialize(o1, o2, o3, o4, o5,
			      o6, o7, o8, o9) == StatusError)
	  escape("unable to serialize the objects");

	leave();
      }

      static Status	Unpack(Archive&				archive,
			       P1&				o1,
			       P2&				o2,
			       P3&				o3,
			       P4&				o4,
			       P5&				o5,
			       P6&				o6,
			       P7&				o7,
			       P8&				o8,
			       P9&				o9)
      {
	enter();

	// extract the objects.
	if (archive.Extract(o1, o2, o3, o4, o5,
			    o6, o7, o8, o9) == StatusError)
	  escape("unable to extract the objects");

	leave();
      }
    };

  }
}

#endif
