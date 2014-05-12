#include <elle/serialization/json/SerializerOut.hh>

#include <elle/assert.hh>
#include <elle/json/json.hh>

namespace elle
{
  namespace serialization
  {
    namespace json
    {
      /*-------------.
      | Construction |
      `-------------*/

      SerializerOut::SerializerOut(std::ostream& output)
        : Super(output)
        , _json()
        , _current()
      {
        this->_current.push_back(&this->_json);
      }

      SerializerOut::~SerializerOut() noexcept(false)
      {
        elle::json::write(this->output(), this->_json);
      }

      /*--------------.
      | Serialization |
      `--------------*/

      void
      SerializerOut::_enter(std::string const& name)
      {
        ELLE_ASSERT(!this->_current.empty());
        auto& current = *this->_current.back();
        if (current.empty())
          current = elle::json::Object();
        if (current.type() == typeid(elle::json::Object))
        {
          auto& object = boost::any_cast<elle::json::Object&>(current);
          auto it = object.insert(std::make_pair(name, boost::any()));
          this->_current.push_back(&it.first->second);
        }
        else if (current.type() == typeid(elle::json::Array))
        {
          auto& array = boost::any_cast<elle::json::Array&>(current);
          array.emplace_back();
          this->_current.push_back(&array.back());
        }
        else
        {
          ELLE_ABORT("cannot serialize a composite and a fundamental object "
                     "in key %s", name);
        }
      }

      void
      SerializerOut::_leave(std::string const& name)
      {
        ELLE_ASSERT(!this->_current.empty());
        this->_current.pop_back();
      }

      void
      SerializerOut::_serialize_array(std::string const& name,
                                      std::function<void ()> const& f)
      {
        ELLE_ASSERT(!this->_current.empty());
        auto& current = *this->_current.back();
        ELLE_ASSERT(current.empty());
        current = elle::json::Array();
        f();
      }

      void
      SerializerOut::_serialize(std::string const& name, int64_t& v)
      {
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(std::string const& name, int32_t& v)
      {
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(std::string const& name, double& v)
      {
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(std::string const& name, bool& v)
      {
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(std::string const& name, std::string& v)
      {
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize_option(std::string const& name,
                                       bool filled,
                                       std::function<void ()> const& f)
      {
        if (filled)
          f();
      }

      boost::any&
      SerializerOut::_get_current()
      {
        ELLE_ASSERT(!this->_current.empty());
        auto& current = *this->_current.back();
        ELLE_ASSERT(current.empty());
        return current;
      }
    }
  }
}
