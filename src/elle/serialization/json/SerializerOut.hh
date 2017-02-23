#ifndef ELLE_SERIALIZATION_JSON_SERIALIZEROUT_HH
# define ELLE_SERIALIZATION_JSON_SERIALIZEROUT_HH

# include <vector>

# include <boost/any.hpp>

# include <elle/attribute.hh>
# include <elle/serialization/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    namespace json
    {
      class ELLE_API SerializerOut
        : public serialization::SerializerOut
      {
      /*------.
      | Types |
      `------*/
      public:
        using Self = elle::serialization::json::SerializerOut;
        using Super = serialization::SerializerOut;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        SerializerOut(std::ostream& output,
                      bool versioned = true,
                      bool pretty = false);
        SerializerOut(std::ostream& output,
                      Versions versions,
                      bool versioned = true,
                      bool pretty = false);
        ~SerializerOut() noexcept(false);

      /*--------------.
      | Serialization |
      `--------------*/
      protected:

        bool
        _enter(std::string const& name) override;

        void
        _leave(std::string const& name) override;

        void
        _serialize_array(int size,
                         std::function<void ()> const& f) override;

        void
        _serialize_dict_key(std::string const& name,
                            std::function<void ()> const& f) override;

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
                                bool filled,
                                std::function<void ()> const& f) override;

        void
        _serialize_option(bool filled,
                          std::function<void ()> const& f) override;
      private:
        boost::any&
        _get_current();

      /*-----.
      | JSON |
      `-----*/
      private:
        ELLE_ATTRIBUTE(boost::any, json);
        ELLE_ATTRIBUTE(std::vector<boost::any*>, current);
        ELLE_ATTRIBUTE(bool, pretty);
      };
    }
  }
}

#endif
