#ifndef  ELLE_FORMAT_JSON_FACTORY_HXX
# define ELLE_FORMAT_JSON_FACTORY_HXX

namespace elle
{
  namespace format
  {
    namespace json
    {

        template <typename T>
        static
        _ELLE_FORMAT_JSON_FACTORY_ENABLE_BY_VALUE(T)
        Factory::construct(T const value)
        {
        }

        template <typename T>
        static
        _ELLE_FORMAT_JSON_FACTORY_ENABLE_BY_REF(T)
        Factory::construct(T const& value)
        {
        }

    }
  }
}


#endif

