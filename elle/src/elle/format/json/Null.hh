#ifndef  ELLE_FORMAT_JSON_NULL_HH
# define ELLE_FORMAT_JSON_NULL_HH

# include <elle/format/json/Object.hh>

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
        virtual
        bool
        operator ==(Object const& other) const;
        virtual
        bool
        operator ==(Null const&) const;

      public:
        using Object::repr;
        virtual
        void
        repr(std::ostream& out) const;

      protected:
        std::unique_ptr<Object>
        clone() const;
      };

      extern Null const null;
    }
  }
}

#endif
