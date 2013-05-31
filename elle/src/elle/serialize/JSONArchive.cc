#include "JSONArchive.hh"

#include <elle/format/json/Object.hh>
#include <elle/format/json/Bool.hh>
#include <elle/format/json/Integer.hh>
#include <elle/format/json/Float.hh>
#include <elle/format/json/String.hh>
#include <elle/format/json/Array.hh>
#include <elle/format/json/Dictionary.hh>
#include <elle/format/json/Null.hh>
#include <elle/format/json/Dictionary.hxx>
#include <elle/format/json/Array.hxx>
#include <elle/format/json/Parser.hh>

#include <ostream>
#include <iomanip>

namespace elle
{
  namespace serialize
  {
    template <>
    JSONArchive<ArchiveMode::output>::JSONArchive(StreamType& stream):
      BaseClass(stream)
    {}

    template <>
    JSONArchive<ArchiveMode::input>::JSONArchive(StreamType& stream):
      BaseClass(stream)
    {}

    template <>
    JSONArchive<ArchiveMode::output>::JSONArchive(StreamType& stream,
                                                  json::Object&& obj):
      BaseClass(stream)
    {
      obj.repr(this->stream());
    }

    template <>
    JSONArchive<ArchiveMode::output>::JSONArchive(StreamType& stream,
                                                  json::Object const& obj):
      BaseClass(stream)
    {
      obj.repr(this->stream());
    }

    template <>
    void
    JSONArchive<ArchiveMode::output>::Save(uint16_t val)
    {
      this->stream() << val;
    }

    template <>
    void
    JSONArchive<ArchiveMode::output>::Save(uint32_t val)
    {
      this->stream() << val;
    }

    template <>
    void
    JSONArchive<ArchiveMode::output>::Save(uint64_t val)
    {
      this->stream() << val;
    }

    template <>
    void
    JSONArchive<ArchiveMode::output>::Save(int16_t val)
    {
      this->stream() << val;
    }

    template <>
    void
    JSONArchive<ArchiveMode::output>::Save(int32_t val)
    {
      this->stream() << val;
    }

    template <>
    void
    JSONArchive<ArchiveMode::output>::Save(int64_t val)
    {
      this->stream() << val;
    }

    template <>
    void
    JSONArchive<ArchiveMode::output>::Save(float val)
    {
      this->stream() << std::setprecision(23) << val;
    }

    template <>
    void
    JSONArchive<ArchiveMode::output>::Save(double val)
    {
      this->stream() << std::setprecision(23) << val;
    }

    template <>
    void
    JSONArchive<ArchiveMode::output>::Save(std::string const& val)
    {
      elle::format::json::String(val).repr(this->stream());
    }
  }
}
