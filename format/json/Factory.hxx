#ifndef  ELLE_FORMAT_JSON_FACTORY_HXX
# define ELLE_FORMAT_JSON_FACTORY_HXX

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
            return std::unique_ptr<Null>(new Null);
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
            return std::unique_ptr<Bool>(new Bool(value));
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
            return std::unique_ptr<Integer>(new Integer(value));
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
            return std::unique_ptr<Float>(new Float(value));
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
            return std::unique_ptr<String>(new String(value));
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
            return std::unique_ptr<Array>(new Array(value));
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
            return std::unique_ptr<Dictionary>(new Dictionary(value));
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

