#include <elle/serialization/json/SerializerIn.hh>

#include <limits>

#include <elle/Backtrace.hh>
#include <elle/chrono.hh>
#include <elle/finally.hh>
#include <elle/format/base64.hh>
#include <elle/json/exceptions.hh>
#include <elle/memory.hh>
#include <elle/meta.hh>
#include <elle/printf.hh>
#include <elle/serialization/Error.hh>
#include <elle/serialization/json/Error.hh>
#include <elle/time.hh>

ELLE_LOG_COMPONENT("elle.serialization.json.SerializerIn");

namespace elle
{
  namespace serialization
  {
    namespace json
    {
      /*-------------.
      | Construction |
      `-------------*/

      SerializerIn::SerializerIn(std::istream& input,
                                 bool versioned)
        : SerializerIn(input, Versions(), versioned)
      {}

      SerializerIn::SerializerIn(std::istream& input,
                                 Versions versions,
                                 bool versioned)
        : Super(std::move(versions), versioned)
        , _partial(false)
        , _json([&]
                {
                  try
                  {
                    return elle::json::read(input);
                  }
                  catch (elle::json::ParseError const& e)
                  {
                    Error exception("json parse error");
                    exception.inner_exception(std::current_exception());
                    throw exception;
                  }
                }())
      {
        ELLE_DUMP("{}: parsed JSON: {}", *this, this->_json);
        this->_current.push_back(&this->_json);
      }

      SerializerIn::SerializerIn(elle::json::Json input, bool versioned)
        : Super(versioned)
        , _partial(false)
        , _json(std::move(input))
      {
        this->_current.push_back(&this->_json);
      }

      void
      SerializerIn::_serialize(int64_t& v)
      {
        v = this->_check_type(elle::json::Type::number_integer);
      }

      void
      SerializerIn::_serialize(uint64_t& v)
      {
        int64_t value;
        this->_serialize(value);
        v = static_cast<uint64_t>(value);
      }

      void
      SerializerIn::_serialize(ulong& v)
      {
        meta::static_if<need_unsigned_long>
          ([this](unsigned long& v)
           {
             uint64_t value;
             this->_serialize(value);
             using type = unsigned long;
             using limits = std::numeric_limits<type>;
             if (value > limits::max())
               throw Overflow(this->current_name(), sizeof(type) * 8, true, value);
             v = value;
           },
           [](auto& v)
           {
             unreachable();
           })
          (v);
      }

      void
      SerializerIn::_serialize(int32_t& v)
      {
        this->_serialize_int<int32_t>(v);
      }

      void
      SerializerIn::_serialize(uint32_t& v)
      {
        this->_serialize_int<uint32_t>(v);
      }

      void
      SerializerIn::_serialize(int16_t& v)
      {
        this->_serialize_int<int16_t>(v);
      }

      void
      SerializerIn::_serialize(uint16_t& v)
      {
        this->_serialize_int<uint16_t>(v);
      }

      void
      SerializerIn::_serialize(int8_t& v)
      {
        this->_serialize_int<int8_t>(v);
      }

      void
      SerializerIn::_serialize(uint8_t& v)
      {
        this->_serialize_int<uint8_t>(v);
      }

      template <typename T>
      void
      SerializerIn::_serialize_int(T& v)
      {
        int64_t value;
        this->_serialize(value);
        if (value > std::numeric_limits<T>::max())
          throw Overflow(this->current_name(), sizeof(T) * 8, true, value);
        if (value < std::numeric_limits<T>::min())
          throw Overflow(this->current_name(), sizeof(T) * 8, false, value);
        v = value;
      }

      void
      SerializerIn::_serialize(double& v)
      {
        v = this->_check_type(elle::json::Type::number_float);
      }

      void
      SerializerIn::_serialize(bool& v)
      {
        v = this->_check_type(elle::json::Type::boolean);
      }

