//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Range.cc
//
// created       julien quintard   [wed mar 31 23:36:12 2010]
// updated       julien quintard   [thu apr 15 16:31:44 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Range.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// destructor.
    ///
    Range::~Range()
    {
      Range::Iterator	i;

      // until the range is empty.
      while (this->container.empty() == false)
	{
	  Record*	record = this->container.front();

	  // remove the first element.
	  this->container.pop_front();

	  // delete the record.
	  delete record;
	}
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds an item to the range.
    ///
    Status		Range::Add(Record*			record)
    {
      enter();

      // add the record to the container.
      this->container.push_back(record);

      leave();
    }

    ///
    /// this method returns true if an entry for the given subject exists.
    ///
    Status		Range::Exist(const Subject&		subject)
    {
      enter();

      // try to locate the entry.
      if (this->Locate(subject) != StatusTrue)
	false();

      true();
    }

    ///
    /// this method removes the identified record.
    ///
    /// the method returns true if the record is found, false otherwise.
    ///
    Status		Range::Lookup(const Subject&		subject,
				      Record*&			record)
    {
      Range::Iterator	iterator;

      enter();

      // locate the record.
      if (this->Locate(subject, &iterator) == false)
	false();

      // return the record.
      record = *iterator;

      true();
    }

    ///
    /// this method removes a record from the range.
    ///
    Status		Range::Remove(const Subject&		subject)
    {
      Range::Iterator	iterator;

      enter();

      // locate the record.
      if (this->Locate(subject, &iterator) == false)
	escape("this subject does not seem to be present in this range");

      // delete the record.
      delete *iterator;

      // erase the record.
      this->container.erase(iterator);

      leave();
    }

    ///
    /// this method returns the number of access entries in the range.
    ///
    Status		Range::Capacity(Size&			size) const
    {
      enter();

      // return the size.
      size = this->container.size();

      leave();
    }

    ///
    /// this method returns the index where the record is located in
    /// the range.
    ///
    Status		Range::Locate(const Subject&		subject,
				      Index&			index)
    {
      Range::Iterator	i;

      enter();

      // go through the container.
      for (i = this->container.begin(), index = 0;
	   i != this->container.end();
	   i++, index++)
	{
	  Record*	record = *i;

	  // return true if found.
	  if (record->subject == subject)
	    true();
	}

      false();
    }

    ///
    /// this method returns an iterator on the identified record. this
    /// method can easily be used to test if a subject belong to the range:
    /// Locate(subject) returns true or false.
    ///
    /// the method returns true if the record is found, false otherwise.
    ///
    Status		Range::Locate(const Subject&		subject,
				      Range::Iterator*		iterator)
    {
      Range::Iterator	i;

      enter();

      // go through the container.
      for (i = this->container.begin();
	   i != this->container.end();
	   i++)
	{
	  Record*	record = *i;

	  if (record->subject == subject)
	    {
	      // return the iterator, if wanted.
	      if (iterator != NULL)
		*iterator = i;

	      true();
	    }
	}

      false();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a range.
    ///
    Status		Range::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');
      Range::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Range]" << std::endl;

      // dump every record
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Record*	record = *scoutor;

	  // dump the record.
	  if (record->Dump(margin + 2) == StatusError)
	    escape("unable to dump the record");
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the range object.
    ///
    Status		Range::Serialize(Archive&		archive) const
    {
      Range::Scoutor	scoutor;
      Size		size;

      enter();

      // retrieve the number of records.
      size = this->container.size();

      // serialize the number of records.
      if (archive.Serialize(size) == StatusError)
	escape("unable to serialize the number of records");

      // serialize every record.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Record*	record = *scoutor;

	  // serialize the record.
	  if (archive.Serialize(*record) == StatusError)
	    escape("unable to serialize the record");
	}

      leave();
    }

    ///
    /// this method extracts the range object.
    ///
    Status		Range::Extract(Archive&		archive)
    {
      Size		size;
      Index		i;

      enter();

      // extract the number of records.
      if (archive.Extract(size) == StatusError)
	escape("unable to extract the number of record");

      // extract every record.
      for (i = 0; i < size; i++)
	{
	  Record*	record;

	  enter(instance(record));

	  // allocate a new record.
	  record = new Record;

	  // extract the record.
	  if (archive.Extract(*record) == StatusError)
	    escape("unable to extract the record");

	  // add the record to the container.
	  this->container.push_back(record);

	  // stop tracking the record.
	  waive(record);

	  release();
	}

      leave();
    }

  }
}
