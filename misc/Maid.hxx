//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Maid.hxx
//
// created       julien quintard   [sun feb 28 09:08:48 2010]
// updated       julien quintard   [mon mar  1 14:49:06 2010]
//

#ifndef ELLE_MISC_MAID_HXX
#define ELLE_MISC_MAID_HXX

namespace elle
{
  namespace misc
  {

//
// ---------- guard -----------------------------------------------------------
//

    ///
    /// the classes below specialise the Guard class with multiple
    /// parameters depending on the number of pointers that must
    /// be tracked.
    ///

    ///
    /// zero-parameter.
    ///
    template <>
    class Maid::Guard<>:
      public Maid::Shell
    {
    };

    ///
    /// single-parameter.
    ///
    template <typename T1>
    class Maid::Guard<T1>:
      public Maid::Shell
    {
    public:
      //
      // attributes
      //
      T1&		object1;

      //
      // constructors & destructors
      //
      Guard(T1&							object1):
	object1(object1)
      {
      }

      ~Guard()
      {
	//
	// delete the objects if necessary.
	//

	if (this->object1 != NULL)
	  delete this->object1;
      }
    };

    ///
    /// two-parameter.
    ///
    template <typename T1,
	      typename T2>
    class Maid::Guard<T1, T2>:
      public Maid::Shell
    {
    public:
      //
      // attributes
      //
      T1&		object1;
      T2&		object2;

      //
      // constructors & destructors
      //
      Guard(T1&							object1,
	    T2&							object2):
	object1(object1),
	object2(object2)
      {
      }

      ~Guard()
      {
	//
	// delete the objects if necessary.
	//

	if (this->object1 != NULL)
	  delete this->object1;

	if (this->object2 != NULL)
	  delete this->object2;
      }
    };

    ///
    /// three-parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3>
    class Maid::Guard<T1, T2, T3>:
      public Maid::Shell
    {
    public:
      //
      // attributes
      //
      T1&		object1;
      T2&		object2;
      T3&		object3;

      //
      // constructors & destructors
      //
      Guard(T1&							object1,
	    T2&							object2,
	    T3&							object3):
	object1(object1),
	object2(object2),
	object3(object3)
      {
      }

      ~Guard()
      {
	//
	// delete the objects if necessary.
	//

	if (this->object1 != NULL)
	  delete this->object1;

	if (this->object2 != NULL)
	  delete this->object2;

	if (this->object3 != NULL)
	  delete this->object3;
      }
    };

    ///
    /// four-parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    class Maid::Guard<T1, T2, T3, T4>:
      public Maid::Shell
    {
    public:
      //
      // attributes
      //
      T1&		object1;
      T2&		object2;
      T3&		object3;
      T4&		object4;

      //
      // constructors & destructors
      //
      Guard(T1&							object1,
	    T2&							object2,
	    T3&							object3,
	    T4&							object4):
	object1(object1),
	object2(object2),
	object3(object3),
	object4(object4)
      {
      }

      ~Guard()
      {
	//
	// delete the objects if necessary.
	//

	if (this->object1 != NULL)
	  delete this->object1;

	if (this->object2 != NULL)
	  delete this->object2;

	if (this->object3 != NULL)
	  delete this->object3;

	if (this->object4 != NULL)
	  delete this->object4;
      }
    };

    ///
    /// five-parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    class Maid::Guard<T1, T2, T3, T4, T5>:
      public Maid::Shell
    {
    public:
      //
      // attributes
      //
      T1&		object1;
      T2&		object2;
      T3&		object3;
      T4&		object4;
      T5&		object5;

      //
      // constructors & destructors
      //
      Guard(T1&							object1,
	    T2&							object2,
	    T3&							object3,
	    T4&							object4,
	    T5&							object5):
	object1(object1),
	object2(object2),
	object3(object3),
	object4(object4),
	object5(object5)
      {
      }

      ~Guard()
      {
	//
	// delete the objects if necessary.
	//

	if (this->object1 != NULL)
	  delete this->object1;

	if (this->object2 != NULL)
	  delete this->object2;

	if (this->object3 != NULL)
	  delete this->object3;

	if (this->object4 != NULL)
	  delete this->object4;

	if (this->object5 != NULL)
	  delete this->object5;
      }
    };

    ///
    /// six-parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    class Maid::Guard<T1, T2, T3, T4, T5, T6>:
      public Maid::Shell
    {
    public:
      //
      // attributes
      //
      T1&		object1;
      T2&		object2;
      T3&		object3;
      T4&		object4;
      T5&		object5;
      T6&		object6;

      //
      // constructors & destructors
      //
      Guard(T1&							object1,
	    T2&							object2,
	    T3&							object3,
	    T4&							object4,
	    T5&							object5,
	    T6&							object6):
	object1(object1),
	object2(object2),
	object3(object3),
	object4(object4),
	object5(object5),
	object6(object6)
      {
      }

      ~Guard()
      {
	//
	// delete the objects if necessary.
	//

	if (this->object1 != NULL)
	  delete this->object1;

	if (this->object2 != NULL)
	  delete this->object2;

	if (this->object3 != NULL)
	  delete this->object3;

	if (this->object4 != NULL)
	  delete this->object4;

	if (this->object5 != NULL)
	  delete this->object5;

	if (this->object6 != NULL)
	  delete this->object6;
      }
    };

