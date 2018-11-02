#include <elle/Backtrace.hh>
#include <elle/IOStream.hh>
#include <elle/assert.hh>
#include <elle/finally.hh>
#include <elle/json/exceptions.hh>
#include <elle/json/json.hh>
#include <elle/log.hh>
#include <elle/printf.hh>

ELLE_LOG_COMPONENT("elle.json");

namespace elle
{
  namespace json
  {
    Json
    read(std::istream& stream)
    {
      ELLE_TRACE_SCOPE("read json from stream");
      Json res;
      stream >> res;
      ELLE_DUMP("read json: {}", res);
      return res;
    }

    Json
    read(std::string const& json)
    {
      try
      {
        return nlohmann::json::parse(json);
      }
      catch (nlohmann::json::exception const& e)
      {
        elle::err("JSON parse error: {}", e.what());
      }
    }

    // FIXME: xalloc for pretty-printing.
    void
    write(std::ostream& stream,
          Json const& json,
          bool with_endl,
          bool pretty_print)
    {
      ELLE_TRACE_SCOPE("write json to stream");
      elle::IOStreamClear clearer(stream);
      elle::SafeFinally restore(
        [&stream, f = stream.flags()] { stream.flags(f); });
      if (pretty_print)
        stream.width(2);
      else
        stream.width(0);
      stream << json;
      if (with_endl)
        stream << '\n';
      stream.flush(); // FIXME: this is bad, but so risky removing it.
    }

    std::string
    pretty_print(Json const& json)
    {
      std::stringstream stream;
      write(stream, json, false, true);
      return stream.str();
    }
  }
}

namespace nlohmann
{
  namespace detail
  {
    std::ostream&
    operator <<(std::ostream& s, value_t t)
    {
      switch (t)
      {
        case value_t::null:
          s << "null";
          break;
        case value_t::object:
          s << "object";
          break;
        case value_t::array:
          s << "array";
          break;
        case value_t::string:
          s << "string";
          break;
        case value_t::boolean:
          s << "boolean";
          break;
        case value_t::number_integer:
        case value_t::number_unsigned:
          s << "integer";
          break;
        case value_t::number_float:
          s << "float";
          break;
        case value_t::discarded:
          s << "discarded";
          break;
      }
      return s;
    }
  }
}
