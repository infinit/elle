#include <elle/serialization/binary/SerializerIn.hh>

#include <elle/serialization/json/Overflow.hh>
#include <elle/serialization/json/FieldError.hh>

ELLE_LOG_COMPONENT("elle.serialization.binary.SerializerIn")

namespace elle
{
  namespace serialization
  {
    namespace binary
    {
      SerializerIn::SerializerIn(std::istream& input,
                                 bool versioned)
        : Super(input, versioned)
      {
        this->_check_magic(input);
      }

      SerializerIn::SerializerIn(std::istream& input,
                                 Versions versions,
                                 bool versioned)
        : Super(input, std::move(versions), versioned)
      {
        this->_check_magic(input);
      }

      void
      SerializerIn::_check_magic(std::istream& input)
      {
        char magic;
        input.read(&magic, 1);
        if (input.gcount() != 1)
          throw Error("unable to read magic");
        if (magic != 0)
          throw Error(
            elle::sprintf("wrong magic for binary serialization: 0x%2x",
                          int(magic)));
      }

      bool
      SerializerIn::_text() const
      {
        return false;
      }

      void
      SerializerIn::_serialize(std::string const& name, int64_t& v)
      {
        ELLE_TRACE_SCOPE("%s: deserialize integer \"%s\"", *this, name);
        v = _serialize_number();
        ELLE_DEBUG("value: %s", v);
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
        this->_serialize_int<int32_t>(name, v);
      }

      void
      SerializerIn::_serialize(std::string const& name, uint32_t& v)
      {
        this->_serialize_int<uint32_t>(name, v);
      }

      void
      SerializerIn::_serialize(std::string const& name, int16_t& v)
      {
        this->_serialize_int<int16_t>(name, v);
      }

      void
      SerializerIn::_serialize(std::string const& name, uint16_t& v)
      {
        this->_serialize_int<uint16_t>(name, v);
      }

      void
      SerializerIn::_serialize(std::string const& name, int8_t& v)
      {
        this->_serialize_int<int8_t>(name, v);
      }

      void
      SerializerIn::_serialize(std::string const& name, uint8_t& v)
      {
        this->_serialize_int<uint8_t>(name, v);
      }

      template <typename T>
      void
      SerializerIn::_serialize_int(std::string const& name, T& v)
      {
        int64_t value;
        this->_serialize(name, value);
        if (value > std::numeric_limits<T>::max())
          throw json::Overflow(name, sizeof(T) * 8, true);
        if (value < std::numeric_limits<T>::min())
          throw json::Overflow(name, sizeof(T) * 8, false);
        v = value;
      }

      void
      SerializerIn::_serialize(std::string const& name, double& v)
      {
        ELLE_TRACE_SCOPE("%s: deserialize double \"%s\"", *this, name);
        input().read((char*)(void*)&v, sizeof(double));
      }

      void
      SerializerIn::_serialize(std::string const& name, bool& v)
      {
        int val;
        _serialize(name, val);
        if (val !=0 && val != 1)
          throw json::Overflow(name, 1, true);
        v = val ? true:false;
      }

      void
      SerializerIn::_serialize_named_option(std::string const& name,
                                            bool,
                                            std::function<void ()> const& f)
      {
        f();
      }

      void
      SerializerIn::_serialize_option(std::string const& name,
                                      bool,
                                      std::function<void ()> const& f)
      {
        ELLE_TRACE_SCOPE("%s: deserialize option \"%s\"", *this, name);
        bool filled;
        this->_serialize(name, filled);
        if (filled)
          f();
      }

      void
      SerializerIn::_serialize(std::string const& name, std::string& v)
      {
        ELLE_TRACE_SCOPE("%s: deserialize string \"%s\"", *this, name);
        elle::Buffer b;
        this->_serialize(name, b);
        v = b.string();
      }

      void
      SerializerIn::_serialize(std::string const& name, elle::Buffer& buffer)
      {
        ELLE_TRACE_SCOPE("%s: deserialize buffer \"%s\"", *this, name);
        int sz = _serialize_number();
        ELLE_DEBUG("%s: deserialize size: %s", *this, sz);
        buffer.size(sz);
        input().read((char*)buffer.mutable_contents(), sz);
        if (input().gcount() != sz)
          throw Error(elle::sprintf(
            "%s: short read when deserializing '%s': expected %s, got %s",
            *this, name, sz, input().gcount()));
      }

