#ifndef NUCLEUS_PROTON_STATISTICS_HH
# define NUCLEUS_PROTON_STATISTICS_HH

# include <elle/types.hh>
# include <elle/io/Dumpable.hh>

# include <nucleus/proton/fwd.hh>

namespace nucleus
{
  namespace proton
  {

    /// XXX
    struct Statistics:
      public elle::io::Dumpable
    {
      //
      // construction
      //
    public:
      Statistics();

      //
      // attributes
      //
    public:
      struct
      {
        Footprint minimum;
        Footprint average;
        Footprint maximum;
      } footprint;

      struct
      {
        Capacity minimum;
        Capacity average;
        Capacity maximum;
      } capacity;

      struct
      {
        elle::Natural64 clean;
        elle::Natural64 dirty;
        elle::Natural64 consistent;
        elle::Natural64 all;
      } blocks;

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 margin = 0) const;
    };

  }
}

#endif
