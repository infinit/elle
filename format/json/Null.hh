#ifndef  ELLE_FORMAT_JSON_NULL_HH
# define ELLE_FORMAT_JSON_NULL_HH

# include "fwd.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      class Null
        : public Object
      {
      protected:
        void Save(elle::serialize::OutputJSONArchive& ar) const;
        std::unique_ptr<Object> Clone() const;

      public:
        using Object::operator ==;
        virtual bool operator ==(Object const& other) const;
        virtual bool operator ==(Null const&) const;
        Null();
        virtual ~Null();
      };

      extern Null const null;

    }
  }
} // !namespace elle::format::json

#endif
