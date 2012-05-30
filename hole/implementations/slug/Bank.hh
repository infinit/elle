#ifndef HOLE_IMPLEMENTATIONS_SLUG_BANK_HH
# define HOLE_IMPLEMENTATIONS_SLUG_BANK_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

      ///
      /// XXX
      ///
      class Bank:
        public elle::radix::Object
      {
      public:
        //
        // methods
        //
        // XXX

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        //
        // attributes
        //
        // XXX
      };

    }
  }
}

#endif
