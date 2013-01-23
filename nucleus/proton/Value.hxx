#ifndef NUCLEUS_PROTON_VALUE_HXX
# define NUCLEUS_PROTON_VALUE_HXX

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Value,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & base_class<nucleus::proton::Node>(value);
}

#endif
