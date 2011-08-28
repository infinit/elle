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
// updated       julien quintard   [sun aug 28 13:05:21 2011]
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
      elle::Status	Cluster::Create(const Neighbourhood&	neighbourhood)
      {
	Neighbourhood::Scoutor	scoutor;

	enter();

	// go through the entries.
	for (scoutor = neighbourhood.container.begin();
	     scoutor != neighbourhood.container.end();
	     scoutor++)
	  {
	    Neighbour*		neighbour = scoutor->second;
	    Cluster::Atom	atom;

	    // ignore non-authenticated neighbours.
	    if (neighbour->state != Neighbour::StateAuthenticated)
	      continue;

	    // ignore null-point neighbours i.e neighbours which have
	    // not provided their listening point.
	    if (neighbour->point == elle::Point::Null)
	      continue;

	    // create the atom.
	    atom.label = neighbour->label;
	    atom.point = neighbour->point;

	    // add the neighbour's point.
	    this->container.push_back(atom);
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
	    Cluster::Atom	atom = *scoutor;

	    // serialize the point.
	    if (archive.Serialize(atom.label,
				  atom.point) == elle::StatusError)
	      escape("unable to serialize the atom");
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
	    Cluster::Atom	atom;

	    // extract the point.
	    if (archive.Extract(atom.label,
				atom.point) == elle::StatusError)
	      escape("unable to extract the atom");

	    // record the point.
	    this->container.push_back(atom);
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
	    Cluster::Atom	atom = *scoutor;

	    // dump the label.
	    if (atom.label.Dump(margin + 2) == elle::StatusError)
	      escape("unable to dump the label");

	    // dump the point.
	    if (atom.point.Dump(margin + 2) == elle::StatusError)
	      escape("unable to dump the point");
	  }

	leave();
      }

    }
  }
}
