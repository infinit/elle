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

        enter();

        // go through the entries.
        for (scoutor = routingtable.container.begin();
             scoutor != routingtable.container.end();
             scoutor++)
          {
            Neighbour*          neighbour = scoutor->second;
            elle::Locus         locus;

            // create a locus with the port on which the peer is listening
            // for incoming connections.
            if (locus.Create(neighbour->locus.host,
                             neighbour->port) == elle::StatusError)
              escape("unable to create the locus");

            // add the neighbour's locus.
            this->container.push_back(locus);
          }

        leave();
      }

//
// ---------- object ----------------------------------------------------------
//

      ///
      /// this macro-function call generates the object.
      ///
      embed(Cluster, _());

//
// ---------- archivable ------------------------------------------------------
//


      ///
      /// XXX
      ///
      elle::Status      Cluster::Serialize(elle::Archive&       archive) const
      {
        Cluster::Scoutor        scoutor;
        elle::Natural32         size;

        enter();

        // retrieve the size of the container.
        size = this->container.size();

        // serialize the number of entries.
        if (archive.Serialize(size) == elle::StatusError)
          escape("unable to serialize the size");

        // go through the container.
        for (scoutor = this->container.begin();
             scoutor != this->container.end();
             scoutor++)
          {
            elle::Locus         locus = *scoutor;

            // serialize the locus.
            if (archive.Serialize(locus) == elle::StatusError)
              escape("unable to serialize the locus");
          }

        leave();
      }

      ///
      /// XXX
      ///
      elle::Status      Cluster::Extract(elle::Archive&         archive)
      {
        elle::Natural32 size;
        elle::Natural32 i;

        enter();

        // extract the number of entries.
        if (archive.Extract(size) == elle::StatusError)
          escape("unable to extract the size");

        // go through the entries.
        for (i = 0; i < size; i++)
          {
            elle::Locus locus;

            // extract the locus.
            if (archive.Extract(locus) == elle::StatusError)
              escape("unable to extract the locus");

            // record the locus.
            this->container.push_back(locus);
          }

        leave();
      }

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

        enter();

        // display the name.
        std::cout << alignment << "[Cluster]" << std::endl;

        // go through the entries.
        for (scoutor = this->container.begin();
             scoutor != this->container.end();
             scoutor++)
          {
            elle::Locus         locus = *scoutor;

            // dump the locus.
            if (locus.Dump(margin + 2) == elle::StatusError)
              escape("unable to dump the locus");
          }

        leave();
      }

    }
  }
}
