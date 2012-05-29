#ifndef  XXX_NODULESERIALIZER_HXX
# define XXX_NODULESERIALIZER_HXX

# include <cassert>

# include <XXX/HandleSerializer.hxx>

# include <XXX/Nodule.hh>

ELLE_SERIALIZE_SIMPLE(XXX::Nodule,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & this->type;
  archive & this->parent;
  archive & this->left;
  archive & this->right;
}

#endif
