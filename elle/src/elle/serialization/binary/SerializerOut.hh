#ifndef ELLE_SERIALIZATION_BINARY_SERIALIZEROUT_HH
# define ELLE_SERIALIZATION_BINARY_SERIALIZEROUT_HH

# include <vector>

# include <elle/attribute.hh>
# include <elle/serialization/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    namespace binary
    {
      class SerializerOut:
        public serialization::SerializerOut
      {
      /*------.
      | Types |
      `------*/
      public:
        typedef SerializerOut Self;
        typedef serialization::SerializerOut Super;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        SerializerOut(std::ostream& output, bool versioned = true);
        SerializerOut(std::ostream& output,
                      Versions versions, bool versioned = true);
        virtual
        ~SerializerOut();
      private:
        void
        _write_magic(std::ostream& output);

      /*--------------.
      | Serialization |
      `--------------*/
      protected:
        virtual
        void
        _size(int size) override;
        virtual
        bool
        _text() const override;
        virtual
        void
        _serialize_array(std::string const& name,
                         int size,
                         std::function<void ()> const& f) override;
        virtual
        void
        _serialize_dict_key(std::string const& name,
                            std::function<void ()> const& f) override;
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
        _serialize(std::string const& name, int16_t& v) override;
        virtual
        void
        _serialize(std::string const& name, uint16_t& v) override;
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
        _serialize_time_duration(std::int64_t& ticks,
                                 std::int64_t& num,
                                 std::int64_t& denom) override;
        virtual
        void
        _serialize_named_option(std::string const& name,
                                bool filled,
                                std::function<void ()> const& f) override;
        virtual
        void
        _serialize_option(std::string const& name,
                          bool filled,
                          std::function<void ()> const& f) override;
      private:
        void
        _serialize_number(int64_t number);

      /*-----.
      | BINARY |
      `-----*/
      private:


      };
    }
  }
}

#endif
