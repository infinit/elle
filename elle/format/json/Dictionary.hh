#ifndef ELLE_FORMAT_JSON_DICTIONARY_HH
# define ELLE_FORMAT_JSON_DICTIONARY_HH

# include <stdexcept>

# include "Object.hh"
# include "Null.hh"
# include "_internal.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      class Dictionary:
        public Object
      {
      private:
        /// Wrapper to set item into the map
        struct _DictProxy;
        friend struct _DictProxy;
      public:
        /// Error class thrown when reading an inexistant key.
        class KeyError;

      private:
        internal::Dictionary _map;

        ///
        /// Construction
        ///
      public:
        /// Empty dictionary.
        Dictionary();
        /// Dictionary from any map. The key type has to be convertible to a
        /// std::string, while the value type has to be convertible to a json
        /// object.
        template <typename Container>
        Dictionary(Container const& container);
        /// Move a map of json object pointers.
        Dictionary(internal::Dictionary&& value);
        /// dtor.
        ~Dictionary();

        /// Set or update key/value pairs from another dictionary.
        void update(Dictionary const& other);

        ///
        /// Returns a JSON Object indexed by a string.
        /// The operator[] will throw a KeyError exception when the key is not
        /// found and:
        ///   - The dictionary itsef is const
        ///   - Or, if you try to read from the proxy object returned when the
        ///     dictionary is not const
        ///
        /// This operator also enables assignment into a dictionary with a
        /// hidden temporary proxy object that supports assignement, comparison
        /// and can cast to a JSON Object. That latter cast will throw if the
        /// key is not found, or not yet assigned.
        ///
        /// @see contains() to know if the key is really present.
        ///
        _DictProxy
        operator [](std::string const& key);
        Object const&
        operator [](std::string const& key) const;

        ///
        /// Check if the dictionary contains a key.
        /// As you might want to do something with the value if it exists, this
        /// function returns a pointer to it.
        ///
        Object const*
        contains(std::string const& key) const;

        /// Returns the number of key/value pair.
        inline
        size_t
        size() const;

        /// Duplicate the current the Dictionary.
        virtual
        std::unique_ptr<Object>
        clone() const;

        using Object::repr;
        virtual
        void
        repr(std::ostream& out) const;

        /// Comparable implementation.
      public:
        using Object::operator ==;
        virtual
        bool
        operator ==(Object const& other) const;
        virtual
        bool
        operator ==(Dictionary const& other) const;
      };

    }
  }
} // !namespace elle::format::json

# include "Dictionary.hxx"

#endif /* ! DICT_HH */
