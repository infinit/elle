//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Object.cc
//
// created       julien quintard   [fri aug 14 19:16:10 2009]
// updated       julien quintard   [sun aug 23 12:43:11 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Object.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Object::Load(context::Object&		context,
				     const hole::Address&	address)
    {
      // set the object address.
      context.address = address;

      // allocate the object.
      context.object = new core::Object;

      // get the block from Hole.
      if (hole::Hole::Get(address, *context.object) == StatusError)
	escape("unable to retrieve the block from Hole");

      // retrieve the subject's rights. that also means that, if the
      // subject is a consumer, the system should eventually be able to find
      // a delegate to vouch for her.
      //
      // note also that everytime the object's permissions are modified,
      // the rights should be re-computed.
      if (Object::Rights(context, context.rights) == StatusError)
	escape("unable to retrieve the rights");

      leave();
    }

    ///
    /// this method retrieve the permissions the given subject has over
    /// the object.
    ///
    /// \todo add a default argument for the subject being a user or group.
    ///
    Status		Object::Rights(context::Object&		context,
				       core::Permissions&	rights,
				       const core::Subject&	subject)
    {
      /* XXX
      // test if the subject is the owner.
      if ((subject.type == core::Subject::TypeUser) &&
	  (*subject.identifier.user == context.object.owner.K))
	{
	  // return the permissions.
	  rights = context.object.meta.owner.permissions;
	}
      else
	{
	  // XXX delegate or consumer
	  printf("XXX[NOT IMPLEMENTED YET]\n");
	}
      */
      leave();
    }

  }
}
