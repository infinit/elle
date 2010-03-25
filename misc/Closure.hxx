//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Closure.hxx
//
// created       julien quintard   [thu mar 25 11:55:10 2010]
// updated       julien quintard   [thu mar 25 15:18:03 2010]
//

#ifndef ELLE_MISC_CLOSURE_HXX
#define ELLE_MISC_CLOSURE_HXX

namespace elle
{
  namespace misc
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// zero-parameter closure.
    ///
    template <>
    class Closure<>
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Callback<>&					callback):
	callback(callback)
      {
      }

      //
      // methods
      //
      Status		Call()
      {
	return (this->callback.Call());
      }

      //
      // attributes
      //
      Callback<>&	callback;
    };

    ///
    /// one-parameter closure.
    ///
    template <typename T1>
    class Closure<T1>
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Callback<T1>&					callback,
	      T1&						o1):
	callback(callback),
	o1(o1)
      {
      }

      //
      // methods
      //
      Status		Call()
      {
	return (this->callback.Call(this->o1));
      }

      //
      // attributes
      //
      Callback<T1>	callback;

      T1&		o1;
    };

    ///
    /// two-parameter closure.
    ///
    template <typename T1,
	      typename T2>
    class Closure<T1, T2>
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Callback<T1, T2>&					callback,
	      T1&						o1,
	      T2&						o2):
	callback(callback),
	o1(o1),
	o2(o2)
      {
      }

      //
      // methods
      //
      Status		Call()
      {
	return (this->callback.Call(this->o1,
				   this->o2));
      }

      //
      // attributes
      //
      Callback<T1,
	      T2>	callback;

      T1&		o1;
      T2&		o2;
    };

    ///
    /// three-parameter closure.
    ///
    template <typename T1,
	      typename T2,
	      typename T3>
    class Closure<T1, T2, T3>
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Callback<T1, T2, T3>&				callback,
	      T1&						o1,
	      T2&						o2,
	      T3&						o3):
	callback(callback),
	o1(o1),
	o2(o2),
	o3(o3)
      {
      }

      //
      // methods
      //
      Status		Call()
      {
	return (this->callback.Call(this->o1,
				   this->o2,
				   this->o3));
      }

      //
      // attributes
      //
      Callback<T1,
	      T2,
	      T3>	callback;

      T1&		o1;
      T2&		o2;
      T3&		o3;
    };

    ///
    /// four-parameter closure.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    class Closure<T1, T2, T3, T4>
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Callback<T1, T2, T3, T4>&				callback,
	      T1&						o1,
	      T2&						o2,
	      T3&						o3,
	      T4&						o4):
	callback(callback),
	o1(o1),
	o2(o2),
	o3(o3),
	o4(o4)
      {
      }

      //
      // methods
      //
      Status		Call()
      {
	return (this->callback.Call(this->o1,
				   this->o2,
				   this->o3,
				   this->o4));
      }

      //
      // attributes
      //
      Callback<T1,
	      T2,
	      T3,
	      T4>	callback;

      T1&		o1;
      T2&		o2;
      T3&		o3;
      T4&		o4;
    };

    ///
    /// five-parameter closure.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    class Closure<T1, T2, T3, T4, T5>
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Callback<T1, T2, T3, T4, T5>&			callback,
	      T1&						o1,
	      T2&						o2,
	      T3&						o3,
	      T4&						o4,
	      T5&						o5):
	callback(callback),
	o1(o1),
	o2(o2),
	o3(o3),
	o4(o4),
	o5(o5)
      {
      }

      //
      // methods
      //
      Status		Call()
      {
	return (this->callback.Call(this->o1,
				   this->o2,
				   this->o3,
				   this->o4,
				   this->o5));
      }

      //
      // attributes
      //
      Callback<T1,
	      T2,
	      T3,
	      T4,
	      T5>	callback;

      T1&		o1;
      T2&		o2;
      T3&		o3;
      T4&		o4;
      T5&		o5;
    };

    ///
    /// six-parameter closure.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    class Closure<T1, T2, T3, T4, T5, T6>
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Callback<T1, T2, T3, T4, T5, T6>&			callback,
	      T1&						o1,
	      T2&						o2,
	      T3&						o3,
	      T4&						o4,
	      T5&						o5,
	      T6&						o6):
	callback(callback),
	o1(o1),
	o2(o2),
	o3(o3),
	o4(o4),
	o5(o5),
	o6(o6)
      {
      }

      //
      // methods
      //
      Status		Call()
      {
	return (this->callback.Call(this->o1,
				   this->o2,
				   this->o3,
				   this->o4,
				   this->o5,
				   this->o6));
      }

      //
      // attributes
      //
      Callback<T1,
	      T2,
	      T3,
	      T4,
	      T5,
	      T6>	callback;

      T1&		o1;
      T2&		o2;
      T3&		o3;
      T4&		o4;
      T5&		o5;
      T6&		o6;
    };

    ///
    /// seven-parameter closure.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    class Closure<T1, T2, T3, T4, T5, T6, T7>
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Callback<T1, T2, T3, T4, T5, T6, T7>&		callback,
	      T1&						o1,
	      T2&						o2,
	      T3&						o3,
	      T4&						o4,
	      T5&						o5,
	      T6&						o6,
	      T7&						o7):
	callback(callback),
	o1(o1),
	o2(o2),
	o3(o3),
	o4(o4),
	o5(o5),
	o6(o6),
	o7(o7)
      {
      }

      //
      // methods
      //
      Status		Call()
      {
	return (this->callback.Call(this->o1,
				   this->o2,
				   this->o3,
				   this->o4,
				   this->o5,
				   this->o6,
				   this->o7));
      }

      //
      // attributes
      //
      Callback<T1,
	      T2,
	      T3,
	      T4,
	      T5,
	      T6,
	      T7>	callback;

      T1&		o1;
      T2&		o2;
      T3&		o3;
      T4&		o4;
      T5&		o5;
      T6&		o6;
      T7&		o7;
    };

    ///
    /// eight-parameter closure.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    class Closure<T1, T2, T3, T4, T5, T6, T7, T8>
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Callback<T1, T2, T3, T4, T5, T6, T7, T8>&		callback,
	      T1&						o1,
	      T2&						o2,
	      T3&						o3,
	      T4&						o4,
	      T5&						o5,
	      T6&						o6,
	      T7&						o7,
	      T8&						o8):
	callback(callback),
	o1(o1),
	o2(o2),
	o3(o3),
	o4(o4),
	o5(o5),
	o6(o6),
	o7(o7),
	o8(o8)
      {
      }

      //
      // methods
      //
      Status		Call()
      {
	return (this->callback.Call(this->o1,
				   this->o2,
				   this->o3,
				   this->o4,
				   this->o5,
				   this->o6,
				   this->o7,
				   this->o8));
      }

      //
      // attributes
      //
      Callback<T1,
	      T2,
	      T3,
	      T4,
	      T5,
	      T6,
	      T7,
	      T8>	callback;

      T1&		o1;
      T2&		o2;
      T3&		o3;
      T4&		o4;
      T5&		o5;
      T6&		o6;
      T7&		o7;
      T8&		o8;
    };

    ///
    /// nine-parameter closure.
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
    class Closure<T1, T2, T3, T4, T5, T6, T7, T8, T9>
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Callback<T1, T2, T3, T4, T5, T6, T7, T8, T9>&	callback,
	      T1&						o1,
	      T2&						o2,
	      T3&						o3,
	      T4&						o4,
	      T5&						o5,
	      T6&						o6,
	      T7&						o7,
	      T8&						o8,
	      T9&						o9):
	callback(callback),
	o1(o1),
	o2(o2),
	o3(o3),
	o4(o4),
	o5(o5),
	o6(o6),
	o7(o7),
	o8(o8),
	o9(o9)
      {
      }

      //
      // methods
      //
      Status		Call()
      {
	return (this->callback.Call(this->o1,
				   this->o2,
				   this->o3,
				   this->o4,
				   this->o5,
				   this->o6,
				   this->o7,
				   this->o8,
				   this->o9));
      }

      //
      // attributes
      //
      Callback<T1,
	      T2,
	      T3,
	      T4,
	      T5,
	      T6,
	      T7,
	      T8,
	      T9>&	callback;

      T1&		o1;
      T2&		o2;
      T3&		o3;
      T4&		o4;
      T5&		o5;
      T6&		o6;
      T7&		o7;
      T8&		o8;
      T9&		o9;
    };

  }
}

#endif
