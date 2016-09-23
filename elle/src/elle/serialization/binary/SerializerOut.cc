#include <elle/serialization/binary/SerializerOut.hh>

#include <elle/assert.hh>
#include <elle/finally.hh>
#include <elle/format/base64.hh>
#include <elle/json/json.hh>

ELLE_LOG_COMPONENT("elle.serialization.binary.SerializerOut")

namespace elle
{
  namespace serialization
  {
    namespace binary
    {
      /*-------------.
      | Construction |
      `-------------*/

      SerializerOut::SerializerOut(std::ostream& output, bool versioned)
        : Super(output, versioned)
      {
        this->_write_magic(output);
      }

      SerializerOut::SerializerOut(std::ostream& output,
                                   Versions versions,
                                   bool versioned)
        : Super(output, std::move(versions), versioned)
      {
        this->_write_magic(output);
      }

      void
      SerializerOut::_write_magic(std::ostream& output)
      {
        static char const magic = 0;
        output.write(&magic, 1);
      }

      SerializerOut::~SerializerOut()
      {}

      /*--------------.
      | Serialization |
      `--------------*/

      void
      SerializerOut::_size(int size)
      {
        _serialize_number(size);
      }

      bool
      SerializerOut::_text() const
      {
        return false;
      }

      void
      SerializerOut::_serialize_number(int64_t n_)
      {
        SerializerOut::serialize_number(this->output(), n_);
      }

      size_t
      SerializerOut::serialize_number(std::ostream& output,
                                      int64_t n_)
      {
        int64_t n = n_;
        bool neg = n < 0;
        if (neg)
          n = -n;
        if (n <= 0x3f)
        { // sgn 0 val
          unsigned char ser = (neg ? 0x80 : 0) + n;
          ELLE_DUMP("serialize %s as 0x%02x", n_, int(ser));
          output.write((const char*)&ser, 1);
          return 1;
        }
        else if (n <= 0x1fff)
        { // sgn 1 0 val val2
          unsigned char ser[2];
          ser[0] = (neg ? 0xC0 : 0x40) + (n >> 8);
          ser[1] = n;
          ELLE_DUMP("serialize %s as 0x%02x%02x", n_, int(ser[0]), int(ser[1]));
          output.write((const char*)ser, 2);
          return 2;
        } // sgn 1 1 0 val val2 val3
        else if (n <= 0x0fffff)
        {
          unsigned char ser[3];
          ser[0] = (neg ? 0xe0 : 0x60) + (n >> 16);
          ser[1] = n >> 8;
          ser[2] = n;
          ELLE_DUMP("serialize %s as 0x%02x%02x%02x",
                    n_, int(ser[0]), int(ser[1]), int(ser[2]));
          output.write((const char*)ser, 3);
          return 3;
        }
        else
        {
          unsigned char c = neg? 0xFF : 0x7F;
          output.write((const char*)&c, 1);
          output.write((const char*)(const void*)&n, 8);
          ELLE_DUMP("serialize %s as 0x%02x%08x", n_, int(c), n);
          return 9;
          /*
          unsigned char ser[9];
          ser[0] = neg? 0xFF : 0x7F;
          for (int i=1; i<9; ++i)
            ser[i] = n >> ((i-1)*8);
          output.write((const char*)ser, 9);*/
        }
      }

      void
      SerializerOut::_serialize_array(std::string const& name,
                                      int size,
                                      std::function<void ()> const& f)
      {
        ELLE_TRACE_SCOPE("%s: serialize array \"%s\"", *this, name);
        _serialize_number(size);
        f();
      }

      void
      SerializerOut::_serialize_dict_key(
        std::string const& name,
        std::function<void ()> const& f)
      {
        this->_serialize("string", const_cast<std::string&>(name));
        f();
      }

      void
      SerializerOut::_serialize(std::string const& name, int64_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\"", *this, name);
        _serialize_number(v);
      }

      void
      SerializerOut::_serialize(std::string const& name, uint64_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\"", *this, name);
        _serialize_number(v);
      }

      void
      SerializerOut::_serialize(std::string const& name, int32_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\"", *this, name);
        _serialize_number(v);
      }

      void
      SerializerOut::_serialize(std::string const& name, uint32_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\"", *this, name);
        _serialize_number(v);
      }

      void
      SerializerOut::_serialize(std::string const& name, int16_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\"", *this, name);
        _serialize_number(v);
      }

      void
      SerializerOut::_serialize(std::string const& name, uint16_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\"", *this, name);
        _serialize_number(v);
      }

      void
      SerializerOut::_serialize(std::string const& name, int8_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\"", *this, name);
        _serialize_number(v);
      }

      void
      SerializerOut::_serialize(std::string const& name, uint8_t& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize integer \"%s\"", *this, name);
        _serialize_number(v);
      }

      void
      SerializerOut::_serialize(std::string const& name, double& v)
      {
        output().write((const char*)&v, sizeof(double));
      }

      void
      SerializerOut::_serialize(std::string const& name, bool& v)
      {
        _serialize_number(v?1:0);
      }

      void
      SerializerOut::_serialize(std::string const& name, std::string& v)
      {
        ELLE_TRACE_SCOPE("%s: serialize string \"%s\"", *this, name);
        elle::Buffer buffer(v.c_str(), v.size());
        this->_serialize(name, buffer);
      }

      void
      SerializerOut::_serialize(std::string const& name, elle::Buffer& buffer)
      {
        ELLE_TRACE_SCOPE("%s: serialize buffer \"%s\"", *this, name);
        ELLE_DEBUG("%s: serialize size: %s", *this, buffer.size())
          this->_serialize_number(buffer.size());
        ELLE_DEBUG("%s: serialize content: %f", *this, buffer)
          output().write((const char*)buffer.contents(), buffer.size());
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
        std::string s(ss.str());
        _serialize(name, s);
      }

      void
      SerializerOut::_serialize_time_duration(std::int64_t& ticks,
                                              std::int64_t& num,
                                              std::int64_t& denom)
      {
        this->_serialize_number(ticks);
        this->_serialize_number(num);
        this->_serialize_number(denom);
      }

      void
      SerializerOut::_serialize_named_option(std::string const&,
                                       bool,
                                       std::function<void ()> const& f)
      {
        f();
      }

      void
      SerializerOut::_serialize_option(std::string const& name,
                                       bool filled,
                                       std::function<void ()> const& f)
      {
        ELLE_TRACE_SCOPE("%s: serialize option \"%s\"", *this, name);
        _serialize(name, filled);
        if (filled)
          f();
      }
    }
  }
}
