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

    // XXX[???]
    // template <typename U>
    // static T*
    // static_(U* p)
    // {
    //   assert(dynamic_cast<T*>(u));
    //   return static_cast<T*>(p);
    // }

    // XXX[???]
    // template <typename U>
    // static std::unique_ptr<T>
    // static_(std::unique_ptr<U>&& p)
    // {
    //   assert(dynamic_cast<T*>(p.get()));
    //   return std::unique_ptr<T>(static_cast<T*>(p.release()));
    // }

    // XXX[???]
    // template <typename U>
    // static T*
    // static_(const std::unique_ptr<U>& p)
    // {
    //   assert(dynamic_cast<T*>(p.get()));
    //   static_cast<T*>(p.get());
    // }
  };
}

#endif
