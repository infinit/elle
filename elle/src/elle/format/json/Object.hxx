#ifndef  ELLE_FORMAT_JSON_OBJECT_HXX
# define ELLE_FORMAT_JSON_OBJECT_HXX

# include "_detail.hh"

# include <elle/types.hh>
# include <elle/Exception.hh>
# include <elle/serialize/fwd.hh>
# include <elle/serialize/extract.hh>
#include<iostream>
namespace elle
{
  namespace format
  {
    namespace json
    {
      namespace detail
      {
        // Generic loader for type T.
        template <typename TargetType>
        struct SelectLoader
        {
          template <typename T>
          static
          bool
          load(Object const* self, T& out)
          {
            if (auto ptr = dynamic_cast<TargetType const*>(self))
              out = static_cast<T>(*ptr);
            else
              return false;
            return true;
          }
        };

        template <>
        struct SelectLoader<String>
        {
          template <typename T>
          static
          bool
          load(Object const* self, T& out)
          {
            std::cerr << "out = " <<  out << std::endl;
            if (auto ptr = dynamic_cast<String const*>(self))
            {
              std::cerr << "REPR = " << ptr->repr() << std::endl;
              out = static_cast<T>(*ptr);
            }
            else
              return false;
            return true;
          }
        };

        // Compatibility with enums
        template <>
        struct SelectLoader<Integer>
        {
          template <typename T>
          static
          bool
          load(Object const* self, T& out)
          {
            if (auto ptr = dynamic_cast<Integer const*>(self))
              out = static_cast<T>(ptr->value());
            else
              return false;
            return true;
          }
        };

        // Convertibility from integer
        template <>
        struct SelectLoader<Float>
        {
          template <typename T>
          static
          bool
          load(Object const* self, T& out)
          {
            if (auto ptr = dynamic_cast<Float const*>(self))
              out = *ptr;
            else if (Integer const* ptr = dynamic_cast<Integer const*>(self))
              out = ptr->value();
            else
              return false;
            return true;
          }
        };

        // Load complex type.
        template <>
        struct SelectLoader<void>
        {
          template <typename T>
          static
          bool
          load(Object const* self, T& out)
          {
            using namespace elle::serialize;
            from_string<InputJSONArchive>(self->repr()) >> out;
            return true;
          }
        };
      } // !detail

      template <typename T>
      void
      Object::load(T& out) const
      {
        typedef typename detail::SelectJSONType<T>::type TargetType;
        if (!detail::SelectLoader<TargetType>::template load(this, out))
          throw elle::Exception{
            "cannot cast " + ELLE_PRETTY_OBJECT_TYPE(this) +
            " to " + ELLE_PRETTY_TYPE(T)
          };
      }

      template <typename T>
      T
      Object::as() const
      {
        T val;
        this->load(val);
        return val;
      }

      template <typename T>
      bool
      Object::try_load(T& out) const
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