    ///
    /// seven-parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    class Maid::Guard<T1, T2, T3, T4, T5, T6, T7>:
      public Maid::Shell
    {
    public:
      //
      // attributes
      //
      T1&		object1;
      T2&		object2;
      T3&		object3;
      T4&		object4;
      T5&		object5;
      T6&		object6;
      T7&		object7;

      //
      // constructors & destructors
      //
      Guard(T1&							object1,
	    T2&							object2,
	    T3&							object3,
	    T4&							object4,
	    T5&							object5,
	    T6&							object6,
	    T7&							object7):
	object1(object1),
	object2(object2),
	object3(object3),
	object4(object4),
	object5(object5),
	object6(object6),
	object7(object7)
      {
      }

      ~Guard()
      {
	//
	// delete the objects if necessary.
	//

	if (this->object1 != NULL)
	  delete this->object1;

	if (this->object2 != NULL)
	  delete this->object2;

	if (this->object3 != NULL)
	  delete this->object3;

	if (this->object4 != NULL)
	  delete this->object4;

	if (this->object5 != NULL)
	  delete this->object5;

	if (this->object6 != NULL)
	  delete this->object6;

	if (this->object7 != NULL)
	  delete this->object7;
      }
    };

    ///
    /// eight-parameter.
    ///
    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    class Maid::Guard<T1, T2, T3, T4, T5, T6, T7, T8>:
      public Maid::Shell
    {
    public:
      //
      // attributes
      //
      T1&		object1;
      T2&		object2;
      T3&		object3;
      T4&		object4;
      T5&		object5;
      T6&		object6;
      T7&		object7;
      T8&		object8;

      //
      // constructors & destructors
      //
      Guard(T1&							object1,
	    T2&							object2,
	    T3&							object3,
	    T4&							object4,
	    T5&							object5,
	    T6&							object6,
	    T7&							object7,
	    T8&							object8):
	object1(object1),
	object2(object2),
	object3(object3),
	object4(object4),
	object5(object5),
	object6(object6),
	object7(object7),
	object8(object8)
      {
      }

      ~Guard()
      {
	//
	// delete the objects if necessary.
	//

	if (this->object1 != NULL)
	  delete this->object1;

	if (this->object2 != NULL)
	  delete this->object2;

	if (this->object3 != NULL)
	  delete this->object3;

	if (this->object4 != NULL)
	  delete this->object4;

	if (this->object5 != NULL)
	  delete this->object5;

	if (this->object6 != NULL)
	  delete this->object6;

	if (this->object7 != NULL)
	  delete this->object7;

	if (this->object8 != NULL)
	  delete this->object8;
      }
    };

    ///
    /// nine-parameter.
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
    class Maid::Guard<T1, T2, T3, T4, T5, T6, T7, T8, T9>:
      public Maid::Shell
    {
    public:
      //
      // attributes
      //
      T1&		object1;
      T2&		object2;
      T3&		object3;
      T4&		object4;
      T5&		object5;
      T6&		object6;
      T7&		object7;
      T8&		object8;
      T9&		object9;

      //
      // constructors & destructors
      //
      Guard(T1&							object1,
	    T2&							object2,
	    T3&							object3,
	    T4&							object4,
	    T5&							object5,
	    T6&							object6,
	    T7&							object7,
	    T8&							object8,
	    T9&							object9):
	object1(object1),
	object2(object2),
	object3(object3),
	object4(object4),
	object5(object5),
	object6(object6),
	object7(object7),
	object8(object8),
	object9(object9)
      {
      }

      ~Guard()
      {
	//
	// delete the objects if necessary.
	//

	if (this->object1 != NULL)
	  delete this->object1;

	if (this->object2 != NULL)
	  delete this->object2;

	if (this->object3 != NULL)
	  delete this->object3;

	if (this->object4 != NULL)
	  delete this->object4;

	if (this->object5 != NULL)
	  delete this->object5;

	if (this->object6 != NULL)
	  delete this->object6;

	if (this->object7 != NULL)
	  delete this->object7;

	if (this->object8 != NULL)
	  delete this->object8;

	if (this->object9 != NULL)
	  delete this->object9;
      }
    };

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns an object instance that will keep track
    /// of objects allocated through the new operator.
    ///
    template <typename T>
    inline Maid::Keyword<T>	Maid::Monitor(Void*		memory,
					      T&		pointer)
    {
      // first initialize the pointer to null.
      pointer = NULL;

      // then, return an Object to keep track of the given pointer.
      return (new (memory) Maid::Keyword<T>(pointer));
    }

    ///
    /// this method returns an object instance that will keep track
    /// of objects allocated through the use of a speific function
    /// such as RSA_new() or even malloc().
    ///
    template <typename T, typename F>
    inline Maid::Routine<T, F>	Maid::Monitor(Void*		memory,
					      T&		pointer,
					      F			function)
    {
      // first initialize the pointer to null.
      pointer = NULL;

      // then return an Object to keep track of the given pointer.
      return (new (memory) Maid::Routine<T, F>(pointer, function));
    }

    ///
    /// this method takes a pointer to a stack-based memory location along
    /// with pointers to variables to keep track of.
    ///
    /// this method creates a Guard instance by specifying the memory
    /// area to use, the given 'memory' pointer.
    ///
    template <typename... T>
    inline Maid::Shell*		Maid::Install(Void*		memory,
					      T&...		objects)
    {
      // create the Guard instance.
      return (new (memory) Maid::Guard<T...>(objects...));
    }

    ///
    /// this specialization is required to avoid creating a guard
    /// should no objects be provided which is what happens most of
    /// the time.
    ///
    template <typename... T>
    inline Maid::Shell*		Maid::Install(Void*		memory)
    {
      return (NULL);
    }

  }
}

#endif
