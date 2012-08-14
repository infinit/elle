#ifndef  ELLE_FORMAT_JSON_FACTORY_HXX
# define ELLE_FORMAT_JSON_FACTORY_HXX

# include "Object.hh"
# include "_detail.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

        struct Factory::Impl
        {
          // Null
          template <typename T>
          static inline
          typename std::enable_if<
              std::is_same<T, Null>::value
            , std::unique_ptr<Null>
          >::type
          construct(T const&)
          {
            return Factory::construct_null();
          }

          // Bool
          template <typename T>
          static inline
          typename std::enable_if<
              std::is_same<T , bool>::value
            , std::unique_ptr<Bool>
          >::type
          construct(T value)
          {
            return Factory::construct_bool(value);
          }

          // Integer
          template <typename T>
          static inline
          typename std::enable_if<
                  std::is_integral<T>::value
              &&  !std::is_same<T , bool>::value
            , std::unique_ptr<Integer>
          >::type
          construct(T value)
          {
            return Factory::construct_integer(value);
          }

          // Float
          template <typename T>
          static inline
          typename std::enable_if<
              std::is_floating_point<T>::value
            , std::unique_ptr<Float>
          >::type
          construct(T value)
          {
            return Factory::construct_float(value);
          }

          // String
          template <typename T>
          static inline
          typename std::enable_if<
              detail::IsString<T>::value
            , std::unique_ptr<String>
          >::type
          construct(T const& value)
          {
            return Factory::construct_string(value);
          }

          // Array
          template <typename T>
          static inline
          typename std::enable_if<
              detail::IsArray<T>::value
            , std::unique_ptr<Array>
          >::type
          construct(T const& value)
          {
            internal::Array objects;
            for (auto const& element : value)
              objects.push_back(Factory::construct(element).release());
            return Factory::construct_array(std::move(objects));
          }

          // Dictionary
          template <typename T>
          static inline
          typename std::enable_if<
              detail::IsStringMap<T>::value
            , std::unique_ptr<Dictionary>
          >::type
          construct(T const& value)
          {
            internal::Dictionary objects;
            for (auto const& pair : value)
              objects.insert(internal::Dictionary::value_type{
                  pair.first,
                  Factory::construct(pair.second).release()
              });
            return Factory::construct_dictionary(std::move(objects));
          }

          // A not null object
          template <typename T>
          static inline
          typename std::enable_if<
                  std::is_base_of<Object, T>::value
              &&  !std::is_same<T, Null>::value
            , std::unique_ptr<T>
          >::type
          construct(T const& value)
          {
            Object* ptr = value.Clone().release();
            assert(dynamic_cast<T*>(ptr) != nullptr);
            return std::unique_ptr<T>(static_cast<T*>(ptr));
          }

          // A compile time error for trying to construct from a pointer
          template <typename T>
          static inline
          typename std::enable_if<
                std::is_pointer<T>::value
            &&  !std::is_array<T>::value
          >::type
          construct(T const&)
          {
            static_assert(
              !std::is_pointer<T>::value,
              "You cannot build JSON Object from a pointer"
            );
          }
        };

        template <typename T>
        _ELLE_FORMAT_JSON_FACTORY_ENABLE_BY_VALUE(T)
        Factory::construct(T value)
        {
          return Impl::construct(value);
        }

        template <typename T>
        _ELLE_FORMAT_JSON_FACTORY_ENABLE_BY_REF(T)
        Factory::construct(T const& value)
        {
          return Impl::construct(value);
        }

    }
  }
} // !elle::json::format


#endif

