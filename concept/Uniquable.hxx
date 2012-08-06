#ifndef  ELLE_CONCEPT_UNIQUABLE_HXX
# define ELLE_CONCEPT_UNIQUABLE_HXX

// XXX[two logs included?]
# include <elle/log.hh>
# include <elle/standalone/Log.hh>

# include <elle/standalone/Report.hh>

# include <elle/serialize/Base64Archive.hh>

# include <elle/idiom/Close.hh>
#  include <sstream>
# include <elle/idiom/Open.hh>

namespace elle
{
  namespace concept
  {

    template <__ECU_ARCHIVE_TPL(Archive)>
      Status Uniquable<Archive>::Save(elle::io::Unique& out) const
      {
        std::ostringstream ss;
        ELLE_LOG_COMPONENT("elle.concept");
        try
          {
            ELLE_TRACE_SCOPE("Save %p of type %s to a Unique string",
                                 this, ELLE_PRETTY_OBJECT_TYPE(this));
            typedef contract::_Serializable<Archive> interface_t;
            assert(dynamic_cast<interface_t const*>(this) != nullptr);
            assert(dynamic_cast<interface_t const*>(this) == static_cast<interface_t const*>(this));
            static_cast<interface_t const*>(this)->serialize(ss);
          }
        catch (std::exception const& err)
          {
            escape("%s", err.what());
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
            ELLE_LOG_COMPONENT("elle.concept");
            ELLE_TRACE_SCOPE("Load %p of type %s from a Unique string",
                                 this, ELLE_PRETTY_OBJECT_TYPE(this));
            typedef contract::_Serializable<Archive> interface_t;
            assert(dynamic_cast<interface_t*>(this) != nullptr);
            assert(dynamic_cast<interface_t const*>(this) == static_cast<interface_t const*>(this));
            static_cast<interface_t*>(this)->deserialize(ss);
          }
        catch (std::exception const& err)
          {
            escape("%s", err.what());
          }

        return Status::Ok;
      }

  }
}

# include <elle/idiom/Close.hh>

#endif

