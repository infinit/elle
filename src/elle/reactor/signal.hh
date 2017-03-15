#pragma once

#include <elle/reactor/Waitable.hh>

namespace elle
{
  namespace reactor
  {
    /// Signal is the most basic Waitable.
    ///
    /// It offers public methods for signal (Waitable::_signal) and signal_one
    /// (Waitable::_signal_one).
    ///
    /// elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope &s)
    /// {
    ///   elle::reactor::Signal s;
    ///   s.run_background("one",
    ///                    [&]()
    ///                    {
    ///                      std::cout << "1"
    ///                      s.wait();
    ///                      std::cout << "2"
    ///                      s.signal();
    ///                      std::cout << "3"
    ///                    });
    ///   s.run_background("two",
    ///                    [&]()
    ///                    {
    ///                      std::cout << "A";
    ///                      s.signal();
    ///                      std::cout << "B";
    ///                      s.wait();
    ///                      std::cout << "C";
    ///                    });
    ///   s.wait();
    /// };
    /// // Result: 1AB23C
    ///
    /// \endcode
    class Signal
      : public Waitable
    {
    public:
      using Self = elle::reactor::Signal;
      using Super = elle::reactor::Waitable;
      /// Construct a signal.
      ///
      /// \param name A descriptive name of the Waitable, for debugging purpose.
      Signal(const std::string& name = std::string());
      virtual
      ~Signal();
      Signal(Signal&&) = default;
      /// Signal Waiters to wake up.
      ///
      /// @see Waitable::_signal.
      bool
      signal();
      /// Signal a Waiter to wake up.
      ///
      /// @see Waitable::_signal_one.
      bool
      signal_one();

      /*----------.
      | Printable |
      `----------*/
    public:
      void
      print(std::ostream& stream) const override;
    };

    /// VSignal works just like Signal but can carry a value.
    ///
    /// After being awake by a VSignal, a Thread can get the last value emitted
    /// by the Signal. It's necessarily the value emitted that woke you up.
    template <typename V>
    class VSignal
      : public Waitable
    {
    public:
      using Self = elle::reactor::Signal;
      using Super = elle::reactor::Waitable;
      VSignal(std::string const& name = std::string());
      /// Set the value and signal.
      bool
      emit(V const& val);
      /// Set the value and signal_one.
      bool
      emit_one(V const& val);
    private:
      /// The last value emitted.
      ELLE_ATTRIBUTE_R(V, value);
    };
  }
}

#include <elle/reactor/signal.hxx>
