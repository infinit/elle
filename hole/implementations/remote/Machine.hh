#ifndef HOLE_IMPLEMENTATIONS_REMOTE_MACHINE_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_MACHINE_HH

# include <elle/network/Locus.hh>
# include <elle/types.hh>
# include <elle/radix/Entity.hh>
# include <elle/network/Locus.hh>

# include <hole/fwd.hh>
# include <hole/implementations/remote/fwd.hh>

# include <elle/Passport.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {
      /// The current host.
      class Machine:
        public elle::radix::Entity
      {
      public:
        //
        // enumerations
        //
        enum Role
          {
            RoleUnknown,
            RoleServer,
            RoleClient
          };

      /*-------------.
      | Construction |
      `-------------*/
      public:
        Machine(Implementation& hole);
        ~Machine();
      private:
        Implementation& _hole;

      public:
        void
        Launch();

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        //
        // attributes
        //
        Role                    role;

        union
        {
          Server*               server;
          Client*               client;
        };
      };

    }
  }
}

#endif
