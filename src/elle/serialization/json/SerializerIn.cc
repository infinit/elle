#include <elle/serialization/json/SerializerIn.hh>

#include <limits>

#include <elle/Backtrace.hh>
#include <elle/chrono.hh>
#include <elle/format/base64.hh>
#include <elle/finally.hh>
#include <elle/json/exceptions.hh>
#include <elle/memory.hh>
#include <elle/printf.hh>
#include <elle/serialization/Error.hh>
#include <elle/serialization/json/MissingKey.hh>
#include <elle/serialization/json/Overflow.hh>
#include <elle/serialization/json/TypeError.hh>
#include <elle/serialization/json/FieldError.hh>

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
        : Super(versioned)
        , _partial(false)
        , _json()
        , _current()
      {
        this->_load_json(input);
      }

      SerializerIn::SerializerIn(std::istream& input,
                                 Versions versions,
                                 bool versioned)
        : Super(std::move(versions), versioned)
        , _partial(false)
        , _json()
        , _current()
      {
        this->_load_json(input);
      }

      SerializerIn::SerializerIn(elle::json::Json input, bool versioned)
        : Super(versioned)
        , _partial(false)
        , _json(std::move(input))
        , _current()
      {
        this->_current.push_back(&this->_json);
      }

      void
      SerializerIn::_load_json(std::istream& input)
      {
        try
        {
          this->_json = elle::json::read(input);
          this->_current.push_back(&this->_json);
        }
        catch (elle::json::ParseError const& e)
        {
          Error exception("json parse error");
          exception.inner_exception(std::current_exception());
          throw exception;
        }
      }

      void
      SerializerIn::_serialize(int64_t& v)
      {
        v = this->_check_type<int64_t>();
      }

      void
      SerializerIn::_serialize(uint64_t& v)
      {
        int64_t value;
        this->_serialize(value);
        if (value < 0)
          throw Error(elle::sprintf(
                        "64-bits unsigned underflow on key \"%s\"",
                        this->current_name()));
        v = value;
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
          throw Overflow(this->current_name(), sizeof(T) * 8, true);
        if (value < std::numeric_limits<T>::min())
          throw Overflow(this->current_name(), sizeof(T) * 8, false);
        v = value;
      }

      void
      SerializerIn::_serialize(double& v)
      {
        v = this->_check_type<double, int64_t>();
      }

      void
      SerializerIn::_serialize(bool& v)
      {
        v = this->_check_type<bool>();
      }

      void
      SerializerIn::_serialize_named_option(std::string const& name,
                                            bool,
                                            std::function<void ()> const& f)
      {
        auto& object = this->_check_type<elle::json::Object>();
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
        if (this->_current.back()->type() != typeid(elle::json::NullType))
          f();
        else
          ELLE_DEBUG("skip option as JSON value is null");
      }

      void
      SerializerIn::_serialize(std::string& v)
      {
        v = this->_check_type<std::string>();
      }

      void
      SerializerIn::_serialize(elle::Buffer& buffer)
      {
        auto& str = this->_check_type<std::string>();
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
        auto& str = this->_check_type<std::string>();
        // Use the ISO extended input facet to interpret the string.
        std::stringstream ss(str);
        auto input_facet =
          std::make_unique<boost::posix_time::time_input_facet>();
        // ISO 8601
        input_facet->format("%Y-%m-%dT%H:%M:%S%F");
        ss.imbue(std::locale(ss.getloc(), input_facet.release()));
        if (!(ss >> time))
          throw FieldError(this->current_name(),
                           elle::sprintf("invalid ISO8601 date: %s", str));
        // Check there isn't any leftover.
        std::string leftover;
        std::getline(ss, leftover);
        if (leftover.empty())
          return;
        // Boost can't parse timezones, handle it manually.
        if (leftover == "Z")
          ; // Accept UTC suffix.
        else if ((leftover[0] == '+' || leftover[0] == '-') && leftover.size() == 5)
        {
          // Handle timezone.
          std::stringstream tz(leftover);
          int direction = tz.get() == '+' ? -1 : 1;
          int amount;
          tz >> amount;
          if (tz.get() != -1)
            throw FieldError(
              this->current_name(),
              elle::sprintf("garbage at end of date: %s", leftover));
          time += boost::posix_time::hours(direction * amount / 100);
        }
        else
          throw FieldError(
            this->current_name(),
            elle::sprintf("garbage at end of date: %s", leftover));
        if (!ss.eof())
        {
          std::string leftover;
          std::getline(ss, leftover);
          throw FieldError(
            this->current_name(),
            elle::sprintf("garbage at end of date: %s", leftover));
        }
      }

      void
      SerializerIn::_serialize_time_duration(std::int64_t& ticks,
                                             std::int64_t& num,
                                             std::int64_t& denom)
      {
        auto const repr = this->_check_type<std::string>();
        elle::chrono::duration_parse(repr, ticks, num, denom);
      }

      bool
      SerializerIn::_enter(std::string const& name)
      {
        auto& object = this->_check_type<elle::json::Object>();
        auto it = object.find(name);
        if (it == object.end())
          if (this->_partial)
            return false;
          else
            throw MissingKey(name);
        else
        {
          this->_current.push_back(&it->second);
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
        auto& array = this->_check_type<elle::json::Array>();
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
        if (current.type() == typeid(elle::json::Object))
        {
          const auto& object = boost::any_cast<elle::json::Object>(current);
          for (auto& elt: object)
            if (this->_enter(elt.first))
            {
              elle::SafeFinally leave([&] { this->_leave(elt.first); });
              f(elt.first);
            }
        }
        else if (current.type() == typeid(elle::json::Array))
        {
          auto& array = boost::any_cast<elle::json::Array&>(current);
          for (auto& elt: array)
          {
            if (elt.type() == typeid(elle::json::Array))
            {
              auto& subarray = boost::any_cast<elle::json::Array&>(elt);
              if (subarray.size() == 2
                  && subarray.front().type() == typeid(std::string))
              {
                auto key = boost::any_cast<std::string>(subarray.front());
                elle::SafeFinally leave([&] { this->_leave(key); });
                this->_current.push_back(&subarray.back());
                f(key);
              }
            }
          }
        }
      }

      template <typename T, typename ... Types>
      struct
      any_casts
      {
        static
        T&
        cast(std::string const& name, boost::any& value)
        {
          throw TypeError(name, typeid(T), value.type());
        }
      };

      template <typename T, typename First, typename ... Tail>
      struct
      any_casts<T, First, Tail ...>
      {
        static
        T&
        cast(std::string const& name, boost::any& value)
        {
          if (value.type() == typeid(First))
          {
            value = T(boost::any_cast<First&>(value));
            return boost::any_cast<T&>(value);
          }
          else
            return any_casts<T, Tail ...>::cast(name, value);
        }
      };

      template <typename T, typename ... Alternatives>
      T&
      SerializerIn::_check_type()
      {
        auto& current = *this->_current.back();
        auto name = this->_names.empty() ? "" : this->_names.back();
        if (current.type() == typeid(T))
          return boost::any_cast<T&>(current);
        return any_casts<T, Alternatives ...>::cast(name, current);
      }
    }
  }
}
