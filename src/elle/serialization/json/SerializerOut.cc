#include <elle/serialization/json/SerializerOut.hh>

#include <elle/Lazy.hh>
#include <elle/assert.hh>
#include <elle/find.hh>
#include <elle/format/base64.hh>
#include <elle/json/json.hh>
#include <elle/log.hh>
#include <elle/meta.hh>

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
        : Super(versioned)
        , _pretty(pretty)
        , _output(output)
      {
        this->_current.push_back(&this->_json);
      }

      SerializerOut::SerializerOut(std::ostream& output,
                                   Versions versions,
                                   bool versioned,
                                   bool pretty)
        : Super(std::move(versions), versioned)
        , _pretty(pretty)
        , _output(output)
      {
        this->_current.push_back(&this->_json);
      }

      SerializerOut::~SerializerOut() noexcept(false)
      {
        ELLE_TRACE_SCOPE("{}: write JSON", this);
        ELLE_DUMP("%s", elle::json::pretty_print(this->_json));
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
        auto& current = *this->_current.back();
        if (current.is_null())
        {
          ELLE_DEBUG("create current object");
          current = nlohmann::json::object();
        }
        if (current.is_object())
        {
          ELLE_DEBUG_SCOPE("insert key \"%s\"", name);
          // FIXME: hackish way to not serialize version twice when
          // serialize_forward is used.
          if (name == ".version" && find(current, name))
            return false;
          else
          {
            auto it = current.emplace(name, elle::json::Json());
            this->_current.push_back(&it.first.value());
            return true;
          }
        }
        else if (current.is_array())
        {
          ELLE_DEBUG_SCOPE("insert array element");
          current.emplace_back();
          this->_current.push_back(&current.back());
          return true;
        }
        else
          elle::err("cannot serialize key {} in JSON object {}", name, current);
      }

      void
      SerializerOut::_leave(std::string const& name)
      {
        this->_current.pop_back();
      }

      void
      SerializerOut::_serialize_array(int size,
                                      std::function<void ()> const& f)
      {
        auto& current = *this->_current.back();
        ELLE_ASSERT(current.is_null());
        current = nlohmann::json::array();
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
      SerializerOut::_serialize(int64_t& v)
      {
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(uint64_t& v)
      {
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(int32_t& v)
      {
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(uint32_t& v)
      {
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(ulong& v)
      {
        meta::static_if<need_unsigned_long>
          ([this](unsigned long& v)
           {
             auto& current = this->_get_current();
             current = v;
           },
           [](auto& v)
           {
             unreachable();
           })
          (v);
      }

      void
      SerializerOut::_serialize(int16_t& v)
      {
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(uint16_t& v)
      {
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(int8_t& v)
      {
        auto& current = this->_get_current();
        current = int(v);
      }

      void
      SerializerOut::_serialize(uint8_t& v)
      {
        auto& current = this->_get_current();
        current = int(v);
      }

      void
      SerializerOut::_serialize(double& v)
      {
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(bool& v)
      {
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(std::string& v)
      {
        auto& current = this->_get_current();
        current = v;
      }

      void
      SerializerOut::_serialize(elle::Buffer& buffer)
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
      SerializerOut::_serialize(boost::posix_time::ptime& time)
      {
        this->_get_current() = to_iso8601(time);
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
        // Create an empty object if held options are null.
        else if (this->_current.back()->is_null())
          *this->_current.back() = nlohmann::json::object();
      }

      void
      SerializerOut::_serialize_option(bool filled,
                                       std::function<void ()> const& f)
      {
        if (filled)
          f();
        else
        {
          *this->_current.back() = nullptr;
          if (!this->_names.empty())
          {
            ELLE_ASSERT_GT(signed(this->_current.size()), 1);
            auto& last = *this->_current[this->_current.size() - 2];
            if (last.is_object())
              ELLE_ENFORCE(last.erase(this->_names.back()));
          }
        }
      }

      elle::json::Json&
      SerializerOut::_get_current()
      {
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
