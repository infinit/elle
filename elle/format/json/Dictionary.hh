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
        /// error class thrown when reading an inexistant key
        class KeyError : public std::runtime_error
        {
        public:
          KeyError(std::string const& key)
            : std::runtime_error("KeyError: '" + key + "'")
          {}
        };

      private:
        internal::Dictionary _map;

      public:
        Dictionary() {}

        template<typename Container>
          Dictionary(Container const& container);

        Dictionary(internal::Dictionary&& value);

        ~Dictionary()
        {
          for (auto it = _map.begin(), end = _map.end(); it != end; ++it)
            delete it->second;
          _map.clear();
        }

        /// Set or update key/value pairs from other.
        void update(Dictionary const& other);

        ///
        /// Returns a JSON Object indexed by a string.
        /// The operator[] will throw a KeyError exception when the key
        /// is not found and:
        ///   - The dictionary itsef is const
        ///   - Or, if you try to read from the proxy object returned when the
        ///     dictionary is not const
        ///
        /// This operator also enables assignment into a dictionary with a hidden
        /// temporary proxy object that supports assignement, comparison and can
        /// cast to a JSON Object. That latter cast will throw if the key is not
        /// found, or not yet assigned.
        ///
        /// @see contains() to know is the key is really present
        ///
        inline _DictProxy operator [](std::string const& key);
        inline Object const& operator [](std::string const& key) const
        {
          auto it = _map.find(key);
          if (it == _map.end())
            throw KeyError(key);
          return *(it->second);
        }

        ///
        /// Check if the dictionary contains a key.
        /// While you might want to do something with the value
        /// if it exists, this function returns a pointer to it.
        ///
        Object const* contains(std::string const& key) const
          {
            auto it = _map.find(key);
            if (it != _map.end())
              return it->second;
            return nullptr;
          }

        size_t size() const { return _map.size(); }

        std::unique_ptr<Object> Clone() const;
      protected:
        void Save(OutputJSONArchive& ar) const;

      public:
        using Object::operator ==;

        virtual bool operator ==(Object const& other) const
          { return other == *this; }

        virtual bool operator ==(Dictionary const& other) const
          {
            if (this->size() != other.size())
              return false;
            auto it = _map.begin(),
                 end = _map.end();
            for (; it != end; ++it)
              {
                auto other_it = other._map.find(it->first);
                if (other_it == other._map.end() ||
                    *other_it->second != *it->second)
                  return false;
              }
            return true;
          }
      };

    }
  }
} // !namespace elle::format::json

#endif /* ! DICT_HH */


