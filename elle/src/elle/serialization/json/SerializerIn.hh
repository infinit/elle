#ifndef ELLE_SERIALIZATION_JSON_SERIALIZERIN_HH
# define ELLE_SERIALIZATION_JSON_SERIALIZERIN_HH

# include <elle/json/json.hh>
# include <elle/serialization/SerializerIn.hh>

namespace elle
{
  namespace serialization
  {
    namespace json
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
        SerializerIn(std::istream& input);

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
        _serialize_option(std::string const& name,
                          bool,
                          std::function<void ()> const& f);
        virtual
        void
        _serialize_array(std::string const& name,
                         std::function<void ()> const& f) override;
        virtual
        void
        _enter(std::string const& name);
        virtual
        void
        _leave(std::string const& name);
        ELLE_ATTRIBUTE(boost::any, json);
        ELLE_ATTRIBUTE(std::vector<boost::any*>, current);

      private:
        template <typename T, typename ... Alternatives>
        T&
        _check_type(std::string const& name);
      };
    }
  }
}

#endif
