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
      {}

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
      SerializerOut::_serialize_number(int64_t n)
      {
        uint64_t number;
        bool neg = n < 0;
        if (neg)
          number = -n;
        else
          number = n;
        if (number <= 0x3f)
        { // sgn 0 val
          unsigned char ser = (neg ? 0x80 : 0) + number;
          output().write((const char*)&ser, 1);
        }
        else if (number <= 0x1fff)
        { // sgn 1 0 val val2
          unsigned char ser[2];
          ser[0] = (neg ? 0xC0 : 0x40) + (number >> 8);
          ser[1] = number;
          output().write((const char*)ser, 2);
        } // sgn 1 1 0 val val2 val3
        else if (number <= 0x0fffff)
        {
          unsigned char ser[3];
          ser[0] = (neg ? 0xe0 : 0x60) + (number >> 16);
          ser[1] = number >> 8;
          ser[2] = number;
          output().write((const char*)ser, 3);
        }
        else
        {
          unsigned char c = neg? 0xFF : 0x7F;
          output().write((const char*)&c, 1);
          output().write((const char*)(const void*)&number, 8);
          /*
          unsigned char ser[9];
          ser[0] = neg? 0xFF : 0x7F;
          for (int i=1; i<9; ++i)
            ser[i] = number >> ((i-1)*8);
          output().write((const char*)ser, 9);*/
        }
      }

      void
      SerializerOut::_serialize_array(std::string const& name,
                                      int size,
                                      std::function<void ()> const& f)
      {
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
        _serialize_number(v);
      }

      void
      SerializerOut::_serialize(std::string const& name, uint64_t& v)
      {
        _serialize_number(v);
      }

      void
      SerializerOut::_serialize(std::string const& name, int32_t& v)
      {
        _serialize_number(v);
      }

      void
      SerializerOut::_serialize(std::string const& name, uint32_t& v)
      {
        _serialize_number(v);
      }

      void
      SerializerOut::_serialize(std::string const& name, int8_t& v)
      {
        _serialize_number(v);
      }

      void
      SerializerOut::_serialize(std::string const& name, uint8_t& v)
      {
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
      SerializerOut::_serialize_option(std::string const& name,
                                       bool filled,
                                       std::function<void ()> const& f)
      {
        _serialize(name, filled);
        if (filled)
          f();
      }
    }
  }
}
