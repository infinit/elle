#ifndef ELLE_FORMAT_JSON_ARRAY_HH
# define ELLE_FORMAT_JSON_ARRAY_HH

# include <vector>

# include "Object.hh"
# include "Bool.hh"
# include "Integer.hh"
# include "Float.hh"
# include "String.hh"
# include "Dictionary.hh"
# include "Null.hh"

# include "_detail.hh"

namespace elle { namespace format { namespace json {

    class Array : public Object
    {
    private:
      std::vector<Object*> _value;

    public:
      Array() {}

      template<typename Container>
      Array(Container const& container,
            typename std::enable_if<detail::IsArray<Container>::value, bool>::type = true)
      {
        auto it = container.begin(), end = container.end();
        for (; it != end; ++it)
        {
          this->push_back(*it);
        }
      }

      ~Array()
      {
        for (auto it = _value.begin(), end = _value.end(); it != end; ++it)
          delete (*it);
        _value.clear();
      }

      inline void push_back(Null const&)
        { _value.push_back(new Null); }

      inline void push_back(bool value)
        { _value.push_back(new Bool(value)); }

      inline void push_back(std::unique_ptr<Object>&& value)
        { _value.push_back(value.release()); }

      template<typename T> inline typename std::enable_if<
          std::is_integral<T>::value
      >::type push_back(T value);

      template<typename T> inline typename std::enable_if<
          std::is_floating_point<T>::value
      >::type push_back(T value);

      template<typename T> inline typename std::enable_if<
          detail::IsString<T>::value
      >::type push_back(T const& value);

      template<typename T> inline typename std::enable_if<
          detail::IsArray<T>::value
      >::type push_back(T const& value);

      template<typename T> inline typename std::enable_if<
          detail::IsStringMap<T>::value
      >::type push_back(T const& value);

      template<typename T> inline typename std::enable_if<
          std::is_base_of<Object, T>::value
      >::type push_back(T const& value);

      size_t size() const { return _value.size(); }

    protected:
      void Save(elle::serialize::OutputJSONArchive& ar) const;
      std::unique_ptr<Object> Clone() const;
    };

}}} // !namespace elle::format::json

#endif /* ! ARRAY_HH */


