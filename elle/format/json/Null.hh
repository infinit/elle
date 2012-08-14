#ifndef  ELLE_FORMAT_JSON_NULL_HH
# define ELLE_FORMAT_JSON_NULL_HH

# include "Object.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      class Null:
        public Object
      {
      public:
        Null();
        virtual ~Null();

      public:
        using Object::operator ==;
        virtual bool operator ==(Object const& other) const;
        virtual bool operator ==(Null const&) const;

      protected:
        void
        Save(elle::serialize::OutputJSONArchive& ar) const;
        std::unique_ptr<Object>
        Clone() const;
      };

      extern Null const null;

    }
  }
} // !namespace elle::format::json

#endif
