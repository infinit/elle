#ifndef ELLE_FORMAT_JSON_ARRAY_HH
# define ELLE_FORMAT_JSON_ARRAY_HH

# include <cassert>
# include <list>
# include <vector>

# include "Object.hh"
# include "_internal.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      class Array:
        public Object
      {
      private:
        internal::Array _value;

      public:
        Array();
        template <typename Container>
        Array(Container const& container);
        Array(internal::Array&& value);
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
        clone() const;

        using Object::repr;
        virtual
        void
        repr(std::ostream& out) const;

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
        operator std::vector<T>() const;
        template<typename T>
        operator std::list<T>() const;
      };

    }
  }
} // !namespace elle::format::json

#include "Array.hxx"

#endif /* ! ARRAY_HH */
