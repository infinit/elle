#ifndef NUCLEUS_NEUTRON_OBJECT_HXX
# define NUCLEUS_NEUTRON_OBJECT_HXX

# include <cassert>
# include <stdexcept>

# include <elle/serialize/ArchiveSerializer.hxx>

namespace nucleus
{
  namespace neutron
  {
    namespace detail
    {

      // XXX it would be better to serialize Record class
      template<elle::serialize::ArchiveMode mode> struct SelectMethod
      { static void update(nucleus::neutron::Object&) {} };

      template<> struct SelectMethod<elle::serialize::ArchiveMode::Input>
      {
        static void update(nucleus::neutron::Object& value)
        {
          ELLE_LOG_TRACE_COMPONENT("infinit.nucleus.neutron.Object");

          ELLE_LOG_TRACE("Compute owner record")
            {
              // XXX[comme pour Imprint::owner::subject, calculer ce record
              //     que lorsqu'il est demande i.e dans le getter]
              if (value.meta.owner.record.Update(
                    value.owner_subject(),
                    value.meta.owner.permissions,
                    value.meta.owner.token) == elle::Status::Error)
                throw std::runtime_error("unable to create the owner access record");
            }
        }
      };

    }
  }
}


ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Object,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & static_cast<nucleus::proton::ImprintBlock&>(value);

  archive & value.author;

  archive & value.meta.owner.permissions;
  archive & value.meta.owner.token;
  archive & value.meta.genre;
  archive & value.meta.stamp;
  archive & value.meta.attributes;
  archive & value.meta.access;
  archive & value.meta.version;
  archive & value.meta.signature;

  archive & value.data.contents;
  archive & value.data.size;
  archive & value.data.stamp;
  archive & value.data.version;
  archive & value.data.signature;

  nucleus::neutron::detail::SelectMethod<Archive::mode>::update(value); // XXX
}

#endif
