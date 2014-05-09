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
        SerializerOut(std::ostream& output);
        ~SerializerOut() noexcept(false);

      /*--------------.
      | Serialization |
      `--------------*/
      protected:
        virtual
        void
        _enter(std::string const& name) override;
        virtual
        void
        _leave(std::string const& name) override;
        virtual
        void
        _serialize_array(std::string const& name,
                         std::function<void ()> const& f) override;
        virtual
        void
        _serialize(std::string const& name, int64_t& v) override;
        virtual
        void
        _serialize(std::string const& name, int32_t& v) override;
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
        _serialize_option(std::string const& name,
                          bool filled,
                          std::function<void ()> const& f);
      private:
        boost::any&
        _get_current();

      /*-----.
      | JSON |
      `-----*/
      private:
        ELLE_ATTRIBUTE(boost::any, json);
        ELLE_ATTRIBUTE(std::vector<boost::any*>, current);

      };
    }
  }
}

#endif
