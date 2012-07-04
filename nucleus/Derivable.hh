#ifndef  NUCLEUS_DERIVABLE_HH
# define NUCLEUS_DERIVABLE_HH

# include <nucleus/neutron/Component.hh>
# include <nucleus/proton/Block.hh>

# include <elle/idiom/Close.hh>
#  include <boost/noncopyable.hpp>
# include <elle/idiom/Open.hh>

namespace nucleus
{

  ///
  /// this class provides a nucleus-specific derivable class i.e a
  /// class for generating nucleus objects based on a component.
  ///
  // XXX This class is temporary.
  // Note: Derivable is not derivable
  class Derivable
    : private boost::noncopyable
  {
  public:
    enum class Kind { none, input, output };
    Kind const kind; // XXX ignored most of the time

  private:
    nucleus::neutron::Component   _component;
    nucleus::proton::Block*       _block;
    bool                          _dynamic_construct;

  public:
    Derivable(nucleus::neutron::Component component,
              nucleus::proton::Block const& block,
              Kind kind = Kind::input)
      : kind(kind)
      , _component(component)
      , _block(const_cast<nucleus::proton::Block*>(&block))
      , _dynamic_construct(false)
    {}

    Derivable()
      : kind(Kind::output)
      , _component(nucleus::neutron::ComponentUnknown)
      , _block(nullptr)
      , _dynamic_construct(true)
    {}

    Derivable(Derivable&& source)
      : kind(source.kind)
      , _component(source._component)
      , _block(source._block)
      , _dynamic_construct(source._dynamic_construct)
    {
      source._block = nullptr;
    }

    ~Derivable()
    {
      if (this->_dynamic_construct)
        {
          delete this->_block;
          this->_block = nullptr;
        }
    }

    std::unique_ptr<nucleus::proton::Block> release()
    {
      assert(this->kind == Kind::output);
      assert(this->_dynamic_construct);
      auto tmp = this->_block;
      this->_block = nullptr;
      return std::unique_ptr<nucleus::proton::Block>(tmp);
    }

    nucleus::proton::Block const& block() const
    {
      if (this->_block == nullptr)
        throw std::runtime_error("No block in this derivable !");
      return *this->_block;
    }

    ELLE_SERIALIZE_FRIEND_FOR(Derivable);
  };

}

# include <nucleus/Derivable.hxx>

#endif
