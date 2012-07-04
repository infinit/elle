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
      if (this->context == NULL)
        {
          // allocate a context according to the nature.
          this->context = new T;
        }

      // return the context by dynamically casting it.
      if ((context = dynamic_cast<T*>(this->context)) == NULL)
        escape("invalid context nature: scope's(%u) target(%u)",
               this->context->nature, T::N);

      return elle::Status::Ok;
    }

  }
}

#endif
