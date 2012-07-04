#ifndef HOLE_IMPLEMENTATIONS_SLUG_GUESTLIST_HH
# define HOLE_IMPLEMENTATIONS_SLUG_GUESTLIST_HH

# include <elle/types.hh>
# include <elle/radix/Entity.hh>
# include <elle/network/TCPSocket.hh>

# include <hole/implementations/slug/Host.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      class Guestlist:
        public elle::radix::Entity
      {
      public:
        //
        // types
        //
        typedef std::pair<elle::network::TCPSocket*, Host*>      Value;
        typedef std::map<elle::network::TCPSocket*, Host*>       Container;
        typedef typename Container::iterator            Iterator;
        typedef typename Container::const_iterator      Scoutor;

        //
        // constructors & destructors
        //
        ~Guestlist();

        //
        // methods
        //
        elle::Status            Add(elle::network::TCPSocket*,
                                    Host*);
        elle::Status            Exist(elle::network::TCPSocket*) const;
        elle::Status            Retrieve(elle::network::TCPSocket*,
                                         Host*&) const;
        elle::Status            Remove(elle::network::TCPSocket*);
        elle::Status            Locate(elle::network::TCPSocket*,
                                       Scoutor&) const;
        elle::Status            Locate(elle::network::TCPSocket*,
                                       Iterator&);

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        //
        // attributes
        //
        Container               container;
      };

    }
  }
}

#endif
