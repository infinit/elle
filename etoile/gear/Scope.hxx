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

      // Return the context by dynamically casting it; this is required in order
      // to make sure nobody can perform file operations on a directory scope.
      if ((context = dynamic_cast<T*>(this->context)) == nullptr)
        throw std::runtime_error("unable to use this scope's context as such");

      return elle::Status::Ok;
    }

  }
}

#endif
