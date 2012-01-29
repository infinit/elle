//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun feb 28 09:00:00 2010]
//

#ifndef ELLE_STANDALONE_MAID_HH
#define ELLE_STANDALONE_MAID_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/Void.hh>

#include <elle/idiom/Close.hh>
# include <cstdarg>
# include <iostream>
# include <sstream>
# include <stdlib.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;

  namespace standalone
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a subsystem of the elle library targeting the
    /// ancient problem of memory deallocation, more specifically when
    /// errors occur.
    ///
    /// the following example illustrates the problem:
    ///
    /// void            foo(B*                  b)
    /// {
    ///   A*            a;
    ///
    ///   a = new A(42);
    ///
    ///   if (bar(a) == -1)
    ///     return (-1);
    ///
    ///   b = new B(a);
    ///
    ///   return (0);
    /// }
    ///
    /// in this example, should the call to bar() fail, the instance 'a'
    /// would be lost, leading to memory leak. the safe way of programming
    /// is to release every object allocated before the error occurs.
    ///
    /// unfortunately, few are the developers dealing with the burden
    /// especially since this problem rarely occurs, only when errors
    /// are detected which are, by definition, not part of the normal
    /// process.
    ///
    /// exceptions cannot solve this problem either because objects
    /// may have been allocated in the calling function/method such
    /// that throwing an exception containing the set of pointers
    /// to the objects to release would go back up through all the
    /// function scopes until it is catched, leading to the release
    /// of those objects. unfortunately, many objects will have been
    /// forgotten. this only way to do it properly with exception would
    /// be to catch the exception in every scope, release the objects
    /// of the called function and forward the exception such that the
    /// source code will look like this:
    ///
    /// void            foo()
    /// {
    ///   [...]
    ///
    ///   try
    ///   {
    ///     bar();
    ///   }
    ///   catch (Exception e)
    ///   {
    ///     e.Release();
    ///   }
    ///
    ///   [...]
    /// }
    ///
    /// void            bar()
    /// {
    ///   [...]
    ///
    ///   try
    ///   {
    ///     baz();
    ///   }
    ///   catch (Exception e)
    ///   {
    ///     e.Release();
    ///   }
    ///
    ///   [...]
    /// }
    ///
    /// void            baz()
    /// {
    ///   [...]
    ///
    ///   if (sometest is wrong)
    ///   {
    ///     throw Memory(a, b, c);
    ///   }
    ///
    ///   [...]
    /// }
    ///
    /// since this way of using exceptions is not appropriate,
    /// a specific system for releasing automatically memory
    /// has been designed.
    ///
    /// this system consists in defining the pointers to track
    /// so that when leaving the scope, if those pointers are
    /// different from NULL, the cleaning method is called on
    /// behalf of the user.
    ///
    /// note that this system can be used to generally release
    /// memory for the developer though programmers are strongly
    /// encouraged to release the memory manually in the normal
    /// case scenario, mainly for reading purposes.
    ///
    /// the system has been designed with intrusion in mind
    /// such that the developer only has to specify the
    /// variables to track in the enter() macro-function. then,
    /// whenever the function leaves the scope through a leave(),
    /// escape() etc. the pointers are released if necessary.
    ///
    /// note that since pointers are considered as requiring
    /// deallocation when there are different from NULL, the
    /// developer must inform the system to stop tracking a
    /// variable if it is manually released or integrated in
    /// another variable that will release it automatically.
    /// The waive() macro-function takes a pointer to stop
    /// tracking.
    ///
    /// two types of variables can be used with this system:
    /// instances of classes which will be released via
    /// the delete statement and non-objects which such as
    /// ::RSA* structures etc. that are released through
    /// the call to a specific function such as ::RSA_free().
    ///
    /// the following illustrates how to use the system:
    ///
    /// void            foo()
    /// {
    ///   ::RSA*        rsa;
    ///   PublicKey*    pk;
    ///
    ///   enterx(instance(pk),
    ///         slab(rsa, ::RSA_free));
    ///
    ///   [...]
    ///
    ///   leave();
    /// }
    ///
    /// noteworthy is that non-pointer variables can also
    /// be used. indeed, sometimes a structure is passed
    /// to functions that may allocate/deallocate data
    /// without the developer knowing. the following
    /// illustrates how to use Maid for this purpose.
    ///
    /// note that the developer must, in this case, specify
    /// the system when to start and stop tracking the
    /// variable through track() and untrack().
    ///
    /// void            foo()
    /// {
    ///   ::EVP_MD_CTX  context;
    ///
    ///   wrap(context);
    ///   enterx(local(context, ::EVP_MD_CTX_cleanup));
    ///
    ///   ::EVP_MD_CTX_init(&context);
    ///
    ///   track(context);
    ///
    ///   [...]
    ///
    ///   ::EVP_MD_CTX_cleanup(&context);
    ///
    ///   untrack(context);
    ///
    ///   leave();
    /// }
    ///
    class Maid
    {
    public:
      //
      // enumerations
      //
      enum Type
        {
          TypeDelete,
          TypeRoutine,
        };

      //
      // classes
      //

      ///
      /// this class represents an object to be released.
      ///
      /// the new and delete operators are overloaded because instances
      /// of this classe are always allocated on the function/method's
      /// stack via the alloca() builtin. therefore, both the new
      /// and delete operators do nothing.
      ///
      class Guard
      {
      public:
        //
        // types
        //

        //
        // constructors & destructors
        //
        virtual ~Guard()
        {
        }

        //
        // operators
        //
        void*                   operator new(size_t,
                                             void*              memory)
        {
          // return the same pointer.
          return (memory);
        }

        void                    operator delete(void*)
        {
          // do not release any memory as this will be done automatically.
        }
      };

      ///
      /// this class stores a set of guards.
      ///
      /// the new and delete operators are overloaded because instances
      /// of this classe are always allocated on the function/method's
      /// stack via the alloca() builtin. therefore, both the new
      /// and delete operators do nothing.
      ///
      class Garrison
      {
      public:
        //
        // constant
        //
        static const Natural32          Capacity = 9;

        //
        // attributes
        //
        Guard*                  guards[Capacity];
        Natural32               size;

        //
        // constructors & destructors
        //
        Garrison(Natural32,
                 ...);
        ~Garrison();

        //
        // operators
        //
        void*                   operator new(size_t,
                                             void*              memory)
        {
          // return the same pointer.
          return (memory);
        }

        void                    operator delete(void*)
        {
          // do not release any memory as this will be done automatically.
        }
      };

      ///
      /// this class keeps track of a class instance i.e objects that
      /// must be released through the use of the delete keyword.
      ///
      template <typename T>
      class Instance:
        public Guard
      {
      public:
        //
        // constructors & destructors
        //
        Instance(T&                                             pointer):
          pointer(pointer)
        {
        }

        ~Instance()
        {
          if (this->pointer != NULL)
            {
              delete this->pointer;

              this->pointer = NULL;
            }
        }

        //
        // attributes
        //
        T&              pointer;
      };

      ///
      /// this class represents an object allocated through a
      /// specific function such as malloc/free or RSA_new/RSA_free
      /// for instance but also BN_init which actually does not allocate
      /// anything but simply initialises.
      ///
      template <typename T, typename F>
      class Slab:
        public Guard
      {
      public:
        //
        // constructors & destructors
        //
        Slab(T&                                                 pointer,
             F                                                  function):
          pointer(pointer),
          function(function)
        {
        }

        ~Slab()
        {
          if (this->pointer != NULL)
            {
              (Void)this->function(this->pointer);

              this->pointer = NULL;
            }
        }

        //
        // attributes
        //
        T&              pointer;
        F               function;
      };

      //
      // static methods
      //
      template <typename... T>
      static Garrison*          Install(Void*,
                                        T...);
      template <typename... T>
      static Garrison*          Install(Void*);

      template <typename T>
      static Guard*             Monitor(Void*, T&);
      template <typename T, typename F>
      static Guard*             Monitor(Void*, T&, F);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/standalone/Maid.hxx>

#endif
