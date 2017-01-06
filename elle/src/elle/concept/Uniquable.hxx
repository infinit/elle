#include <sstream>

#include <elle/assert.hh>
#include <elle/log.hh>
#include <elle/serialize/Base64Archive.hh>

namespace elle
{
  namespace concept
  {
    template <__ECU_ARCHIVE_TPL(Archive)>
    Status Uniquable<Archive>::Save(elle::io::Unique& out) const
    {
      std::ostringstream ss;
      ELLE_LOG_COMPONENT("elle.concept.Uniquable");
      try
      {
        ELLE_DEBUG_SCOPE("save %p of type %s to a Unique string",
                         this, ELLE_PRETTY_OBJECT_TYPE(this));
        using interface_t = serialize::_Serializable<Archive>;
        ELLE_ASSERT(dynamic_cast<interface_t const*>(this) != nullptr);
        ELLE_ASSERT(dynamic_cast<interface_t const*>(this)
                    == static_cast<interface_t const*>(this));
        static_cast<interface_t const*>(this)->serialize(ss);
      }
      catch (std::exception const& err)
      {
        elle::err("failed to save: %s", err.what());
      }

      out = ss.str();
      return Status::Ok;
    }

    template <__ECU_ARCHIVE_TPL(Archive)>
    Status
    Uniquable<Archive>::Restore(elle::io::Unique const& in)
    {
      std::istringstream ss(in);
      try
      {
        ELLE_LOG_COMPONENT("elle.concept.Uniquable");
        ELLE_DEBUG_SCOPE("load %p of type %s from a Unique string",
                             this, ELLE_PRETTY_OBJECT_TYPE(this));
        using interface_t = serialize::_Serializable<Archive>;
        ELLE_ASSERT(dynamic_cast<interface_t*>(this) != nullptr);
        ELLE_ASSERT(dynamic_cast<interface_t const*>(this)
                    == static_cast<interface_t const*>(this));
        static_cast<interface_t*>(this)->deserialize(ss);
      }
      catch (std::exception const& err)
      {
        elle::err("failed to restore: %s", err.what());
      }

      return Status::Ok;
    }
  }
}
