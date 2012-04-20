#ifndef ELLE_TEST_NETWORK_UDP_TABLE_HH
#define ELLE_TEST_NETWORK_UDP_TABLE_HH

#include <elle/types.hh>
#include <elle/radix/Object.hh>
#include <elle/network/Locus.hh>
#include <elle/concurrency/Timer.hh>

#include <elle/idiom/Close.hh>
# include <list>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace test
  {

    using namespace elle::network;

    class Node;
    class Neighbour;

    class Table:
      public elle::radix::Object
    {
    public:
      //
      // types
      //
      typedef std::list<Neighbour*>             Container;
      typedef Container::iterator               Iterator;
      typedef Container::const_iterator         Scoutor;

      //
      // methods
      //
      Status            Create(Node*);

      Status            Update(const Locus&,
                               const String&);
      Status            Refresh(const Locus&);
      Status            Remove(const Locus&);
      Status            Locate(const Locus&,
                               Iterator&);

      Status            Merge(const Table&);

      //
      // interfaces
      //

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      // archivable
      //Status            Serialize(Archive&) const;
      //Status            Extract(Archive&);

      //
      // attributes
      //
      Node*             node;

      Timer             timer;

      Container         container;

      //
      // callbacks
      //
      Status            Renew();
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/udp/Node.hh>
#include <elle/test/network/udp/Neighbour.hh>

#endif
