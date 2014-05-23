#include <elle/serialization/json/SerializerIn.hh>

#include <limits>

#include <elle/Backtrace.hh>
#include <elle/format/base64.hh>
#include <elle/json/exceptions.hh>
#include <elle/memory.hh>
#include <elle/printf.hh>
#include <elle/serialization/Error.hh>
#include <elle/serialization/json/MissingKey.hh>
#include <elle/serialization/json/TypeError.hh>

namespace elle
{
  namespace serialization
  {
    namespace json
    {
      /*-------------.
      | Construction |
      `-------------*/

      SerializerIn::SerializerIn(std::istream& input)
        : Super(input)
        , _partial(false)
        , _json()
        , _current()
      {
        try
        {
          this->_json = elle::json::read(input);
          this->_current.push_back(&this->_json);
        }
        catch (elle::json::ParserError const& e)
        {
          Error exception("json parse error");
          exception.inner_exception(
            elle::make_unique<elle::json::ParserError>(e));
          throw exception;
        }
      }

      SerializerIn::SerializerIn(elle::json::Json input)
        : Super(ELLE_SFINAE_INSTANCE(std::istream)) // FIXME
        , _partial(false)
        , _json(std::move(input))
        , _current()
      {
        this->_current.push_back(&this->_json);
      }

      void
      SerializerIn::_serialize(std::string const& name, int64_t& v)
      {
        v = this->_check_type<int64_t>(name);
      }

      void
      SerializerIn::_serialize(std::string const& name, uint64_t& v)
      {
        int64_t value;
        this->_serialize(name, value);
        if (value < 0)
          throw Error(elle::sprintf(
                        "64-bits unsigned underflow on key \"%s\"", name));
        v = value;
      }

      void
      SerializerIn::_serialize(std::string const& name, int32_t& v)
      {
        int64_t value;
        this->_serialize(name, value);
        if (value > std::numeric_limits<int32_t>::max())
          throw Error(elle::sprintf(
                        "32-bits overflow on key \"%s\"", name));
        if (value < std::numeric_limits<int32_t>::min())
          throw Error(elle::sprintf(
                        "32-bits underflow on key \"%s\"", name));
        v = value;
      }

      void
      SerializerIn::_serialize(std::string const& name, uint32_t& v)
      {
        int32_t value;
        this->_serialize(name, value);
        if (value < 0)
          throw Error(elle::sprintf(
                        "32-bits unsigned underflow on key \"%s\"", name));
        v = value;
      }

      void
      SerializerIn::_serialize(std::string const& name, double& v)
      {
        v = this->_check_type<double, int64_t>(name);
      }

      void
      SerializerIn::_serialize(std::string const& name, bool& v)
      {
        v = this->_check_type<bool>(name);
      }

      void
      SerializerIn::_serialize_option(std::string const& name,
                                      bool,
                                      std::function<void ()> const& f)
      {
        auto& object = this->_check_type<elle::json::Object>(name);
        if (object.find(name) != object.end())
          f();
      }

      void
      SerializerIn::_serialize(std::string const& name, std::string& v)
      {
        v = this->_check_type<std::string>(name);
      }

      void
      SerializerIn::_serialize(std::string const& name, elle::Buffer& buffer)
      {
        std::stringstream encoded(this->_check_type<std::string>(name));
        elle::format::base64::Stream base64(encoded);
        elle::IOStream output(new elle::OutputStreamBuffer(buffer));
        std::copy(std::istream_iterator<char>(base64),
                  std::istream_iterator<char>(),
                  std::ostream_iterator<char>(output));
      }

      bool
      SerializerIn::_enter(std::string const& name)
      {
        auto& object = this->_check_type<elle::json::Object>(name);
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
      SerializerIn::_leave(std::string const&)
      {
        this->_current.pop_back();
      }

      void
      SerializerIn::_serialize_array(
        std::string const& name,
        std::function<void ()> const& serialize_element)
      {
        auto& array = this->_check_type<elle::json::Array>(name);
        for (auto& elt: array)
        {
          this->_current.push_back(&elt);
          serialize_element();
          this->_current.pop_back();
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
      SerializerIn::_check_type(std::string const& name)
      {
        auto& current = *this->_current.back();
        if (current.type() == typeid(T))
          return boost::any_cast<T&>(current);
        return any_casts<T, Alternatives ...>::cast(name, current);
      }
    }
  }
}
