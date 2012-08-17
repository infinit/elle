#include <etoile/portal/Wrapper.hh>

#include <etoile/wall/Access.hh>
#include <etoile/wall/Attributes.hh>
#include <etoile/wall/Directory.hh>
#include <etoile/wall/Group.hh>
#include <etoile/gear/Identifier.hh>

#include <nucleus/neutron/Index.hh>
#include <nucleus/neutron/Subject.hh>
#include <nucleus/neutron/Record.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/Trait.hh>
#include <nucleus/neutron/Entry.hh>
#include <nucleus/neutron/Fellow.hh>

#include <elle/log.hh>
#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("infinit.etoile.portal.Wrapper");

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
      nucleus::neutron::Record const* r;

      ELLE_TRACE_SCOPE("Access::Lookup()");

      // forward the call to wall.
      if (wall::Access::Lookup(identifier,
                               subject,
                               r) == elle::Status::Error)
        escape("unable to lookup the given subject");

      // construct the record depending on the value of _r_.
      if (r != nullptr)
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

      ELLE_TRACE_SCOPE("Access::Consult()");

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
      nucleus::neutron::Trait const* t;

      ELLE_TRACE_SCOPE("Attributes::Get()");

      // forward the call to wall.
      if (wall::Attributes::Get(identifier,
                                name,
                                t) == elle::Status::Error)
        escape("unable to get the attribute");

      // construct the trait depending on the value of _t_.
      if (t != nullptr)
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

      ELLE_TRACE_SCOPE("Attributes::Fetch()");

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
      nucleus::neutron::Entry const* e;

      ELLE_TRACE_SCOPE("Directory::Lookup()");

      // forward the call to wall.
      if (wall::Directory::Lookup(identifier,
                                  slab,
                                  e) == elle::Status::Error)
        escape("unable to lookup the entry");

      // construct the entry depending on the value of _e_.
      if (e != nullptr)
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

      ELLE_TRACE_SCOPE("Directory::Consult()");

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

//
// ---------- group -----------------------------------------------------------
//

    elle::Status
    Wrapper::Group::Lookup(gear::Identifier const& identifier,
                           nucleus::neutron::Subject const& subject,
                           nucleus::neutron::Fellow& fellow)
    {
      nucleus::neutron::Fellow const* f;

      ELLE_TRACE_SCOPE("Group::Lookup()");

      // forward the call to wall.
      if (wall::Group::Lookup(identifier,
                              subject,
                              f) == elle::Status::Error)
        escape("unable to lookup the fellow");

      // construct the fellow depending on the value of _f_.
      if (f != nullptr)
        fellow = *f;
      else
        fellow = nucleus::neutron::Fellow::Null;

      return elle::Status::Ok;
    }

    elle::Status
    Wrapper::Group::Consult(gear::Identifier const& identifier,
                            nucleus::neutron::Index const& index,
                            nucleus::neutron::Size const& size,
                            nucleus::neutron::Range<
                              nucleus::neutron::Fellow>& range)
    {
      nucleus::neutron::Range<nucleus::neutron::Fellow> r;

      ELLE_TRACE_SCOPE("Group::Consult(%s, %s, %s, %s)",
                           identifier, index, size, range);

      // forward the call to wall.
      if (wall::Group::Consult(identifier,
                               index,
                               size,
                               r) == elle::Status::Error)
        escape("unable to consult the given group fellows");

      // copy the range, duplicating every entry in it.
      range = r;

      return elle::Status::Ok;
    }

  }
}
