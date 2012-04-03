#ifndef ELLE_FORMAT_JSON_STRING_HH
# define ELLE_FORMAT_JSON_STRING_HH

# include <string>

# include "Object.hh"

namespace elle { namespace format { namespace json {

    struct String : public Object
    {
      friend class OutputJSONArchive;
    private:
      std::string _value;

    public:
      String(std::string const& value) : _value(value) {}

      std::string const& value() const  { return _value; }
      std::string& value()              { return _value; }

    protected:
      void Save(elle::serialize::OutputJSONArchive& ar) const;
      std::unique_ptr<Object> Clone() const
        { return std::unique_ptr<Object>(new String(_value)); }
    };

}}} // !namespace elle::format::json

#endif /* ! STRING_HH */