      void
      SerializerIn::_serialize_named_option(std::string const& name,
                                            bool,
                                            std::function<void ()> const& f)
      {
        auto& object = this->_check_type(elle::json::Type::object);
        auto it = object.find(name);
        if (it != object.end())
          f();
        else
          ELLE_DEBUG("skip option as JSON key is missing");
      }

      void
      SerializerIn::_serialize_option(bool,
                                      std::function<void ()> const& f)
      {
        if (this->_current.back()->is_null())
          ELLE_DEBUG("skip option as JSON value is null");
        else
          f();
      }

      void
      SerializerIn::_serialize(std::string& v)
      {
        v = this->_check_type(elle::json::Type::string);
      }

      void
      SerializerIn::_serialize(elle::Buffer& buffer)
      {
        // FIXME: don't copy
        std::string str = this->_check_type(elle::json::Type::string);
        std::stringstream encoded(str);
        elle::format::base64::Stream base64(encoded);
        {
          elle::IOStream output(buffer.ostreambuf());
          std::copy(std::istreambuf_iterator<char>(base64),
                    std::istreambuf_iterator<char>(),
                    std::ostreambuf_iterator<char>(output));
        }
      }

      void
      SerializerIn::_serialize(boost::posix_time::ptime& time)
      {
        auto& str = this->_check_type(elle::json::Type::string);
        try
        {
          time = iso8601_to_posix_time(str);
        }
        catch (elle::Error const& e)
        {
          throw FieldError(this->current_name(), e.what());
        }
      }

      void
      SerializerIn::_serialize_time_duration(std::int64_t& ticks,
                                             std::int64_t& num,
                                             std::int64_t& denom)
      {
        auto const repr = this->_check_type(elle::json::Type::string);
        elle::chrono::duration_parse(repr, ticks, num, denom);
      }

      bool
      SerializerIn::_enter(std::string const& name)
      {
        auto& object = this->_check_type(elle::json::Type::object);
        auto it = object.find(name);
        if (it == object.end())
          if (this->_partial)
            return false;
          else
            throw MissingKey(name);
        else
        {
          this->_current.push_back(&*it);
          return true;
        }
      }

      void
      SerializerIn::_leave(std::string const& name)
      {
        this->_current.pop_back();
      }

      void
      SerializerIn::_serialize_array(
        int size,
        std::function<void ()> const& serialize_element)
      {
        auto& array = this->_check_type(elle::json::Type::array);
        for (auto& elt: array)
        {
          this->_current.push_back(&elt);
          serialize_element();
          this->_current.pop_back();
        }
      }

      void
      SerializerIn::_deserialize_dict_key(
        std::function<void (std::string const&)> const& f)
      {
        auto& current = *this->_current.back();
        auto name = this->_names.empty() ? "" : this->_names.back();
        if (current.is_object())
        {
          for (auto it = current.begin(); it != current.end(); ++it)
            if (this->_enter(it.key()))
            {
              elle::SafeFinally leave([&] { this->_leave(it.key()); });
              f(it.key());
            }
        }
        else if (current.is_array())
        {
          for (auto& elt: current)
          {
            if (elt.is_array())
            {
              if (elt.size() != 2)
                throw FieldError(
                  name,
                  elle::print("element has size {} instead of 2", elt.size()));
              if (elt.front().is_string())
              {
                auto key = std::string(elt.front());
                elle::SafeFinally leave([&] { this->_leave(key); });
                this->_current.push_back(&elt.back());
                f(key);
              }
              else
                throw TypeError(name,
                                elle::print("{}", elle::json::Type::string),
                                elle::print("{}", elt.front().type()));
            }
          }
        }
      }

      elle::json::Json&
      SerializerIn::_check_type(elle::json::Type t)
      {
        using elle::json::Type;
        auto& c = *this->_current.back();
        auto name = this->_names.empty() ? "" : this->_names.back();
        if (c.type() == t ||
            t == Type::number_integer && c.type() == Type::number_unsigned)
          return c;
        else
          throw TypeError(
            name, elle::print("{}", t), elle::print("{}", c.type()));
      }
    }
  }
}
