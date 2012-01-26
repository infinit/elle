//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [wed nov  2 09:14:37 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/portal/Wrapper.hh>

#include <etoile/wall/Access.hh>
#include <etoile/wall/Attributes.hh>
#include <etoile/wall/Directory.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace portal
  {

//
// ---------- access ----------------------------------------------------------
//

    ///
    /// this method wraps the wall::Access::Lookup() method by
    /// translating the pointer-to-record into a local copy.
    ///
    elle::Status        Wrapper::Access::Lookup(
                          const gear::Identifier&               identifier,
                          const nucleus::Subject&               subject,
                          nucleus::Record&                      record)
    {
      nucleus::Record*  r;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] portal::Wrapper:Access::Lookup()\n");

      // forward the call to wall.
      if (wall::Access::Lookup(identifier,
                               subject,
                               r) == elle::StatusError)
        escape("unable to lookup the given subject");

      // construct the record depending on the value of _r_.
      if (r != NULL)
        record = *r;
      else
        record = nucleus::Record::Null;

      leave();
    }

    ///
    /// this method wraps the wall::Access::Consult() method by
    /// translating the range of pointer-to-record into a local copy.
    ///
    elle::Status        Wrapper::Access::Consult(
                          const gear::Identifier&               identifier,
                          const nucleus::Index&                 index,
                          const nucleus::Size&                  size,
                          nucleus::Range<nucleus::Record>&      range)
    {
      nucleus::Range<nucleus::Record>   r;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] portal::Wrapper:Access::Consult()\n");

      // forward the call to wall.
      if (wall::Access::Consult(identifier,
                                index,
                                size,
                                r) == elle::StatusError)
        escape("unable to consult the given object's access records");

      // copy the range, duplicating every record in it.
      range = r;

      leave();
    }

//
// ---------- attributes ------------------------------------------------------
//

    ///
    /// this method wraps the wall::Attributes::Get() method by
    /// translating the pointer-to-trait into a local copy.
    ///
    elle::Status        Wrapper::Attributes::Get(
                          const gear::Identifier&               identifier,
                          const elle::String&                   name,
                          nucleus::Trait&                       trait)
    {
      nucleus::Trait*   t;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] portal::Wrapper:Attributes::Get()\n");

      // forward the call to wall.
      if (wall::Attributes::Get(identifier,
                                name,
                                t) == elle::StatusError)
        escape("unable to get the attribute");

      // construct the trait depending on the value of _t_.
      if (t != NULL)
        trait = *t;
      else
        trait = nucleus::Trait::Null;

      leave();
    }

    ///
    /// this method wraps the wall::Attribtues::Fetch() method by
    /// translating the set of pointer-to-trait into a local copy.
    ///
    elle::Status        Wrapper::Attributes::Fetch(
                          const gear::Identifier&               identifier,
                          nucleus::Range<nucleus::Trait>&       range)
    {
      nucleus::Range<nucleus::Trait>    r;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] portal::Wrapper:Attributes::Fetch()\n");

      // forward the call to wall.
      if (wall::Attributes::Fetch(identifier,
                                  r) == elle::StatusError)
        escape("unable to fetch the given object's attributes");

      // copy the range, duplicating every trait in it.
      range = r;

      leave();
    }

//
// ---------- directory -------------------------------------------------------
//

    ///
    /// this method wraps the wall::Directory::Lookup() method by
    /// translating the pointer-to-entry into a local copy.
    ///
    elle::Status        Wrapper::Directory::Lookup(
                          const gear::Identifier&               identifier,
                          const path::Slab&                     slab,
                          nucleus::Entry&                       entry)
    {
      nucleus::Entry*   e;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] portal::Wrapper:Directory::Lookup()\n");

      // forward the call to wall.
      if (wall::Directory::Lookup(identifier,
                                  slab,
                                  e) == elle::StatusError)
        escape("unable to lookup the entry");

      // construct the entry depending on the value of _e_.
      if (e != NULL)
        entry = *e;
      else
        entry = nucleus::Entry::Null;

      leave();
    }

    ///
    /// this method wraps the wall::Directory::Consult() method by
    /// translating the set of pointer-to-entry into a local copy.
    ///
    elle::Status        Wrapper::Directory::Consult(
                          const gear::Identifier&               identifier,
                          const nucleus::Index&                 index,
                          const nucleus::Size&                  size,
                          nucleus::Range<nucleus::Entry>&       range)
    {
      nucleus::Range<nucleus::Entry>    r;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] portal::Wrapper:Directory::Consult()\n");

      // forward the call to wall.
      if (wall::Directory::Consult(identifier,
                                   index,
                                   size,
                                   r) == elle::StatusError)
        escape("unable to consult the given object's directory entries");

      // copy the range, duplicating every entry in it.
      range = r;

      leave();
    }

  }
}
