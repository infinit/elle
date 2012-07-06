#ifndef ELLE_FORMAT_JSON_nullptr_HH
# define ELLE_FORMAT_JSON_nullptr_HH

# include "Object.hh"

namespace elle { namespace format { namespace json {

    class Null : public Object
    {
    protected:
      void Save(elle::serialize::OutputJSONArchive& ar) const;
      std::unique_ptr<Object> Clone() const
        { return std::unique_ptr<Object>(new Null); }
    public:
      using Object::operator ==;
      virtual bool operator ==(Object const& other) const
        { return other == *this; }
      virtual bool operator ==(Null const&) const
        { return true; }
    };

    extern Null const null;

}}} // !namespace elle::format::json

#endif /* ! nullptr_HH */


