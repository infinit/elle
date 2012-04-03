#ifndef ELLE_JSON_BOOL_HH
# define ELLE_JSON_BOOL_HH

# include "Object.hh"

namespace elle { namespace format { namespace json {

    struct Bool : public Object
    {
    private:
      bool _value;

    public:
      Bool(bool value) : _value(value) {}

      bool value() const  { return _value; }
      bool& value()       { return _value; }

    private:
      void Save(elle::serialize::OutputJSONArchive& ar) const;
      std::unique_ptr<Object> Clone() const
        { return std::unique_ptr<Object>(new Bool(_value)); }
    };

}}} // !namespace elle::format::json

#endif /* ! BOOL_HH */


