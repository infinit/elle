//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [tue aug  4 13:28:39 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Data.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method updates a segment of the data object.
    ///
    elle::Status        Data::Write(const Offset&               offset,
                                    const elle::Region&         region)
    {
      // expand if necessary.
      if ((offset + region.size) > this->region.capacity)
        {
          // adjust the region capacity.
          if (this->region.Adjust(offset + region.size) == elle::StatusError)
            escape("unable to expand the region");
        }

      // write the data.
      if (this->region.Write(offset,
                             region.contents,
                             region.size) == elle::StatusError)
        escape("unable to write the data");

      // set the data as dirty.
      this->state = proton::StateDirty;

      return elle::StatusOk;
    }

    ///
    /// this method returns a region of the data object.
    ///
    elle::Status        Data::Read(const Offset&                offset,
                                   const Size&                  size,
                                   elle::Region&                region) const
    {
      Size              length;

      // check the operation's validity: note that since such out-of-bound
      // calls are frequent, no warning or error is raised. instead, no data
      // is simply returned.
      if (offset > this->region.size)
        return elle::StatusOk;

      // set size to the maximum between the request size and the available
      // size.
      length = (this->region.size - offset) < size ?
        (this->region.size - offset) :
        size;

      // prepare the output region.
      if (region.Prepare(length) == elle::StatusError)
        escape("unable to prepare the output region");

      // set the current size.
      region.size = length;

      // read the data from the region.
      if (this->region.Read(offset,
                            region.contents,
                            region.size) == elle::StatusError)
        escape("unable to read the data");

      return elle::StatusOk;
    }

    ///
    /// this method adjust the size of the data either by expanding or
    /// shrinking the region.
    ///
    elle::Status        Data::Adjust(const Size&                size)
    {
      // first, make sure the region's capacity is large enough.
      if (this->region.Adjust(size) == elle::StatusError)
        escape("unable to adjust the region size");

      // then, manually set the region size.
      this->region.size = size;

      // set the data as dirty.
      this->state = proton::StateDirty;

      return elle::StatusOk;
    }

    ///
    /// this method returns the size of the data.
    ///
    elle::Status        Data::Capacity(Size&                    size) const
    {
      // set the size.
      size = this->region.size;

      return elle::StatusOk;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Data, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    elle::Status        Data::Dump(elle::Natural32              margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Data]" << std::endl;

      // dump the parent class.
      if (proton::ContentHashBlock::Dump(margin + 2) == elle::StatusError)
        escape("unable to dump the underlying block");

      // dump the region attribute.
      if (this->region.Dump(margin + 2) == elle::StatusError)
        escape("unable to dump the region");

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    elle::Status        Data::Serialize(elle::Archive&          archive) const
    {
      // call the parent class.
      if (proton::ContentHashBlock::Serialize(archive) == elle::StatusError)
        escape("unable to serialize the underlying physical block");

      // serialize the internal region.
      if (archive.Serialize(this->region) == elle::StatusError)
        escape("unable to serialize the internal region");

      return elle::StatusOk;
    }

    ///
    /// this method extracts the block object.
    ///
    elle::Status        Data::Extract(elle::Archive&            archive)
    {
      // call the parent class.
      if (proton::ContentHashBlock::Extract(archive) == elle::StatusError)
        escape("unable to extract the underyling physical block");

      // extract the region.
      if (archive.Extract(this->region) == elle::StatusError)
        escape("unable to extract the region");

      return elle::StatusOk;
    }

  }
}
