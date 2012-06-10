//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon aug 17 12:19:13 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Object.hh>
#include <etoile/gear/Nature.hh>

#include <etoile/depot/Depot.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    Object::Object():
      Context(NatureObject),

      access(NULL)
    {
      // initialize the rights structure.
      this->rights.role = nucleus::RoleUnknown;
      this->rights.permissions = nucleus::PermissionNone;
    }

    ///
    /// the nature-specific constructor.
    ///
    Object::Object(const Nature                                 nature):
      Context(nature),

      access(NULL)
    {
      // initialize the rights structure.
      this->rights.role = nucleus::RoleUnknown;
      this->rights.permissions = nucleus::PermissionNone;
    }

    ///
    /// the destructor.
    ///
    Object::~Object()
    {
      // delete the access.
      if (this->access != NULL)
        delete this->access;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the object.
    ///
    elle::Status        Object::Dump(const elle::Natural32      margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Object]" << std::endl;

      // dump the parent context.
      if (Context::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the parent context");

      // dump the location.
      if (this->location.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the location");

      // dump the object.
      if (this->object.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the object");

      // dump the access, if present.
      if (this->access != NULL)
        {
          // dump the access block.
          if (this->access->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the access");
        }
      else
        {
          // dump none.
          std::cout << alignment << elle::Dumpable::Shift
                    << "[Access] " << elle::none << std::endl;
        }

      // dump the rights.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Rights]" << std::endl;

      // dump the role.
      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
                << "[Role] "
                << this->rights.role << std::endl;

      // dump the permissions.
      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
                << "[Permissions] "
                << (int)this->rights.permissions
                << std::endl;

      // dump the key.
      if (this->rights.key.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the key");

      // dump the record.
      if (this->rights.record.Dump(margin + 4) == elle::Status::Error)
        escape("unable to dump the record");

      // dump the author.
      if (this->author.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the author");

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the object.
    ///
    //elle::Status        Object::Serialize(elle::Archive&        archive) const
    //{
    //  // serialize the attributes.
    //  if (archive.Serialize(this->location,
    //                        this->object) == elle::Status::Error)
    //    escape("unable to serialize the attributes");

    //  // serialize the access.
    //  if (this->access == NULL) // <<< oups, fonction jamais testée ?
    //    {
    //      // serialize the access.
    //      if (archive.Serialize(*this->access) == elle::Status::Error)
    //        escape("unable to serialize the access");
    //    }
    //  else
    //    {
    //      // serialize 'none'.
    //      if (archive.Serialize(elle::none) == elle::Status::Error)
    //        escape("unable to serialize 'none'");
    //    }

    //  // serialize the attributes.
    //  if (archive.Serialize(
    //        static_cast<elle::Natural8>(this->rights.role),
    //        static_cast<elle::Natural8>(this->rights.permissions),
    //        this->rights.key,
    //        this->rights.record,
    //        this->author) == elle::Status::Error)
    //    escape("unable to serialize the attributes");

    //  return elle::Status::Ok;
    //}

    /////
    ///// this method extracts the object.
    /////
    //elle::Status        Object::Extract(elle::Archive&          archive)
    //{
    //  elle::Archive::Type       type;

    //  // extract the attributes.
    //  if (archive.Extract(this->location,
    //                      this->object) == elle::Status::Error)
    //    escape("unable to extract the attributes");

    //  // fetch the next element's type.
    //  if (archive.Fetch(type) == elle::Status::Error)
    //    escape("unable to fetch the next element's type");

    //  // extract the access.
    //  if (type == elle::Archive::TypeNull)
    //    {
    //      // extract 'none'.
    //      if (archive.Extract(elle::none) == elle::Status::Error)
    //        escape("unable to extract 'none'");
    //    }
    //  else
    //    {
    //      // allocate an access.
    //      this->access = new nucleus::Access;

    //      // extract the access.
    //      if (archive.Extract(*this->access) == elle::Status::Error)
    //        escape("unable to extract the access");
    //    }

    //  // extract the attributes.
    //  if (archive.Extract(
    //        reinterpret_cast<elle::Natural8&>(this->rights.role),
    //        reinterpret_cast<elle::Natural8&>(this->rights.permissions),
    //        this->rights.key,
    //        this->rights.record,
    //        this->author) == elle::Status::Error)
    //    escape("unable to extract the attributes");

    //  return elle::Status::Ok;
    //}

  }
}
