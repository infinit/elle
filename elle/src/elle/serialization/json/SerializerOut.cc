#include <elle/serialization/json/SerializerOut.hh>

#include <elle/Lazy.hh>
#include <elle/assert.hh>
#include <elle/format/base64.hh>
#include <elle/json/json.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.serialization.json.SerializerOut")

namespace elle
{
  namespace serialization
  {
    namespace json
    {
      /*-------------.
      | Construction |
      `-------------*/

      SerializerOut::SerializerOut(std::ostream& output,
                                   bool versioned,
                                   bool pretty)
        : Super(output, versioned)
        , _json()
        , _current()
        , _pretty(pretty)
      {
        this->_current.push_back(&this->_json);
      }

      SerializerOut::SerializerOut(std::ostream& output,
                                   Versions versions,
                                   bool versioned,
                                   bool pretty)
        : Super(output, std::move(versions), versioned)
        , _json()
        , _current()
        , _pretty(pretty)
      {
        this->_current.push_back(&this->_json);
      }

      SerializerOut::~SerializerOut() noexcept(false)
      {
        ELLE_TRACE_SCOPE("%s: write JSON %s", this, this->output());
        ELLE_DUMP(
          "%s",
          elle::lazy([&] { return elle::json::pretty_print(this->_json); }));
        if (this->_pretty)
          this->output() << elle::json::pretty_print(this->_json);
        else
          elle::json::write(this->output(), this->_json);
      }

      /*--------------.
      | Serialization |
      `--------------*/

      bool
      SerializerOut::_enter(std::string const& name)
      {
        ELLE_ASSERT(!this->_current.empty());
        auto& current = *this->_current.back();
        if (current.empty())
        {
          ELLE_DEBUG("create current object");
          current = elle::json::Object();
        }
        if (current.type() == typeid(elle::json::Object))
        {
          ELLE_DEBUG_SCOPE("insert key \"%s\"", name);
          auto& object = boost::any_cast<elle::json::Object&>(current);
          // FIXME: hackish way to not serialize version twice when
          // serialize_forward is used.
          if (name == ".version" && object.find(name) != object.end())
            return false;
          auto it = object.insert(std::make_pair(name, boost::any()));
          this->_current.push_back(&it.first->second);
        }
        else if (current.type() == typeid(elle::json::Array))
        {
          ELLE_DEBUG_SCOPE("insert array element");
          auto& array = boost::any_cast<elle::json::Array&>(current);
          array.emplace_back();
          this->_current.push_back(&array.back());
        }
        else
        {
          ELLE_ABORT("cannot serialize a composite and a fundamental object "
                     "in key %s", name);
        }
        return true;
      }

      void
      SerializerOut::_leave(std::string const& name)
      {
        ELLE_ASSERT(!this->_current.empty());
        this->_current.pop_back();
      }

      void
      SerializerOut::_serialize_array(std::string const& name,
                                      int size,
                                      std::function<void ()> const& f)
      {
        ELLE_TRACE_SCOPE("%s: create array", *this);
        ELLE_ASSERT(!this->_current.empty());
        auto& current = *this->_current.back();
        ELLE_ASSERT(current.empty());
        current = elle::json::Array();
        f();
      }

      void
      SerializerOut::_serialize_dict_key(
        std::string const& name,
        std::function<void ()> const& f)
      {
        ELLE_TRACE_SCOPE("%s: name = %s", *this, name);
        if (this->_enter(name))
          f();
      }

      void
      SerializerOut::_serialize(std::string const& name, int64_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\": %s", *this, name, v);
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(std::string const& name, uint64_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\": %s", *this, name, v);
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(std::string const& name, int32_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\": %s", *this, name, v);
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(std::string const& name, uint32_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\": %s", *this, name, v);
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(std::string const& name, int16_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\": %s", *this, name, v);
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(std::string const& name, uint16_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\": %s", *this, name, v);
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(std::string const& name, int8_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\": %s", *this, name, v);
        auto& current = this->_get_current();
        current = int(v);
      }

      void
      SerializerOut::_serialize(std::string const& name, uint8_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\": %s", *this, name, v);
        auto& current = this->_get_current();
        current = int(v);
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
      SerializerOut::_serialize(std::string const& name, elle::Buffer& buffer)
      {
        std::stringstream encoded;
        {
          elle::format::base64::Stream base64(encoded);
          base64.write(reinterpret_cast<char*>(buffer.contents()),
                       buffer.size());
        }
        auto& current = this->_get_current();
        current = encoded.str();
      }

      void
      SerializerOut::_serialize(std::string const& name,
                                boost::posix_time::ptime& time)
      {
        std::stringstream ss;
        auto output_facet =
          elle::make_unique<boost::posix_time::time_facet>();
        // ISO 8601
        output_facet->format("%Y-%m-%dT%H:%M:%S%F%q");
        ss.imbue(std::locale(ss.getloc(), output_facet.release()));
        ss << time;
        this->_get_current() = ss.str();
      }

      void
      SerializerOut::_serialize_time_duration(std::int64_t& ticks_,
                                              std::int64_t& num_,
                                              std::int64_t& denom_)
      {
        auto ticks = ticks_;
        auto num = num_;
        auto denom = denom_;
        auto order = 3;
        while (denom % 1000 == 0)
        {
          denom /= 1000;
          if (ticks % 1000 == 0)
            ticks /= 1000;
          else
            ++order;
        }
        if (denom != 1)
          if (1000 % denom == 0)
          {
            num *= 1000 / denom;
            denom *= 1;
            ++order;
          }
          else
            ELLE_ABORT("cannot safely represent time with ratio %s:%s",
                       num_, denom_);
        ELLE_ASSERT_EQ(denom, 1);
        char orders[][9] = {"d", "h", "min", "s", "ms", "us", "ns", "ps", "fs"};
        if (order >= 9)
          ELLE_ABORT("cannot safely represent time with ratio %s:%s",
                       num_, denom_);
        ticks *= num;
        if (order == 3)
        {
          if (ticks % 60 == 0)
          {
            ticks /= 60;
            --order;
            if (ticks % 60 == 0)
            {
              ticks /= 60;
              --order;
              if (ticks % 24 == 0)
              {
                ticks /= 24;
                --order;
              }
            }
          }
        }
        auto& current = this->_get_current();
        current = elle::sprintf("%s%s", ticks, orders[order]);
      }

      void
      SerializerOut::_serialize_named_option(std::string const& name,
                                             bool filled,
                                             std::function<void ()> const& f)
      {
        if (filled)
          f();
      }

      void
      SerializerOut::_serialize_option(std::string const& name,
                                       bool filled,
                                       std::function<void ()> const& f)
      {
        if (filled)
          f();
        else
        {
          *this->_current.back() = elle::json::NullType();
          if (!this->_names.empty())
          {
            ELLE_ASSERT_GT(signed(this->_current.size()), 1);
            auto& last = *this->_current[this->_current.size() - 2];
            if (last.type() == typeid(elle::json::Object))
              ELLE_ENFORCE(boost::any_cast<elle::json::Object&>(last).erase(
                             this->_names.back()));
          }
        }
      }

      boost::any&
      SerializerOut::_get_current()
      {
        ELLE_ASSERT(!this->_current.empty());
        auto& current = *this->_current.back();
        if (!current.empty())
        {
          ELLE_ABORT("%s: serializing in-place to an already filled object: %s",
                     *this, elle::json::pretty_print(current));
        }
        return current;
      }
    }
  }
}
