#ifndef ETOILE_PATH_SLICE_HH
# define ETOILE_PATH_SLICE_HH

# include <elle/types.hh>

namespace etoile
{
  namespace path
  {

    ///
    /// this type represents the name of a file system path item.
    ///
    /// for instance, the path /teton/avale/suce/ is composed of
    /// three slices: 'teton', 'avale' and 'suce'.
    ///
    typedef elle::String        Slice;

  }
}

#endif
