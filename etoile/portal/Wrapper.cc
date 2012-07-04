#include <etoile/portal/Wrapper.hh>

#include <etoile/wall/Access.hh>
#include <etoile/wall/Attributes.hh>
#include <etoile/wall/Directory.hh>

#include <nucleus/neutron/Index.hh>
#include <nucleus/neutron/Subject.hh>
#include <nucleus/neutron/Record.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/Trait.hh>
#include <nucleus/neutron/Entry.hh>

#include <elle/log.hh>
#include <elle/idiom/Open.hh>

ELLE_LOG_TRACE_COMPONENT("etoile.portal.Wrapper");

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
                          const gear::Identifier& identifier,
                          const nucleus::neutron::Subject& subject,
                          nucleus::neutron::Record& record)
    {
      nucleus::neutron::Record* r;

      ELLE_LOG_TRACE("Access::Lookup()");

      // forward the call to wall.
      if (wall::Access::Lookup(identifier,
                               subject,
                               r) == elle::Status::Error)
        escape("unable to lookup the given subject");

      // construct the record depending on the value of _r_.
      if (r != NULL)
        record = *r;
      else
        record = nucleus::neutron::Record::Null;

      return elle::Status::Ok;
    }

    ///
    /// this method wraps the wall::Access::Consult() method by
    /// translating the range of pointer-to-record into a local copy.
    ///
    elle::Status        Wrapper::Access::Consult(
                          const gear::Identifier&               identifier,
                          const nucleus::neutron::Index& index,
                          const nucleus::neutron::Size& size,
                          nucleus::neutron::Range<
                            nucleus::neutron::Record>& range)
    {
      nucleus::neutron::Range<nucleus::neutron::Record> r;

      ELLE_LOG_TRACE("Access::Consult()");

      // forward the call to wall.
      if (wall::Access::Consult(identifier,
                                index,
                                size,
                                r) == elle::Status::Error)
        escape("unable to consult the given object's access records");

      // copy the range, duplicating every record in it.
      range = r;

      return elle::Status::Ok;
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
                          nucleus::neutron::Trait& trait)
    {
      nucleus::neutron::Trait* t;

      ELLE_LOG_TRACE("Attributes::Get()");

      // forward the call to wall.
      if (wall::Attributes::Get(identifier,
                                name,
                                t) == elle::Status::Error)
        escape("unable to get the attribute");

      // construct the trait depending on the value of _t_.
      if (t != NULL)
        trait = *t;
      else
        trait = nucleus::neutron::Trait::Null;

      return elle::Status::Ok;
    }

    ///
    /// this method wraps the wall::Attribtues::Fetch() method by
    /// translating the set of pointer-to-trait into a local copy.
    ///
    elle::Status        Wrapper::Attributes::Fetch(
                          const gear::Identifier&               identifier,
                          nucleus::neutron::Range<
                            nucleus::neutron::Trait>& range)
    {
      nucleus::neutron::Range<nucleus::neutron::Trait> r;

      ELLE_LOG_TRACE("Attributes::Fetch()");

      // forward the call to wall.
      if (wall::Attributes::Fetch(identifier,
                                  r) == elle::Status::Error)
        escape("unable to fetch the given object's attributes");

      // copy the range, duplicating every trait in it.
      range = r;

      return elle::Status::Ok;
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
                          nucleus::neutron::Entry& entry)
    {
      nucleus::neutron::Entry* e;

      ELLE_LOG_TRACE("Directory::Lookup()");

      // forward the call to wall.
      if (wall::Directory::Lookup(identifier,
                                  slab,
                                  e) == elle::Status::Error)
        escape("unable to lookup the entry");

      // construct the entry depending on the value of _e_.
      if (e != NULL)
        entry = *e;
      else
        entry = nucleus::neutron::Entry::Null;

      return elle::Status::Ok;
    }

    ///
    /// this method wraps the wall::Directory::Consult() method by
    /// translating the set of pointer-to-entry into a local copy.
    ///
    elle::Status        Wrapper::Directory::Consult(
                          const gear::Identifier&               identifier,
                          const nucleus::neutron::Index& index,
                          const nucleus::neutron::Size& size,
                          nucleus::neutron::Range<
                            nucleus::neutron::Entry>& range)
    {
      nucleus::neutron::Range<nucleus::neutron::Entry> r;

      ELLE_LOG_TRACE("Directory::Consult()");

      // forward the call to wall.
      if (wall::Directory::Consult(identifier,
                                   index,
                                   size,
                                   r) == elle::Status::Error)
        escape("unable to consult the given object's directory entries");

      // copy the range, duplicating every entry in it.
      range = r;

      return elle::Status::Ok;
    }

  }
}
