#ifndef HOLE_IMPLEMENTATIONS_REMOTE_MACHINE_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_MACHINE_HH

# include <elle/types.hh>
# include <elle/radix/Entity.hh>

# include <hole/implementations/remote/fwd.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

      ///
      /// this class represents the current host.
      ///
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

        //
        // constructors & destructors
        //
        Machine();
        ~Machine();

        //
        // methods
        //
        void Launch();

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