      void
      SerializerIn::_serialize(std::string const& name,
                               boost::posix_time::ptime& time)
      {
        ELLE_TRACE_SCOPE("%s: deserialize date \"%s\"", *this, name);
        std::string str;
        _serialize(name, str);
        // Use the ISO extended input facet to interpret the string.
        std::stringstream ss(str);
        auto input_facet =
          elle::make_unique<boost::posix_time::time_input_facet>();
        // ISO 8601
        input_facet->format("%Y-%m-%dT%H:%M:%S%F");
        ss.imbue(std::locale(ss.getloc(), input_facet.release()));
        if (!(ss >> time))
          throw json::FieldError
            (name, elle::sprintf("invalid ISO8601 date: %s", str));
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
            throw json::FieldError
              (name, elle::sprintf("garbage at end of date: %s", leftover));
          time += boost::posix_time::hours(direction * amount / 100);
        }
        else
          throw json::FieldError
            (name, elle::sprintf("garbage at end of date: %s", leftover));
        if (!ss.eof())
        {
          std::string leftover;
          std::getline(ss, leftover);
          throw json::FieldError
            (name, elle::sprintf("garbage at end of date: %s", leftover));
        }
      }

      void
      SerializerIn::_serialize_time_duration(std::int64_t& ticks,
                                             std::int64_t& num,
                                             std::int64_t& denom)
      {
        ticks = this->_serialize_number();
        num = this->_serialize_number();
        denom = this->_serialize_number();
      }

      bool
      SerializerIn::_enter(std::string const& name)
      {
        ELLE_TRACE_SCOPE("%s: enter \"%s\"", *this, name);
        return true;
      }

      void
      SerializerIn::_leave(std::string const& name)
      {
        ELLE_TRACE_SCOPE("%s: leave \"%s\"", *this, name);
      }

      void
      SerializerIn::_serialize_array(
        std::string const& name,
        int,
        std::function<void ()> const& serialize_element)
      {
        ELLE_TRACE_SCOPE("%s: deserialize array \"%s\"", *this, name);
        int count = _serialize_number();
        for (int i=0; i<count; ++i)
          serialize_element();
      }

      void
      SerializerIn::_deserialize_dict_key(
        std::function<void (std::string const&)> const& f)
      {
        int count = _serialize_number();
        for (int i = 0; i < count; ++i)
        {
          std::string key;
          _serialize("key", key);
          f(key);
        }
      }

      static
      char
      get(std::istream& s)
      {
        int res = s.get();
        if (res == EOF)
          throw Error("end of stream while reading number");
        return res;
      }

      int64_t
      SerializerIn::_serialize_number()
      {
        ELLE_DEBUG_SCOPE("deserialize number");
        uint64_t res;
        unsigned char c = get(input());
        bool negative = c & 0x80;
        if (!(c & 0x40))
        {
          ELLE_DUMP("1-byte coding");
          res = c&0x3f;
        }
        else if (! (c&0x20))
        {
          ELLE_DUMP("2-bytes coding");
          unsigned char c2 = get(input());
          res = ((c&0x1F) << 8) + c2;
        }
        else if (! (c&0x10))
        {
          ELLE_DUMP("4-bytes coding");
          unsigned char c2 = get(input());
          unsigned char c3 = get(input());
          res = ((c&0x0F) << 16) + (c2 << 8) + c3;
        }
        else
        {
          ELLE_DUMP("8-bytes coding");
          input().read((char*)(void*)&res, 8);
          /*
          unsigned char elems[8];
          input().read((char*)elems, 8);
          res = 0;
          for (int i=0; i<8; ++i)
            res += uint64_t(elems[i]) << (8*i);*/
        }
        int64_t s = negative ? - (int64_t)res : res;
        ELLE_DEBUG("value: %s", s);
        return s;
      }
    }
  }
}
