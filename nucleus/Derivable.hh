#ifndef  NUCLEUS_DERIVABLE_HH
# define NUCLEUS_DERIVABLE_HH

# include <nucleus/neutron/Component.hh>
# include <nucleus/proton/Block.hh>

namespace nucleus
{

  struct Derivable
  {
    enum class Kind { none, input, output };
    Kind const kind;
    Derivable(Kind kind) : kind(kind) {}
    Derivable() : kind(Kind::none) {}
  };

  ///
  /// this class provides a nucleus-specific derivable class i.e a
  /// class for generating nucleus objects based on a component.
  ///
  class InputDerivable
    : public Derivable
  {
  private:
    nucleus::neutron::Component   _component;
    nucleus::proton::Block const& _block;
  public:
    InputDerivable(nucleus::neutron::Component component,
                   nucleus::proton::Block const& block);

    nucleus::proton::Block const& block() const { return this->_block; }

    ELLE_SERIALIZE_FRIEND_FOR(InputDerivable);
  };

  class OutputDerivable
    : public Derivable
  {
  private:
    nucleus::neutron::Component _component;
    nucleus::proton::Block*     _block;
    bool                        _dynamic_construct;
  public:
    OutputDerivable(nucleus::neutron::Component component,
                    nucleus::proton::Block& block);
    OutputDerivable(nucleus::proton::Block& block);

    ELLE_SERIALIZE_FRIEND_FOR(OutputDerivable);
  };

}

# include <nucleus/Derivable.hxx>

#endif
