//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [fri aug 26 13:57:10 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/cirkle/Cluster.hh>
#include <hole/implementations/cirkle/Neighbour.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Cluster::~Cluster()
      {
        // simply clear the content.
        this->container.clear();
      }

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status      Cluster::Create(const RoutingTable&     routingtable)
      {
        RoutingTable::Scoutor   scoutor;

        // go through the entries.
        for (scoutor = routingtable.container.begin();
             scoutor != routingtable.container.end();
             scoutor++)
          {
            Neighbour*          neighbour = scoutor->second;
            elle::network::Locus         locus;

            // create a locus with the port on which the peer is listening
            // for incoming connections.
            if (locus.Create(neighbour->locus.host,
                             neighbour->port) == elle::Status::Error)
              escape("unable to create the locus");

            // add the neighbour's locus.
            this->container.push_back(locus);
          }

        return elle::Status::Ok;
      }

//
// ---------- object ----------------------------------------------------------
//

      ///
      /// this macro-function call generates the object.
      ///
      embed(Cluster, _());

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this function dumps a cluster.
      ///
      elle::Status      Cluster::Dump(elle::Natural32   margin) const
      {
        elle::String            alignment(margin, ' ');
        Cluster::Scoutor        scoutor;

        // display the name.
        std::cout << alignment << "[Cluster]" << std::endl;

        // go through the entries.
        for (scoutor = this->container.begin();
             scoutor != this->container.end();
             scoutor++)
          {
            elle::network::Locus         locus = *scoutor;

            // dump the locus.
            if (locus.Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the locus");
          }

        return elle::Status::Ok;
      }

    }
  }
}
