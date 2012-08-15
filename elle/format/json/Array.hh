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

        //
        // Construction.
        //
      public:
        /// Default constructor makes an empty array.
        Array();

        /// Build an array from any kind of sequence. Contained type has to be
        /// convertible to a json object.
        template <typename Container>
        Array(Container const& container);

        /// An array of json object pointers can be moved.
        Array(internal::Array&& value);

        /// dtor.
        ~Array();

        //
        // Behave like an std::vector.
        //
      public:
        /// Add an arbitrary value.
        template <typename T>
        inline
        void
        push_back(T const& value);

        /// Move a json object pointer.
        void
        push_back(std::unique_ptr<Object>&& value);

        /// Indexable.
        /// @throws std::range_error.
        inline
        Object&
        operator [](size_t index) const;

        /// Size of the array.
        inline
        size_t
        size() const;

        // Representation.
      public:
        using Object::repr;
        virtual
        void
        repr(std::ostream& out) const;

        // Clonable.
      public:
        std::unique_ptr<Object>
        clone() const;

        // Comparable.
      public:
        using Object::operator ==;
        virtual
        bool
        operator ==(Array const& other) const;
        virtual
        bool
        operator ==(Object const& other) const;

        // Convertible to std::list and std::vector.
      public:
        template<typename T>
        operator std::vector<T>() const;
        template<typename T>
        operator std::list<T>() const;
      };

    }
  }
} // !namespace elle::format::json

# include "Array.hxx"

#endif /* ! ARRAY_HH */
