#ifndef ELLE_FORMAT_JSON_ARRAY_HH
# define ELLE_FORMAT_JSON_ARRAY_HH

# include <vector>

# include "Object.hh"

# include "_detail.hh"

namespace elle { namespace format { namespace json {

    class Null;

    class Array : public Object
    {
    private:
      std::vector<Object*> _value;

    public:
      Array() {}

      template<typename Container>
      Array(Container const& container,
            typename std::enable_if<detail::IsArray<Container>::value, bool>::type = true);

      ~Array()
      {
        for (auto it = _value.begin(), end = _value.end(); it != end; ++it)
          delete (*it);
        _value.clear();
      }

      template<typename T> inline typename std::enable_if<
          std::is_same<T, Null>::value
      >::type push_back(T const&);

      template<typename T> inline typename std::enable_if<
          std::is_same<T, bool>::value
      >::type push_back(T value);

      inline void push_back(std::unique_ptr<Object>&& value)
        { _value.push_back(value.release()); }

      template<typename T> inline typename std::enable_if<
            std::is_integral<T>::value
        &&  !std::is_same<T , bool>::value
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
        &&  !std::is_same<T, Null>::value
      >::type push_back(T const& value);

      template<typename T> inline typename std::enable_if<
          std::is_pointer<T>::value && !std::is_array<T>::value
      >::type push_back(T const& value);

      size_t size() const { return _value.size(); }

    protected:
      void Save(elle::serialize::OutputJSONArchive& ar) const;
      std::unique_ptr<Object> Clone() const;
    };

}}} // !namespace elle::format::json

#endif /* ! ARRAY_HH */


