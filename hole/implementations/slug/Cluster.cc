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

#include <hole/implementations/slug/Cluster.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
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
      elle::Status      Cluster::Create(const Neighbourhood&    neighbourhood)
      {
        Neighbourhood::Scoutor  scoutor;

        ;

        // go through the entries.
        for (scoutor = neighbourhood.container.begin();
             scoutor != neighbourhood.container.end();
             scoutor++)
          {
            Host*               host = scoutor->second;

            // add the host's locus.
            this->container.push_back(host->locus);
          }

        return elle::StatusOk;
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

        ;

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

        return elle::StatusOk;
      }

      ///
      /// XXX
      ///
      elle::Status      Cluster::Extract(elle::Archive&         archive)
      {
        elle::Natural32 size;
        elle::Natural32 i;

        ;

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

        return elle::StatusOk;
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

        ;

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

        return elle::StatusOk;
      }

    }
  }
}
