#pragma once

#include <elle/reactor/waitable.hh>

namespace elle
{
  namespace reactor
  {
    class Signal
      : public Waitable
    {
    public:
      typedef Signal Self;
      typedef Waitable Super;
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
      virtual
      void
      print(std::ostream& stream) const override;
    };

    template <typename V>
    class VSignal
      : public Waitable
    {
    public:
      typedef Signal Self;
      typedef Waitable Super;
      VSignal(std::string const& name = std::string());
      bool Emit(V const& val);
      bool EmitOne(V const& val);
      V const& Value() const;
    private:
      V _value;
    };
  }
}

#include <elle/reactor/signal.hxx>
