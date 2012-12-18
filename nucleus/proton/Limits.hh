#ifndef NUCLEUS_PROTON_LIMITS_HH
# define NUCLEUS_PROTON_LIMITS_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <nucleus/proton/Extent.hh>
# include <nucleus/proton/Contention.hh>
# include <nucleus/proton/Balancing.hh>

namespace nucleus
{
  namespace proton
  {
    namespace limits
    {
      /// XXX
      struct Porcupine:
        public elle::Printable
      {
        /*-----------.
        | Interfaces |
        `-----------*/
      public:
        // printable
        virtual
        void
        print(std::ostream& stream) const;
      };

      /// XXX
      struct Node:
        public elle::Printable
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        Node(Extent const extent,
             Contention const contention,
             Balancing const balancing);

        /*-----------.
        | Interfaces |
        `-----------*/
      public:
        // printable
        virtual
        void
        print(std::ostream& stream) const;

        /*-----------.
        | Attributes |
        `-----------*/
      private:
        ELLE_ATTRIBUTE_R(Extent, extent);
        ELLE_ATTRIBUTE_R(Contention, contention);
        ELLE_ATTRIBUTE_R(Balancing, balancing);
      };
    }

    /// Define the limits for every component of a block-based data structure
    /// i.e a porcupine.
    ///
    /// One should be careful regarding the limits used as the impact both
    /// on the performance and the storage consumption can be huge. Besides,
    /// one should be aware that some values may lead to undetermined behaviours
    /// (meaning not thorougly tested cases), especially extreme values such
    /// as very low extent and close to zero or one contention and balancing
    /// ratios.
    class Limits:
      public elle::Printable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Limits(limits::Porcupine const& porcupine,
             limits::Node const& nodule,
             limits::Node const& value);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(limits::Porcupine, porcupine);
      ELLE_ATTRIBUTE_R(limits::Node, nodule);
      ELLE_ATTRIBUTE_R(limits::Node, value);

      // XXX
    public:
      Extent
      extent() const { return this->_nodule.extent(); };
      elle::Real
      contention() const { return this->_nodule.contention(); }
      elle::Real
      balancing() const { return this->_nodule.balancing(); }
      // XXX
    };
  }
}

#endif
