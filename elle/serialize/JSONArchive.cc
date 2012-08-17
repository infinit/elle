# include <ostream>

# include <elle/format/json/Object.hh>
# include <elle/format/json/Bool.hh>
# include <elle/format/json/Integer.hh>
# include <elle/format/json/Float.hh>
# include <elle/format/json/String.hh>
# include <elle/format/json/Array.hh>
# include <elle/format/json/Dictionary.hh>
# include <elle/format/json/Null.hh>
# include <elle/format/json/Dictionary.hxx>
# include <elle/format/json/Array.hxx>
# include <elle/format/json/Parser.hh>

# include <elle/serialize/JSONArchive.hh>

namespace elle
{
  namespace serialize
  {

    ArchiveMode const OutputJSONArchive::_DictStream::mode = ArchiveMode::Output;
    ArchiveMode const InputJSONArchive::_DictStream::mode = ArchiveMode::Input;

    OutputJSONArchive::OutputJSONArchive(StreamType& stream, json::Object&& obj)
      : BaseClass(stream)
    {
      obj.repr(this->stream());
    }

    OutputJSONArchive::OutputJSONArchive(StreamType& stream, json::Object const& obj)
      : BaseClass(stream)
    {
      obj.repr(this->stream());
    }

    void OutputJSONArchive::Save(std::string const& val)
    {
      elle::format::json::String(val).repr(this->stream());
    }

  }
}
