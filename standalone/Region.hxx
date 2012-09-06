#ifndef ELLE_STANDALONE_REGION_HXX
#define ELLE_STANDALONE_REGION_HXX

namespace elle
{
  namespace standalone
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method recycles a region.
    ///
    template <typename T>
    Status              Region::Recycle(const T*                object)
    {
      // release the resources.
      this->~Region();

      if (object == nullptr)
        {
          // initialize the object with default values.
          new (this) T;
        }
      else
        {
          // initialize the object with defined values.
          new (this) T(*object);
        }

      return Status::Ok;
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/utility/Buffer.hh>

ELLE_SERIALIZE_SPLIT(elle::standalone::Region)

ELLE_SERIALIZE_SPLIT_SAVE(elle::standalone::Region,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);
  archive << static_cast<uint64_t>(value.size);
  archive.SaveBinary(value.contents, value.size);
}

ELLE_SERIALIZE_SPLIT_LOAD(elle::standalone::Region,
                          archive,
                          value,
                          version)
{
  uint64_t size;
  enforce(version == 0);
  archive >> size;
  if (value.Prepare(size) != elle::Status::Ok)
    throw std::runtime_error("Cannot prepare the region");
  archive.LoadBinary(value.contents, size);
  value.size = size;
}

#endif
