#ifndef ELLE_CAST_HH
# define ELLE_CAST_HH

# include <elle/printf.hh>

namespace elle
{
  template <typename T>
  struct cast
  {

    // XXX move should be explicit, this specialization should not be used.
    template <typename U>
    static std::unique_ptr<T>
    runtime(std::unique_ptr<U>& p)
    {
      T* res = dynamic_cast<T*>(p.get());
      if (!res)
        return nullptr;
      p.release();
      return std::unique_ptr<T>(static_cast<T*>(res));
    }

    template <typename U>
    static std::unique_ptr<T>
    runtime(std::unique_ptr<U>&& p)
    {
      T* res = dynamic_cast<T*>(p.get());
      if (!res)
        return nullptr;
      p.release();
      return std::unique_ptr<T>(static_cast<T*>(res));
    }

    // template <typename U>
    // static T*
    // static_(U* p)
    // {
    //   assert(dynamic_cast<T*>(u));
    //   return static_cast<T*>(p);
    // }

    // template <typename U>
    // static std::unique_ptr<T>
    // static_(std::unique_ptr<U>&& p)
    // {
    //   assert(dynamic_cast<T*>(p.get()));
    //   return std::unique_ptr<T>(static_cast<T*>(p.release()));
    // }

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
