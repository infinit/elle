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
    using Config = json_spirit::Config_map<std::string>;

    namespace
    {
      Json
      from_spirit(json_spirit::Value const& value)
      {
        switch (value.type())
        {
          case json_spirit::Value::OBJECT_TYPE:
          {
            auto res = Object{};
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
            auto res = Array{};
            for (auto const& element: value.getArray())
              res.emplace_back(from_spirit(element));
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

      json_spirit::Value
      to_spirit(Json const& any)
      {
        ELLE_DUMP("to_spirit of type: %s", any.type().name());
        if (any.type() == typeid(OrderedObject))
        {
          auto res = Config::Object_type{};
          for (auto const& element: boost::any_cast<OrderedObject>(any))
          {
            auto key = element.first;
            auto value = to_spirit(element.second);
            res.emplace(key, value);
          }
          return res;
        }
        else if (any.type() == typeid(Object))
        {
          auto res = Config::Object_type{};
          for (auto const& element: boost::any_cast<Object>(any))
          {
            auto key = element.first;
            auto value = to_spirit(element.second);
            res.emplace(key, value);
          }
          return res;
        }
        else if (any.type() == typeid(Array))
        {
          auto res = Config::Array_type{};
          for (auto const& element: boost::any_cast<Array>(any))
            res.emplace_back(to_spirit(element));
          return res;
        }

#ifdef __clang__
# define CL(a) std::string((a).name())
#else
# define CL(a) (a)
#endif

#define CASE(Type)                                      \
        else if (CL(any.type()) == CL(typeid(Type)))    \
          return boost::any_cast<Type>(any)

        CASE(std::string);
        CASE(char const*);
        CASE(bool);
        CASE(int16_t);
        CASE(int32_t);
        CASE(int64_t);
        CASE(uint16_t);
        CASE(uint32_t);
        CASE(uint64_t);
        // On macOS, `uint64_t` is `unsigned long long`, which is not
        // the same type as `unsigned long`.
        CASE(long);
        CASE(unsigned long);
        CASE(long long);
        CASE(unsigned long long);
        CASE(float);
        CASE(double);
        else if (CL(any.type()) == CL(typeid(NullType)))
          return json_spirit::Value();
        else if (CL(any.type()) == CL(typeid(void)))
          return json_spirit::Value();
        else
          ELLE_ABORT("unable to make JSON from type: %s",
                     elle::demangle(any.type().name()));
      }
    }

    Json
    read(std::istream& stream)
    {
      ELLE_TRACE_SCOPE("read json from stream");
      json_spirit::Value value;
      if (!json_spirit::read(stream, value))
        throw ParseError(elle::sprintf("JSON error"));
      return from_spirit(value);
    }

    Json
    read(std::string const& json)
    {
      std::stringstream s(json);
      auto res = read(s);
      {
        std::string word;
        if (s >> word)
          elle::err("garbage at end of JSON value: %s", word);
      }
      return res;
    }

    void
    write(std::ostream& stream,
          Json const& any,
          bool with_endl,
          bool pretty_print)
    {
      ELLE_TRACE_SCOPE("write json to stream");
      auto spirit = to_spirit(any);
      elle::IOStreamClear clearer(stream);
      int options = json_spirit::raw_utf8;
      if (pretty_print)
        options |= json_spirit::pretty_print;
      json_spirit::write(spirit, stream, options);
      if (with_endl)
        stream << '\n';
      stream.flush();
    }

    std::string
    pretty_print(Json const& any)
    {
      std::stringstream stream;
      write(stream, any, false, true);
      return stream.str();
    }

    std::ostream&
    operator <<(std::ostream& stream, Object const& obj)
    {
      write(stream, obj);
      return stream;
    }

    std::ostream&
    operator <<(std::ostream& stream, OrderedObject const& obj)
    {
      write(stream, obj);
      return stream;
    }
  }
}
