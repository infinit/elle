#ifndef INFINIT_REACTOR_SIGNAL_HH
# define INFINIT_REACTOR_SIGNAL_HH

# include <reactor/waitable.hh>

namespace reactor
{
  class Signal
    : public Waitable
  {
  public:
    using Self = reactor::Signal;
    using Super = reactor::Waitable;
    Signal(const std::string& name = std::string());
    virtual
    ~Signal();
    Signal(Signal&&) = default;
    bool signal();
    bool signal_one();

  /*----------.
  | Printable |
  `----------*/
  public:

    void
    print(std::ostream& stream) const override;
  };

  template <typename V>
  class VSignal
    : public Waitable
  {
    public:
      using Self = reactor::Signal;
      using Super = reactor::Waitable;
      VSignal(std::string const& name = std::string());
      bool Emit(V const& val);
      bool EmitOne(V const& val);
      V const& Value() const;
    private:
      V _value;
  };
}

# include <reactor/signal.hxx>

#endif
