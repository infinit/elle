#include <elle/json/utils.hh>

#include <elle/IOStream.hh>
#include <elle/json/exceptions.hh>
#include <elle/printf.hh>

#include <json_spirit/json_spirit_reader.h>
#include <json_spirit/json_spirit_value.h>
#include <json_spirit/json_spirit_writer.h>

ELLE_LOG_COMPONENT("elle.json");

namespace elle
{
  namespace json
  {
    typedef json_spirit::Config_vector<std::string> Config;

    static
    boost::any
    from_spirit(json_spirit::Value const& value)
    {
      switch (value.type())
      {
        case json_spirit::obj_type:
        {
          Object res;
          for (auto const& element: value.get_obj())
          {
            auto key = Config::get_name(element);
            auto value = from_spirit(Config::get_value(element));
            res.insert(std::make_pair(key, value));
          }
          return res;
        }
        case json_spirit::array_type:
        {
          Array res;
          for (auto const& element: value.get_array())
          {
            res.push_back(from_spirit(element));
          }
          return res;
        }
        case json_spirit::str_type:
          return value.get_str();
        case json_spirit::bool_type:
          return value.get_bool();
        case json_spirit::int_type:
          return value.get_int();
        case json_spirit::real_type:
          return value.get_real();
        case json_spirit::null_type:
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
      if (any.type() == typeid(Object))
      {
        Config::Object_type res;
        for (auto const& element: boost::any_cast<Object>(any))
        {
          auto key = element.first;
          auto value = to_spirit(element.second);
          res.push_back(Config::Pair_type(key, value));
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
      if (any.type() == typeid(int))
        return boost::any_cast<int>(any);
      if (any.type() == typeid(float))
        return boost::any_cast<float>(any);
      if (any.type() == typeid(double))
        return boost::any_cast<double>(any);
      if (any.type() == typeid(NullType))
        return json_spirit::null_type;

      ELLE_ABORT("unable to make JSON from type: %s", any.type().name());
    }

    Object
    read_json(std::istream& stream)
    {
      ELLE_DEBUG("read json from stream: %s", stream);
      elle::IOStreamClear clearer(stream);
      json_spirit::Value value;
      if (!json_spirit::read(stream, value))
        throw ParserError(elle::sprintf("JSON error"));
      auto res = from_spirit(value.get_obj());
      if (res.type() != typeid(Object))
        throw ParserError("JSON is not an object");
      return boost::any_cast<Object>(res);
    }

    void
    write_json(std::ostream& stream,
               boost::any const& any)
    {
      ELLE_DEBUG("write json to stream: %s", stream);
      auto spirit = to_spirit(any);
      elle::IOStreamClear clearer(stream);
      json_spirit::write(spirit, stream);
      stream << "\n";
      stream.flush();
    }

    std::string
    pretty_print_json(boost::any const& any)
    {
      auto spirit = to_spirit(any);
      return json_spirit::write_formatted(spirit);
    }
  }
}
