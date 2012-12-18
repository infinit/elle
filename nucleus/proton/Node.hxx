#ifndef NUCLEUS_PROTON_NODE_HXX
# define NUCLEUS_PROTON_NODE_HXX

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Node,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  // XXX nothing to serialize but we have to declare this shit!
  (void)archive;
  (void)value;
}

#endif
