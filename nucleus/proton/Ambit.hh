#ifndef NUCLEUS_PROTON_AMBIT_HH
# define NUCLEUS_PROTON_AMBIT_HH

# include <elle/types.hh>
# include <elle/attribute.hh>

# include <nucleus/proton/Handle.hh>

namespace nucleus
{
  namespace proton
  {

    /// XXX
    template <typename T>
    class Ambit:
      public elle::io::Dumpable
    {
      //
      // enumerations
      //
    public:
      /// XXX
      enum Mode
        {
          ModeManual,
          ModeAutomatic,
          ModeTransparent
        };
      /// XXX
      enum State
        {
          StateUnloaded,
          StateLoaded
        };

      //
      // constructors & destructors
      //
    public:
      /// XXX
      Ambit(Nest&,
            Handle&,
            Mode const = ModeAutomatic);
      /// XXX
      ~Ambit();

      //
      // methods
      //
    public:
      /// XXX
      void
      load();
      /// XXX
      void
      unload();
      /// XXX
      Handle&
      handle();
      /// XXX
      Contents*
      contents();
      /// XXX[special: cast the content's node into T]
      T*
      operator ()();

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_ASSIGNMENT(Ambit<T>);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
    private:
      Nest& _nest;
      Mode _mode;
      State _state;
      Handle& _handle;
      std::shared_ptr<Contents> _block;
    };

  }
}

#include <nucleus/proton/Ambit.hxx>

#endif
