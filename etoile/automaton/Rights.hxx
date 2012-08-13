#ifndef ETOILE_AUTOMATON_RIGHTS_HXX
# define ETOILE_AUTOMATON_RIGHTS_HXX

# include <etoile/gear/Operation.hh>

# include <elle/standalone/Report.hh>

# include <elle/idiom/Open.hh>

namespace etoile
{
  namespace automaton
  {

    ///
    /// this method is triggered whenever the user's rights need to be
    /// recomputed.
    ///
    template <typename T>
    elle::Status
    Rights::Recompute(T& context)
    {
      // reset the role in order to make sure the Determine() method
      // will carry on.
      context.rights.role = T::Role::RoleUnknown;

      if (Rights::Determine(context) == elle::Status::Error)
        escape("unable to determine the rights");

      return elle::Status::Ok;
    }

    ///
    /// this method checks whether the user has the necessary permissions
    /// and eventually the required role to perform the given operation.
    ///
    template <typename T>
    elle::Status
    Rights::Operate(T& context,
                    const gear::Operation& operation)
    {
      // depending on the operation.
      switch (operation)
        {
        case gear::OperationUnknown:
          {
            escape("unable to check the rights for a unknown operation");
          }
        case gear::OperationDiscard:
          {
            //
            // nothing to check since discarding does not require
            // special privileges.
            //

            break;
          }
        case gear::OperationStore:
          {
            //
            // in this case, the user must have had the permission to update
            // the context.
            //
            // however, the permission checking process would have been
            // performed once the modifying operation such as Add() for
            // a directory would have been invoked.
            //
            // therefore, no special check is performed here.
            //

            break;
          }
        case gear::OperationDestroy:
          {
            //
            // in this case, the user must be the context's owner in order
            // to destroy it.
            //

            // determine the user's rights on this context.
            if (Rights::Determine(context) == elle::Status::Error)
              escape("unable to determine the rights");

            // check if the current user has the given role.
            if (context.rights.role != T::Role::RoleOwner)
              escape("the user does not seem to have the permission to "
                     "perform the requested operation");

            break;
          }
        }

      return elle::Status::Ok;
    }

  }
}

#endif
