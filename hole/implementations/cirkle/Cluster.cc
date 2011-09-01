//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/cirkle/Cluster.cc
//
// created       julien quintard   [fri aug 26 13:57:10 2011]
// updated       julien quintard   [thu sep  1 00:24:11 2011]
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
      elle::Status	Cluster::Create(const RoutingTable&	routingtable)
      {
	RoutingTable::Scoutor	scoutor;

	enter();

	// go through the entries.
	for (scoutor = routingtable.container.begin();
	     scoutor != routingtable.container.end();
	     scoutor++)
	  {
	    Neighbour*		neighbour = scoutor->second;
	    elle::Point		point;

	    // create a point with the port on which the peer is listening
	    // for incoming connections.
	    if (point.Create(neighbour->point.host,
			     neighbour->port) == elle::StatusError)
	      escape("unable to create the point");

	    // add the neighbour's point.
	    this->container.push_back(point);
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
      elle::Status	Cluster::Serialize(elle::Archive&	archive) const
      {
	Cluster::Scoutor	scoutor;
	elle::Natural32		size;

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
	    elle::Point		point = *scoutor;

	    // serialize the point.
	    if (archive.Serialize(point) == elle::StatusError)
	      escape("unable to serialize the point");
	  }

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	Cluster::Extract(elle::Archive&		archive)
      {
	elle::Natural32	size;
	elle::Natural32	i;

	enter();

	// extract the number of entries.
	if (archive.Extract(size) == elle::StatusError)
	  escape("unable to extract the size");

	// go through the entries.
	for (i = 0; i < size; i++)
	  {
	    elle::Point	point;

	    // extract the point.
	    if (archive.Extract(point) == elle::StatusError)
	      escape("unable to extract the point");

	    // record the point.
	    this->container.push_back(point);
	  }

	leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this function dumps a cluster.
      ///
      elle::Status	Cluster::Dump(elle::Natural32	margin) const
      {
	elle::String		alignment(margin, ' ');
	Cluster::Scoutor	scoutor;

	enter();

	// display the name.
	std::cout << alignment << "[Cluster]" << std::endl;

	// go through the entries.
	for (scoutor = this->container.begin();
	     scoutor != this->container.end();
	     scoutor++)
	  {
	    elle::Point		point = *scoutor;

	    // dump the point.
	    if (point.Dump(margin + 2) == elle::StatusError)
	      escape("unable to dump the point");
	  }

	leave();
      }

    }
  }
}
