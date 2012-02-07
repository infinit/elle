//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon jun 20 14:13:45 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/automaton/Author.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/automaton/Access.hh>

#include <agent/Agent.hh>

namespace etoile
{
  namespace automaton
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method forges the author objects require in order to prove
    /// the user had the permission to operate upon the object.
    ///
    elle::Status        Author::Forge(gear::Object&             context)
    {
      ;

      // if an author exists, return.
      if (context.author != nucleus::Author::Null)
        return elle::StatusOk;

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
        escape("unable to determine the rights");

      // build the author object according to the subject's role.
      switch (context.rights.role)
        {
        case nucleus::RoleOwner:
          {
            // create an owner author.
            if (context.author.Create() == elle::StatusError)
              escape("unable to create the author");

            break;
          }
        case nucleus::RoleLord:
          {
            nucleus::Index      index;

            // open the access.
            if (Access::Open(context) == elle::StatusError)
              escape("unable to open the access");

            // lookup the user's subject in the access records.
            if (context.access->Lookup(agent::Agent::Subject,
                                       index) == elle::StatusError)
              escape("unable to lookup the user's identity in the "
                     "access block");

            // create a lord author.
            if (context.author.Create(index) == elle::StatusError)
              escape("unable to create the author");

            break;
          }
        default:
          {
            // XXX to implement.

            break;
          }
        }

      return elle::StatusOk;
    }

  }
}
