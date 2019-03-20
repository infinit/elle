#pragma once

#include <elle/TypeInfo.hh>
#include <elle/err.hh>

namespace elle
{
  template <typename T>
  struct cast
  {
    /// Cast \a p to type \a T.
    template <typename U>
    static T*
    runtime(U* p)
    {
      return dynamic_cast<T*>(p);
    }

    /// Cast \a p to type \a T.
    ///
    /// \throws elle::Error if the cast is not possible.
    template <typename U>
    static T&
    runtime(U& p)
    {
      if (auto res = runtime(&p))
        return *res;
      else
        elle::err("{} is not of type {}",
                  elle::type_info(p), elle::type_info<T>());
    }

    /// Cast \a p to type \a T.
    ///
    /// If the cast is possible, the new pointer is returned and \a p is
    /// reseted, otherwise null is returned an \a p is left untouched.
    template <typename U>
    static std::unique_ptr<T>
    runtime(std::unique_ptr<U>& p)
    {
      if (T* res = runtime(p.get()))
      {
        p.release();
        return std::unique_ptr<T>(res);
      }
      else
        return nullptr;
    }

    /// Cast \a p to type \a T.
    ///
    /// If the cast is possible, the new pointer is returned, otherwise null is
    /// returned.
    template <typename U>
    static std::unique_ptr<T>
    runtime(std::unique_ptr<U>&& p)
    {
      return runtime(p);
    }

    /// Unconditionnaly cast \a p to type \a T.
    template <typename U>
    static T*
    compiletime(U* p)
    {
      if constexpr(std::is_polymorphic_v<U>)
        ELLE_ASSERT(dynamic_cast<T*>(p));
      return static_cast<T*>(p);
    }

    /// Unconditionnaly cast \a p to type \a T.
    ///
    /// \throws elle::Error if the cast is not possible.
    template <typename U>
    static T&
    compiletime(U& p)
    {
      return *compiletime(&p);
    }

    /// Unconditionnaly cast \a p to type \a T.
    template <typename U>
    static std::unique_ptr<T>
    compiletime(std::unique_ptr<U>& p)
    {
      T* res = compiletime(p.get());
      p.release();
      return std::unique_ptr<T>(res);
    }

    /// Unconditionnaly cast \a p to type \a T.
    template <typename U>
    static std::unique_ptr<T>
    compiletime(std::unique_ptr<U>&& p)
    {
      T* res = compiletime(p.get());
      p.release();
      return std::unique_ptr<T>(res);
    }
  };
}
