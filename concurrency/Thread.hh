#ifndef ELLE_CONCURRENCY_THREAD_HH
# define ELLE_CONCURRENCY_THREAD_HH

# include <elle/radix/Entity.hh>

# include <elle/idiom/Close.hh>
#  include <QThread>
# include <elle/idiom/Open.hh>

namespace elle
{
  namespace concurrency
  {

    ///
    /// XXX \todo to implement
    ///
    class Thread:
      public ::QThread,

      public Entity
    {
    public:
      //
      // static methods
      //
      static Thread*    Retrieve();
      template <typename T>
      static Thread*    Retrieve(T*);

      Status            Start();

      //
      // constructors & destructors
      //
      Thread() {};
      ~Thread() {};

      //
      // interface
      //
      virtual Status    Run() = 0;

      // qthread
      void              run();
    };

  }
}

#endif
