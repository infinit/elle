#pragma once

#include <elle/json/json.hh>
#include <elle/serialization/SerializerIn.hh>

namespace elle
{
  namespace serialization
  {
    namespace json
    {
      class ELLE_API SerializerIn
        : public serialization::SerializerIn
      {
      /*------.
      | Types |
      `------*/
      public:
        using Self = elle::serialization::json::SerializerIn;
        using Super = serialization::SerializerIn;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        SerializerIn(std::istream& input, bool versioned = true);
        SerializerIn(std::istream& input,
                     Versions versions, bool versioned = true);
        SerializerIn(elle::json::Json input, bool versioned = true);
      private:
        void
        _load_json(std::istream& input);

      /*--------------.
      | Configuration |
      `--------------*/
      public:
        ELLE_ATTRIBUTE_RW(bool, partial);

      /*--------------.
      | Serialization |
      `--------------*/
      protected:
        void
        _serialize(int64_t& v) override;
        void
        _serialize(uint64_t& v) override;
        void
        _serialize(int32_t& v) override;
        void
        _serialize(uint32_t& v) override;
        void
        _serialize(int16_t& v) override;
        void
        _serialize(uint16_t& v) override;
        void
        _serialize(int8_t& v) override;
        void
        _serialize(uint8_t& v) override;
        void
        _serialize(double& v) override;
        void
        _serialize(bool& v) override;
        void
        _serialize(std::string& v) override;
        void
        _serialize(elle::Buffer& v) override;
        void
        _serialize(boost::posix_time::ptime& v) override;
        void
        _serialize_time_duration(std::int64_t& ticks,
                                 std::int64_t& num,
                                 std::int64_t& denom) override;
        void
        _serialize_named_option(std::string const& name,
                                bool,
                                std::function<void ()> const& f) override;
        void
        _serialize_option(bool,
                          std::function<void ()> const& f) override;
        void
        _serialize_array(int size,
                         std::function<void ()> const& f) override;
        void
        _deserialize_dict_key(
          std::function<void (std::string const&)> const& f) override;
        bool
        _enter(std::string const& name) override;
        void
        _leave(std::string const& name) override;

        ELLE_ATTRIBUTE(boost::any, json);
        ELLE_ATTRIBUTE(std::vector<boost::any*>, current);
        ELLE_ATTRIBUTE(std::istream&, input);
      private:
        template <typename T, typename ... Alternatives>
        T&
        _check_type();
        template <typename T>
        void
        _serialize_int(T& v);
      };
    }
  }
}
