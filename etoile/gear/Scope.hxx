#ifndef ETOILE_GEAR_SCOPE_HXX
# define ETOILE_GEAR_SCOPE_HXX

# include <etoile/gear/Context.hh>

# include <elle/idiom/Open.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns the context associated with the scope
    /// by casting it with the given type.
    ///
    template <typename T>
    elle::Status        Scope::Use(T*&                          context)
    {
      // first, if the scope's context is null, allocate one.
      if (this->context == nullptr)
        {
          // allocate a context according to the nature.
          this->context = new T;
        }

      // return the context by dynamically casting it.
      context = static_cast<T*>(this->context);
      assert(dynamic_cast<T*>(this->context) != nullptr);

      return elle::Status::Ok;
    }

  }
}

#endif
