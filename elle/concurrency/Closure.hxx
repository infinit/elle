//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Closure.hxx
//
// created       julien quintard   [thu mar 25 11:55:10 2010]
// updated       julien quintard   [sun may  2 17:13:08 2010]
//

#ifndef ELLE_CONCURRENCY_CLOSURE_HXX
#define ELLE_CONCURRENCY_CLOSURE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Void.hh>

#include <elle/radix/Entity.hh>

#include <elle/concurrency/Entrance.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// zero-parameter closure.
    ///
    template <>
    class Closure<>:
      public Entity
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Entrance<>&					entrance):
	entrance(entrance)
      {
      }

      //
      // methods
      //
      Void		Trigger()
      {
	this->entrance.Trigger();
      }

      //
      // attributes
      //
      Entrance<>&	entrance;
    };

    ///
    /// one-parameter closure.
    ///
    template <typename T1>
    class Closure<T1>:
      public Entity
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Entrance<T1>&					entrance,
	      T1&						o1):
	entrance(entrance),
	o1(o1)
      {
      }

      //
      // methods
      //
      Void		Trigger()
      {
	this->entrance.Trigger(this->o1);
      }

      //
      // attributes
      //
      Entrance<T1>	entrance;

      T1&		o1;
    };

    ///
    /// two-parameter closure.
    ///
    template <typename T1,
	      typename T2>
    class Closure<T1, T2>:
      public Entity
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Entrance<T1, T2>&					entrance,
	      T1&						o1,
	      T2&						o2):
	entrance(entrance),
	o1(o1),
	o2(o2)
      {
      }

      //
      // methods
      //
      Void		Trigger()
      {
	this->entrance.Trigger(this->o1,
			       this->o2);
      }

      //
      // attributes
      //
      Entrance<T1,
	      T2>	entrance;

      T1&		o1;
      T2&		o2;
    };

    ///
    /// three-parameter closure.
    ///
    template <typename T1,
	      typename T2,
	      typename T3>
    class Closure<T1, T2, T3>:
      public Entity
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Entrance<T1, T2, T3>&				entrance,
	      T1&						o1,
	      T2&						o2,
	      T3&						o3):
	entrance(entrance),
	o1(o1),
	o2(o2),
	o3(o3)
      {
      }

      //
      // methods
      //
      Void		Trigger()
      {
	this->entrance.Trigger(this->o1,
			       this->o2,
			       this->o3);
      }

      //
      // attributes
      //
      Entrance<T1,
	      T2,
	      T3>	entrance;

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
    class Closure<T1, T2, T3, T4>:
      public Entity
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Entrance<T1, T2, T3, T4>&				entrance,
	      T1&						o1,
	      T2&						o2,
	      T3&						o3,
	      T4&						o4):
	entrance(entrance),
	o1(o1),
	o2(o2),
	o3(o3),
	o4(o4)
      {
      }

      //
      // methods
      //
      Void		Trigger()
      {
	this->entrance.Trigger(this->o1,
			       this->o2,
			       this->o3,
			       this->o4);
      }

      //
      // attributes
      //
      Entrance<T1,
	      T2,
	      T3,
	      T4>	entrance;

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
    class Closure<T1, T2, T3, T4, T5>:
      public Entity
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Entrance<T1, T2, T3, T4, T5>&			entrance,
	      T1&						o1,
	      T2&						o2,
	      T3&						o3,
	      T4&						o4,
	      T5&						o5):
	entrance(entrance),
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
      Void		Trigger()
      {
	this->entrance.Trigger(this->o1,
			       this->o2,
			       this->o3,
			       this->o4,
			       this->o5);
      }

      //
      // attributes
      //
      Entrance<T1,
	      T2,
	      T3,
	      T4,
	      T5>	entrance;

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
    class Closure<T1, T2, T3, T4, T5, T6>:
      public Entity
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Entrance<T1, T2, T3, T4, T5, T6>&			entrance,
	      T1&						o1,
	      T2&						o2,
	      T3&						o3,
	      T4&						o4,
	      T5&						o5,
	      T6&						o6):
	entrance(entrance),
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
      Void		Trigger()
      {
	this->entrance.Trigger(this->o1,
			       this->o2,
			       this->o3,
			       this->o4,
			       this->o5,
			       this->o6);
      }

      //
      // attributes
      //
      Entrance<T1,
	      T2,
	      T3,
	      T4,
	      T5,
	      T6>	entrance;

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
    class Closure<T1, T2, T3, T4, T5, T6, T7>:
      public Entity
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Entrance<T1, T2, T3, T4, T5, T6, T7>&		entrance,
	      T1&						o1,
	      T2&						o2,
	      T3&						o3,
	      T4&						o4,
	      T5&						o5,
	      T6&						o6,
	      T7&						o7):
	entrance(entrance),
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
      Void		Trigger()
      {
	this->entrance.Trigger(this->o1,
			       this->o2,
			       this->o3,
			       this->o4,
			       this->o5,
			       this->o6,
			       this->o7);
      }

      //
      // attributes
      //
      Entrance<T1,
	      T2,
	      T3,
	      T4,
	      T5,
	      T6,
	      T7>	entrance;

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
    class Closure<T1, T2, T3, T4, T5, T6, T7, T8>:
      public Entity
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Entrance<T1, T2, T3, T4, T5, T6, T7, T8>&		entrance,
	      T1&						o1,
	      T2&						o2,
	      T3&						o3,
	      T4&						o4,
	      T5&						o5,
	      T6&						o6,
	      T7&						o7,
	      T8&						o8):
	entrance(entrance),
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
      Void		Trigger()
      {
	this->entrance.Trigger(this->o1,
			       this->o2,
			       this->o3,
			       this->o4,
			       this->o5,
			       this->o6,
			       this->o7,
			       this->o8);
      }

      //
      // attributes
      //
      Entrance<T1,
	      T2,
	      T3,
	      T4,
	      T5,
	      T6,
	      T7,
	      T8>	entrance;

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
    class Closure<T1, T2, T3, T4, T5, T6, T7, T8, T9>:
      public Entity
    {
    public:
      //
      // constructors & destructors
      //
      Closure(Entrance<T1, T2, T3, T4, T5, T6, T7, T8, T9>&	entrance,
	      T1&						o1,
	      T2&						o2,
	      T3&						o3,
	      T4&						o4,
	      T5&						o5,
	      T6&						o6,
	      T7&						o7,
	      T8&						o8,
	      T9&						o9):
	entrance(entrance),
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
      Void		Trigger()
      {
	this->entrance.Trigger(this->o1,
			       this->o2,
			       this->o3,
			       this->o4,
			       this->o5,
			       this->o6,
			       this->o7,
			       this->o8,
			       this->o9);
      }

      //
      // attributes
      //
      Entrance<T1,
	      T2,
	      T3,
	      T4,
	      T5,
	      T6,
	      T7,
	      T8,
	      T9>&	entrance;

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
