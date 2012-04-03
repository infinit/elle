#ifndef ELLE_FORMAT_JSON_FLOAT_HH
# define ELLE_FORMAT_JSON_FLOAT_HH

# include "Object.hh"

namespace elle { namespace format { namespace json {

    struct Float : public Object
    {
      friend class elle::serialize::OutputJSONArchive;
    public:
      typedef double Type;
    private:
      Type _value;

    public:
      Float(Type value) : _value(value) {}

      Type value() const { return _value; }
      Type& value()      { return _value; }

    protected:
      void Save(elle::serialize::OutputJSONArchive& ar) const;
      std::unique_ptr<Object> Clone() const
        { return std::unique_ptr<Object>(new Float(_value)); }
    };

}}} // !namespace elle::format::json

#endif /* ! FLOAT_HH */


