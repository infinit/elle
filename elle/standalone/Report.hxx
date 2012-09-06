#ifndef ELLE_STANDALONE_REPORT_HXX
#define ELLE_STANDALONE_REPORT_HXX

namespace elle
{
  namespace standalone
  {

//
// ---------- object-like -----------------------------------------------------
//

    ///
    /// this method recycles a report.
    ///
    template <typename T>
    Status              Report::Recycle(const T*                object)
    {
      // release the resources.
      this->~Report();

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

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(elle::standalone::Report::Entry,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & value.location;
  archive & value.time;
  archive & value.message;
}

ELLE_SERIALIZE_SPLIT(elle::standalone::Report)

ELLE_SERIALIZE_SPLIT_SAVE(elle::standalone::Report,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);
  archive << static_cast<elle::Natural32>(value.container.size());
  auto it = value.container.begin(),
       end = value.container.end();
  for(; it != end; ++it)
    {
      auto entry = *it;
      enforce(entry != nullptr);
      archive << *entry;
    }
}

ELLE_SERIALIZE_SPLIT_LOAD(elle::standalone::Report,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  elle::Natural32 size;
  archive >> size;

  for (elle::Natural32 i = 0; i < size; ++i)
    {
      auto entry = archive.template Construct<elle::standalone::Report::Entry>();
      value.container.push_back(entry.get());
      entry.release();
    }
}

#endif
