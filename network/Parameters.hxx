//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Parameters.hxx
//
// created       julien quintard   [sun feb 21 15:29:32 2010]
// updated       julien quintard   [mon feb 22 14:38:37 2010]
//

#ifndef ELLE_NETWORK_PARAMETERS_HXX
#define ELLE_NETWORK_PARAMETERS_HXX

namespace elle
{
  namespace network
  {

    ///
    /// this type is used for convenience purpose only.
    ///

    ///
    /// zero parameter.
    ///
    template <>
    class Parameters<>
    {
    public:
      static const Natural32	Count = 0;
    };

    ///
    /// one parameter.
    ///
    template <typename T1>
    class Parameters<T1>
    {
    public:
      static const Natural32	Count = 1;

      typedef T1		Parameter1;
    };

    ///
    /// two parameter.
    ///
    template <typename T1,
	      typename T2>
    class Parameters<T1, T2>
    {
    public:
      static const Natural32	Count = 2;

      typedef T1		Parameter1;
      typedef T2		Parameter2;
    };

    ///
    /// three parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3>
    class Parameters<T1, T2, T3>
    {
    public:
      static const Natural32	Count = 3;

      typedef T1		Parameter1;
      typedef T2		Parameter2;
      typedef T3		Parameter3;
    };

    ///
    /// four parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    class Parameters<T1, T2, T3, T4>
    {
    public:
      static const Natural32	Count = 4;

      typedef T1		Parameter1;
      typedef T2		Parameter2;
      typedef T3		Parameter3;
      typedef T4		Parameter4;
    };

    ///
    /// five parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    class Parameters<T1, T2, T3, T4, T5>
    {
    public:
      static const Natural32	Count = 5;

      typedef T1		Parameter1;
      typedef T2		Parameter2;
      typedef T3		Parameter3;
      typedef T4		Parameter4;
      typedef T5		Parameter5;
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
    class Parameters<T1, T2, T3, T4, T5, T6>
    {
    public:
      static const Natural32	Count = 6;

      typedef T1		Parameter1;
      typedef T2		Parameter2;
      typedef T3		Parameter3;
      typedef T4		Parameter4;
      typedef T5		Parameter5;
      typedef T6		Parameter6;
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
    class Parameters<T1, T2, T3, T4, T5, T6, T7>
    {
    public:
      static const Natural32	Count = 7;

      typedef T1		Parameter1;
      typedef T2		Parameter2;
      typedef T3		Parameter3;
      typedef T4		Parameter4;
      typedef T5		Parameter5;
      typedef T6		Parameter6;
      typedef T7		Parameter7;
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
    class Parameters<T1, T2, T3, T4, T5, T6, T7, T8>
    {
    public:
      static const Natural32	Count = 8;

      typedef T1		Parameter1;
      typedef T2		Parameter2;
      typedef T3		Parameter3;
      typedef T4		Parameter4;
      typedef T5		Parameter5;
      typedef T6		Parameter6;
      typedef T7		Parameter7;
      typedef T8		Parameter8;
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
    class Parameters<T1, T2, T3, T4, T5, T6, T7, T8, T9>
    {
    public:
      static const Natural32	Count = 9;

      typedef T1		Parameter1;
      typedef T2		Parameter2;
      typedef T3		Parameter3;
      typedef T4		Parameter4;
      typedef T5		Parameter5;
      typedef T6		Parameter6;
      typedef T7		Parameter7;
      typedef T8		Parameter8;
      typedef T9		Parameter9;
    };

  }
}

#endif
