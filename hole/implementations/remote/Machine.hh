//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed aug 31 00:52:52 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_REMOTE_MACHINE_HH
#define HOLE_IMPLEMENTATIONS_REMOTE_MACHINE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <hole/implementations/remote/Client.hh>
#include <hole/implementations/remote/Server.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

//
// ---------- classes ---------------------------------------------------------
//

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
        elle::Status            Launch();

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
