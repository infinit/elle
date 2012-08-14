#ifndef ELLE_FORMAT_JSON_ARRAY_HH
# define ELLE_FORMAT_JSON_ARRAY_HH

# include <cassert>
# include <vector>
# include <list>

# include "Object.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      class Null;

      class Array:
        public Object
      {
      private:
        std::vector<Object*> _value;

      public:
        Array();
        template <typename Container>
        Array(Container const& container);
        ~Array();

      public:
        template <typename T>
        inline
        void
        push_back(T const& value);

        void
        push_back(std::unique_ptr<Object>&& value);

      public:
        inline
        size_t
        size() const;

        std::unique_ptr<Object>
        Clone() const;

      protected:
        void
        Save(elle::serialize::OutputJSONArchive& ar) const;

      public:
        using Object::operator ==;

        inline
        Object&
        operator[] (size_t index) const;

        virtual
        bool
        operator ==(Object const& other) const;

        virtual
        bool
        operator ==(Array const& other) const;

        template<typename T>
        operator std::list<T>() const;
      };

    }
  }
} // !namespace elle::format::json

#include "Array.hxx"

#endif /* ! ARRAY_HH */
