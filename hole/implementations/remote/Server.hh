//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed may 25 19:20:52 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_REMOTE_SERVER_HH
#define HOLE_IMPLEMENTATIONS_REMOTE_SERVER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>

#include <hole/implementations/remote/Customer.hh>

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
      /// this class represents the server which waits for clients to
      /// connect which are then authenticated.
      ///
      /// note that multiple clients can connect to the server in
      /// parallel.
      ///
      class Server:
        public elle::Entity
      {
      public:
        //
        // types
        //
        typedef std::map<elle::TCPSocket*, Customer*>   Container;
        typedef typename Container::iterator            Iterator;
        typedef typename Container::const_iterator      Scoutor;

        //
        // constructors & destructors
        //
        Server(const elle::network::Locus&);
        ~Server();

        //
        // methods
        //
        elle::Status            Launch();

        elle::Status            Add(elle::TCPSocket*,
                                    Customer*);
        elle::Status            Remove(elle::TCPSocket*);
        elle::Status            Retrieve(elle::TCPSocket*,
                                         Customer*&);
        elle::Status            Locate(elle::TCPSocket*,
                                       Iterator* = NULL);

        elle::Status            Put(const nucleus::Address&,
                                    const nucleus::ImmutableBlock&);
        elle::Status            Put(const nucleus::Address&,
                                    const nucleus::MutableBlock&);
        elle::Status            Get(const nucleus::Address&,
                                    nucleus::ImmutableBlock&);
        elle::Status            Get(const nucleus::Address&,
                                    const nucleus::Version&,
                                    nucleus::MutableBlock&);
        elle::Status            Kill(const nucleus::Address&);

        //
        // callbacks
        //
        elle::Status            Connection(elle::TCPSocket*);

        elle::Status            Challenge(const lune::Passport&);

        elle::Status            Sweep(Customer*);

        elle::Status            Push(const nucleus::Address&,
                                     const
                                       nucleus::Derivable<nucleus::Block>&);
        elle::Status            Pull(const nucleus::Address&,
                                     const nucleus::Version&);
        elle::Status            Wipe(const nucleus::Address&);

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        //
        // attributes
        //
        elle::network::Locus             locus;

        Container               container;
      };

    }
  }
}

#endif
