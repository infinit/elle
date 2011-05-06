//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Index.hh
//
// created       julien quintard   [thu apr  1 22:36:01 2010]
// updated       julien quintard   [wed may  4 23:22:04 2011]
//

#ifndef NUCLEUS_NEUTRON_INDEX_HH
#define NUCLEUS_NEUTRON_INDEX_HH

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Size.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this type is an alias of the size and has been introduced
    /// for clarity when representing indexes.
    ///
    typedef Size		Index;

//
// ---------- constants -------------------------------------------------------
//

    ///
    /// this constants defines the first and last indexes.
    ///
    const Index			IndexFirst = elle::Type<Index>::Minimum;
    const Index			IndexLast = elle::Type<Index>::Maximum;

  }
}

#endif
