#ifndef NUCLEUS_PROTON_INLET_HH
# define NUCLEUS_PROTON_INLET_HH

# include <elle/types.hh>
# include <elle/attribute.hh>

# include <nucleus/proton/Footprint.hh>
# include <nucleus/proton/Handle.hh>
# include <nucleus/proton/State.hh>

# include <boost/noncopyable.hpp>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class represents a nodule item being for seams or quills.
    ///
    /// an inlet therefore embeds the item's key along with the address
    /// of the value whose type depends upon the nodule's nature. indeed,
    /// while seams' entries reference other nodules, i.e sub-seams or quills,
    /// quills' entries reference the final value.
    ///
    /// as said above, every inlet embeds the address of the value object.
    /// note however that should the inlet just be created (common case),
    /// the address would be null, hence empty. unfortunately, the address
    /// will eventually be computed. since the size of the address differs
    /// between its creation and its sealing, the footprint calculation is
    /// biaised and thus incorrect.
    ///
    /// in order to circumvent this issue, the initial empty address is set
    /// to Address::Some, whose role is to provide an address with a valid
    /// size.
    ///
    template <typename K>
    class Inlet:
      public elle::io::Dumpable
    {
    public:
      //
      // constructors & destructors
      //
    public:
      Inlet();
      Inlet(K const&,
            Handle const&);

      //
      // methods
      //
    public:
      /// XXX
      K const&
      key() const;
      /// XXX
      void
      key(K const&);
      /// XXX
      Handle&
      value();
      /// XXX
      Capacity const&
      capacity() const;
      /// XXX
      void
      capacity(Capacity const);
      /// XXX
      State const&
      state() const;
      /// XXX
      void
      state(State const);
      /// XXX
      Footprint const&
      footprint() const;
      /// XXX
      void
      footprint(Footprint const);

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_ASSIGNMENT(Inlet);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Inlet);

      //
      // attributes
      //
    private:
      K _key;
      Handle _value;
      Capacity _capacity;
      State _state;
      Footprint _footprint;
    };

  }
}

# include <nucleus/proton/Inlet.hxx>

#endif
