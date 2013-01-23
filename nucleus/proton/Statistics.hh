#ifndef NUCLEUS_PROTON_STATISTICS_HH
# define NUCLEUS_PROTON_STATISTICS_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/io/Dumpable.hh>

# include <nucleus/proton/fwd.hh>

namespace nucleus
{
  namespace proton
  {
    /// Represent the statistics gathered on a porcupine data structure.
    struct Statistics:
      public elle::io::Dumpable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Statistics();

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 margin = 0) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_RW(Footprint, footprint_minimum);
      ELLE_ATTRIBUTE_RW(Footprint, footprint_average);
      ELLE_ATTRIBUTE_RW(Footprint, footprint_maximum);
      ELLE_ATTRIBUTE_RW(Capacity, capacity_minimum);
      ELLE_ATTRIBUTE_RW(Capacity, capacity_average);
      ELLE_ATTRIBUTE_RW(Capacity, capacity_maximum);
      ELLE_ATTRIBUTE_RW(elle::Size, blocks_clean);
      ELLE_ATTRIBUTE_RW(elle::Size, blocks_dirty);
      ELLE_ATTRIBUTE_RW(elle::Size, blocks_consistent);
      ELLE_ATTRIBUTE_RW(elle::Size, blocks_all);
    };
  }
}

#endif
