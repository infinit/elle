#ifndef HOLE_IMPLEMENTATIONS_SLUG_SYNCHRONISER_HH
# define HOLE_IMPLEMENTATIONS_SLUG_SYNCHRONISER_HH

# include <elle/concurrency/Timer.hh>

# include <elle/types.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

      ///
      /// XXX
      ///
      class Synchroniser:
        public elle::radix::Entity
      {
      public:
        //
        // methods
        //
        elle::Status            Start();

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        //
        // callbacks
        //
        elle::Status            Run();

        //
        // signals
        //
        struct
        {
          elle::concurrency::Signal<
            elle::radix::Parameters<
              >
            >                   synchronised;
        }                       signal;

        //
        // attributes
        //
        elle::concurrency::Timer             timer;
      };

    }
  }
}

#endif
