#ifndef ELLE_CAST_HH
# define ELLE_CAST_HH

# include <elle/printf.hh>

namespace elle
{
  template <typename T>
  struct cast
  {
    /// Cast \a p to type \a T.
    ///
    /// If the cast is possible, the new pointer is returned and \a p is
    /// reseted, otherwise null is returned an \a p is left untouched.
    template <typename U>
    static std::unique_ptr<T>
    runtime(std::unique_ptr<U>& p)
    {
      T* res = dynamic_cast<T*>(p.get());
      if (!res)
        return nullptr;
      p.release();
      return std::unique_ptr<T>(res);
    }

    /// Cast \a p to type \a T.
    ///
    /// If the cast is possible, the new pointer is returned, otherwise null is
    /// returned.
    template <typename U>
    static std::unique_ptr<T>
    runtime(std::unique_ptr<U>&& p)
    {
      T* res = dynamic_cast<T*>(p.get());
      if (!res)
        return nullptr;
      p.release();
      return std::unique_ptr<T>(res);
    }

    /// Unconditionnaly cast \a p to type \a T.
    template <typename U>
    static std::unique_ptr<T>
    compiletime(std::unique_ptr<U>& p)
    {
      ELLE_ASSERT(dynamic_cast<T*>(p.get()));
      T* res = static_cast<T*>(p.get());
      p.release();
      return std::unique_ptr<T>(res);
    }

    /// Unconditionnaly cast \a p to type \a T.
    template <typename U>
    static std::unique_ptr<T>
    compiletime(std::unique_ptr<U>&& p)
    {
      ELLE_ASSERT(dynamic_cast<T*>(p.get()));
      T* res = static_cast<T*>(p.get());
      p.release();
      return std::unique_ptr<T>(res);
    }
  };
}

#endif
