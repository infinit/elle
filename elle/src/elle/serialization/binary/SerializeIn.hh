#ifndef ELLE_SERIALIZATION_BINARY_SERIALIZERIN_HH
# define ELLE_SERIALIZATION_BINARY_SERIALIZERIN_HH

# include <vector>

# include <elle/attribute.hh>
# include <elle/serialization/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    namespace binary
    {
       class SerializerIn:
        public serialization::SerializerIn
      {
      /*------.
      | Types |
      `------*/
      public:
        typedef SerializerIn Self;
        typedef serialization::SerializerIn Super;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        SerializerIn(std::istream& input,
                     bool versioned = true);
        SerializerIn(std::istream& input,
                     elle::Version version,
                     bool versioned = true);
        SerializerIn(elle::json::Json input,
                     bool versioned = true);
        /*--------------.
      | Serialization |
      `--------------*/
      protected:
        virtual
        void
        _serialize(std::string const& name, int64_t& v) override;
        virtual
        void
        _serialize(std::string const& name, uint64_t& v) override;
        virtual
        void
        _serialize(std::string const& name, int32_t& v) override;
        virtual
        void
        _serialize(std::string const& name, uint32_t& v) override;
        virtual
        void
        _serialize(std::string const& name, int8_t& v) override;
        virtual
        void
        _serialize(std::string const& name, uint8_t& v) override;
        virtual
        void
        _serialize(std::string const& name, double& v) override;
        virtual
        void
        _serialize(std::string const& name, bool& v) override;
        virtual
        void
        _serialize(std::string const& name, std::string& v) override;
        virtual
        void
        _serialize(std::string const& name, elle::Buffer& v) override;
        virtual
        void
        _serialize(std::string const& name,
                   boost::posix_time::ptime& v) override;
        virtual
        void
        _serialize_option(std::string const& name,
                          bool,
                          std::function<void ()> const& f);
        virtual
        void
        _serialize_array(std::string const& name,
                         std::function<void ()> const& f) override;
        virtual
        bool
        _enter(std::string const& name);
        virtual
        void
        _leave(std::string const& name);
      private:
        int64_t _serialize_number()
    }
  }
}

#endif