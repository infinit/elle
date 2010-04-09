//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/core/Entity.cc
//
// created       julien quintard   [sat mar 20 00:46:21 2010]
// updated       julien quintard   [tue apr  6 16:25:45 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Entity.hh>

///
/// these includes are placed here in order to prevent pre-processing
/// conflicts.
///
#include <elle/miscellaneous/Report.hh>
#include <elle/miscellaneous/Maid.hh>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace core
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns the memory footprint of the actual object.
    ///
    /// this method has been introduced because there is no way to known
    /// the size of an object when its base class is manipulated.
    ///
    /// note that this method can be automatically generating by using
    /// the Embed(Entity, T) macro function.
    ///
    Status		Entity::Imprint(Natural32&) const
    {
      enter();

      escape("this method should never have been called");
    }

    ///
    /// this method clones the current object by allocating a new
    /// one through the copy constructor.
    ///
    Status		Entity::Clone(Entity*&) const
    {
      enter();

      escape("this method should never have been called");
    }

//
// ---------- operators -------------------------------------------------------
//

    ///
    /// this operator copies an entity.
    ///
    /// this method (i) starts by checking if the given entity
    /// is not the current one before (ii) calling Recycle().
    ///
    /// note that the Embed(Entity, T) macro function can be used
    /// to automatically embed the source code of this operator.
    ///
    Entity&		Entity::operator=(const Entity&)
    {
      fail("this method should never have been called");
    }

    ///
    /// this operator compares two entitys.
    ///
    Boolean		Entity::operator==(const Entity&) const
    {
      enter();

      flee("this method should never have been called");
    }

    ///
    /// this operator compares two entitys. the source code of this
    /// function can be automatically generated through the Embed(Entity, T)
    /// macro function.
    ///
    Boolean		Entity::operator!=(const Entity&) const
    {
      enter();

      flee("this method should never have been called");
    }

  }
}
