#include <hole/implementations/slug/Cluster.hh>
#include <hole/implementations/slug/Neighbourhood.hh>

#include <elle/network/Locus.hh>
#include <elle/utility/Time.hh>

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

        // go through the entries.
        for (scoutor = neighbourhood.container.begin();
             scoutor != neighbourhood.container.end();
             scoutor++)
          {
            Host*               host = scoutor->second;

            // add the host's locus.
            this->container.push_back(host->locus);
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
