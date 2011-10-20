//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed aug 24 11:30:42 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/slug/RoutingTable.hh>

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
      RoutingTable::~RoutingTable()
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
      elle::Status	RoutingTable::Add(const Label&		label,
					  Neighbour*		neighbour)
      {
	std::pair<RoutingTable::Iterator, elle::Boolean>	result;

	enter();

	// insert the neighbour in the container.
	result = this->container.insert(std::pair<const Label,
						  Neighbour*>(label,
							      neighbour));

	// check if the insertion was successful.
	if (result.second == false)
	  escape("unable to insert the neighbour in the container");

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	RoutingTable::Exist(const Label&	label)
      {
	enter();

	// try to locate the label.
	if (this->Locate(label) == elle::StatusTrue)
	  true();

	false();
      }

      ///
      /// XXX
      ///
      elle::Status	RoutingTable::Retrieve(const Label&	label,
					       Neighbour*&	neighbour)
      {
	RoutingTable::Iterator	iterator;

	enter();

	// try to locate the label.
	if (this->Locate(label, &iterator) == elle::StatusFalse)
	  escape("unable to locate the given label");

	// return the associated neighbour.
	neighbour = iterator->second;

	leave();
      }

      ///
      /// XXX
      ///
      /// note that the neighbour objects are not deleted since the
      /// routing table entries reference the neighbourhood.
      ///
      elle::Status	RoutingTable::Remove(const Label&	label)
      {
	RoutingTable::Iterator	iterator;

	enter();

	// try to locate the label.
	if (this->Locate(label, &iterator) == elle::StatusFalse)
	  escape("unable to locate the given label");

	// erase the iterator.
	this->container.erase(iterator);

	leave();
      }

      ///
      /// XXX
      ///
      elle::Status	RoutingTable::Locate(const Label&	label,
					     Iterator*		iterator)
      {
	RoutingTable::Iterator	i;

	enter();

	// try to locate the neighbour.
	if ((i = this->container.find(label)) != this->container.end())
	  {
	    if (iterator != NULL)
	      *iterator = i;

	    true();
	  }

	false();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this function dumps a routing table object.
      ///
      elle::Status	RoutingTable::Dump(elle::Natural32	margin) const
      {
	elle::String		alignment(margin, ' ');
	RoutingTable::Scoutor	scoutor;

	enter();

	// display the name.
	std::cout << alignment << "[RoutingTable]" << std::endl;

	// go through the entries.
	for (scoutor = this->container.begin();
	     scoutor != this->container.end();
	     scoutor++)
	  {
	    // dump the entry.
	    std::cout << alignment << elle::Dumpable::Shift
		      << "[Entry]" << std::endl;

	    // dump the label.
	    if (scoutor->first.Dump(margin + 4) == elle::StatusError)
	      escape("unable to dump the label");

	    // dump the neighbour address.
	    std::cout << alignment
		      << elle::Dumpable::Shift
		      << elle::Dumpable::Shift
		      << "[Neighbour] "
		      << std::hex << scoutor->second << std::endl;
	  }

	leave();
      }

    }
  }
}
