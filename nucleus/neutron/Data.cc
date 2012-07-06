#include <nucleus/neutron/Data.hh>

#include <nucleus/proton/Contents.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Data::Data(proton::Contents<Data>&                          contents):
      contents(contents)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the data.
    ///
    elle::Status        Data::Create()
    {
      this->contents.state = proton::StateDirty;

      return elle::Status::Ok;
    }

    ///
    /// this method updates a segment of the data object.
    ///
    elle::Status        Data::Write(const Offset&               offset,
                                    const elle::standalone::Region& region)
    {
      // expand if necessary.
      if ((offset + region.size) > this->region.capacity)
        {
          // adjust the region capacity.
          if (this->region.Adjust(offset + region.size) == elle::Status::Error)
            escape("unable to expand the region");
        }

      // write the data.
      if (this->region.Write(offset,
                             region.contents,
                             region.size) == elle::Status::Error)
        escape("unable to write the data");

      // set the data as dirty.
      this->contents.state = proton::StateDirty;

      return elle::Status::Ok;
    }

    ///
    /// this method returns a region of the data object.
    ///
    elle::Status        Data::Read(const Offset&                offset,
                                   const Size&                  size,
                                   elle::standalone::Region&    region) const
    {
      Size              length;

      // check the operation's validity: note that since such out-of-bound
      // calls are frequent, no warning or error is raised. instead, no data
      // is simply returned.
      if (offset > this->region.size)
        return elle::Status::Ok;

      // set size to the maximum between the request size and the available
      // size.
      length = (this->region.size - offset) < size ?
        (this->region.size - offset) :
        size;

      // prepare the output region.
      if (region.Prepare(length) == elle::Status::Error)
        escape("unable to prepare the output region");

      // set the current size.
      region.size = length;

      // read the data from the region.
      if (this->region.Read(offset,
                            region.contents,
                            region.size) == elle::Status::Error)
        escape("unable to read the data");

      return elle::Status::Ok;
    }

    ///
    /// this method adjust the size of the data either by expanding or
    /// shrinking the region.
    ///
    elle::Status        Data::Adjust(const Size&                size)
    {
      // first, make sure the region's capacity is large enough.
      if (this->region.Adjust(size) == elle::Status::Error)
        escape("unable to adjust the region size");

      // then, manually set the region size.
      this->region.size = size;

      // set the data as dirty.
      this->contents.state = proton::StateDirty;

      return elle::Status::Ok;
    }

    ///
    /// this method returns the size of the data.
    ///
    elle::Status        Data::Capacity(Size&                    size) const
    {
      // set the size.
      size = this->region.size;

      return elle::Status::Ok;
    }

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

      // dump the region attribute.
      if (this->region.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the region");

      return elle::Status::Ok;
    }

  }
}
