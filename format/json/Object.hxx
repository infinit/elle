#ifndef  ELLE_FORMAT_JSON_OBJECT_HXX
# define ELLE_FORMAT_JSON_OBJECT_HXX

# include <typeinfo>

# include "_detail.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      template <typename T>
      void
      Object::Load(T& out) const
      {
        static_assert(
            !std::is_base_of<T, Object>::value,
            "Cannot load into a json object"
        );
        typedef typename detail::SelectJSONType<T>::type SelfType;
        out = dynamic_cast<SelfType const&>(*this);
      }

      template <typename T>
      T
      Object::as() const
      {
        T val;
        this->Load(val);
        return val;
      }

      template <typename T>
      bool
      Object::TryLoad(T& out) const
      {
        typedef typename detail::SelectJSONType<T>::type SelfType;
        if (auto ptr = dynamic_cast<SelfType const*>(this))
          {
            out = *ptr;
            return true;
          }
        return false;
      }


      template <typename T>
      typename std::enable_if<
          !std::is_base_of<T, Object>::value
        , bool
      >::type
      Object::operator ==(T const& other) const
      {
        typedef typename detail::SelectJSONType<T>::type SelfType;
        if (auto self = dynamic_cast<SelfType const*>(this))
          return *self == SelfType(other);
        return false;
      }

      inline
      bool
      Object::operator !=(Object const& other) const
      {
        return !(*this == other);
      }

      inline
      bool
      Object::operator !=(Array const& other) const
      {
        return !(*this == other);
      }

      inline
      bool
      Object::operator !=(Bool const& other) const
      {
        return !(*this == other);
      }

      inline
      bool
      Object::operator !=(Dictionary const& other) const
      {
        return !(*this == other);
      }

      inline
      bool
      Object::operator !=(Float const& other) const
      {
        return !(*this == other);
      }

      inline
      bool
      Object::operator !=(Integer const& other) const
      {
        return !(*this == other);
      }

      inline
      bool
      Object::operator !=(Null const& other) const
      {
        return !(*this == other);
      }

      inline
      bool
      Object::operator !=(String const& other) const
      {
        return !(*this == other);
      }

      template <typename T>
      inline
      bool
      Object::operator !=(T const& other) const
      {
        return !(*this == other);
      }

    }
  }
} // !namespace elle::format::json

#endif
