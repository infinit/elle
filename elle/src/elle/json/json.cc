#include <json_spirit/reader.h>
#include <json_spirit/value.h>
#include <json_spirit/writer.h>

#include <elle/Backtrace.hh>
#include <elle/IOStream.hh>
#include <elle/json/exceptions.hh>
#include <elle/json/json.hh>
#include <elle/printf.hh>

ELLE_LOG_COMPONENT("elle.json");

namespace elle
{
  namespace json
  {
    typedef json_spirit::Config_map<std::string> Config;

    static
    boost::any
    from_spirit(json_spirit::Value const& value)
    {
      switch (value.type())
      {
        case json_spirit::Value::OBJECT_TYPE:
        {
          Object res;
          for (auto const& element: value.getObject())
          {
            auto key = Config::get_name(element);
            auto value = from_spirit(Config::get_value(element));
            res.insert(std::make_pair(key, value));
          }
          return res;
        }
        case json_spirit::Value::ARRAY_TYPE:
        {
          Array res;
          for (auto const& element: value.getArray())
          {
            res.push_back(from_spirit(element));
          }
          return res;
        }
        case json_spirit::Value::STRING_TYPE:
          return value.getString();
        case json_spirit::Value::BOOL_TYPE:
          return value.getBool();
        case json_spirit::Value::INT_TYPE:
          return value.getInt64();
        case json_spirit::Value::REAL_TYPE:
          return value.getReal();
        case json_spirit::Value::NULL_TYPE:
          return NullType();

        default:
          ELLE_ABORT("unknown JSON value type");
      }
    }

    static
    json_spirit::Value
    to_spirit(boost::any const& any)
    {
      ELLE_DUMP("to_spirit of type: %s", any.type().name());
      if (any.type() == typeid(OrderedObject))
      {
        Config::Object_type res;
        for (auto const& element: boost::any_cast<OrderedObject>(any))
        {
          auto key = element.first;
          auto value = to_spirit(element.second);
          res.insert(Config::Pair_type(key, value));
        }
        return res;
      }
      if (any.type() == typeid(Object))
      {
        Config::Object_type res;
        for (auto const& element: boost::any_cast<Object>(any))
        {
          auto key = element.first;
          auto value = to_spirit(element.second);
          res.insert(Config::Pair_type(key, value));
        }
        return res;
      }
      if (any.type() == typeid(Array))
      {
        Config::Array_type res;
        for (auto const& element: boost::any_cast<Array>(any))
        {
          res.push_back(to_spirit(element));
        }
        return res;
      }
      if (any.type() == typeid(std::string))
        return boost::any_cast<std::string>(any);
      if (any.type() == typeid(bool))
        return boost::any_cast<bool>(any);
      if (any.type() == typeid(int16_t))
        return boost::any_cast<int16_t>(any);
      if (any.type() == typeid(int32_t))
        return boost::any_cast<int32_t>(any);
      if (any.type() == typeid(int64_t))
        return boost::any_cast<int64_t>(any);
      if (any.type() == typeid(uint16_t))
        return boost::any_cast<uint16_t>(any);
      if (any.type() == typeid(uint32_t))
        return boost::any_cast<uint32_t>(any);
      if (any.type() == typeid(uint64_t))
        return boost::any_cast<uint64_t>(any);
      if (any.type() == typeid(float))
        return boost::any_cast<float>(any);
      if (any.type() == typeid(double))
        return boost::any_cast<double>(any);
      if (any.type() == typeid(NullType))
        return json_spirit::Value::NULL_TYPE;

      ELLE_ABORT("unable to make JSON from type: %s",
                 elle::demangle(any.type().name()));
    }

    boost::any
    read(std::istream& stream)
    {
      ELLE_TRACE_SCOPE("read json from stream");
      elle::IOStreamClear clearer(stream);
      json_spirit::Value value;
      if (!json_spirit::read(stream, value))
        throw ParserError(elle::sprintf("JSON error"));
      auto res = from_spirit(value);
      return res;
    }

    void
    write(std::ostream& stream,
          boost::any const& any)
    {
      ELLE_TRACE_SCOPE("write json to stream");
      auto spirit = to_spirit(any);
      elle::IOStreamClear clearer(stream);
      json_spirit::write(spirit, stream, json_spirit::raw_utf8);
      stream << "\n";
      stream.flush();
    }

    std::string
    pretty_print(boost::any const& any)
    {
      auto spirit = to_spirit(any);
      return json_spirit::write_formatted(spirit);
    }
  }
}
