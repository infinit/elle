#ifndef ELLE_FORMAT_JSON_INT_HH
# define ELLE_FORMAT_JSON_INT_HH

# include "Object.hh"

namespace elle { namespace format { namespace json {

    struct Int : public Object
    {
      friend class OutputJSONArchive;
    public:
      typedef int32_t Type;
    private:
      Type _value;

    public:
      Int(Type value) : _value(value) {}

      int32_t value() const { return _value; }
      int32_t& value()      { return _value; }

    protected:
      void Save(elle::serialize::OutputJSONArchive& ar) const;
      std::unique_ptr<Object> Clone() const
        { return std::unique_ptr<Object>(new Int(_value)); }
    };

}}} // !namespace elle::format::json


#endif /* ! JSON_INT_HH */


