#ifndef NUCLEUS_PROTON_AMBIT_HH
# define NUCLEUS_PROTON_AMBIT_HH

# include <nucleus/proton/Handle.hh>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>

# include <boost/noncopyable.hpp>

namespace nucleus
{
  namespace proton
  {
    /// Represent an area of control over a node. Instanciating such
    /// an ambit makes it easy to load and unload the referenced node block
    /// from the nest given at construction.
    template <typename T>
    class Ambit:
      public elle::io::Dumpable,
      public elle::Printable,
      private boost::noncopyable
    {
      /*-------------.
      | Enumerations |
      `-------------*/
    public:
      /// Define the way the referenced node block is loaded and unloaded.
      ///
      /// The _manual_ mode does nothing and let the caller manually load/unload
      /// the block. The _automatic_ mode makes sure the block is unloaded is
      /// the caller forgets to do it manually i.e when the scope is leaved.
      /// Finally, the _transparent_ mode loads and unloads the node block
      /// without the caller having to do anything.
      enum class Mode
        {
          manual,
          automatic,
          transparent
        };
      /// Represent the state of the node block being either loaded or unloaded.
      enum class State
        {
          unloaded,
          loaded
        };

      /*-------------.
      | Construction |
      `-------------*/
    public:
      /// Construct an ambit enabling one to load/unload from the _nest_ the
      /// block referenced by _handle_.
      ///
      /// Note that through these operations the _handle_ may be changed.
      ///
      /// The corollary is that the reference to the handle must remain valid
      /// throughout the life of the ambit. One must therefore take care not
      /// to unload the block containing the given handle.
      Ambit(Nest& nest,
            Handle& handle,
            Mode const mode = Mode::automatic);
      /// Destruct the ambit, making sure to unload the referenced block if
      /// necessary.
      ~Ambit();

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Load the block referenced by the ambit.
      void
      load();
      /// Unload the block referenced by the ambit.
      void
      unload();
      /// Return the Contents block associated with the given handle.
      Contents const&
      contents() const;
      /// Return the Contents block associated with the given handle.
      Contents&
      contents();

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Ambit<T>);

      /// Return the node of type T embedded in the referenced Contents block.
      T const&
      operator ()() const;
      /// Return the node of type T embedded in the referenced Contents block.
      T&
      operator ()();

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_X(Nest&, nest);
      ELLE_ATTRIBUTE(Mode, mode);
      ELLE_ATTRIBUTE_R(State, state);
      ELLE_ATTRIBUTE_RX(Handle&, handle);
      ELLE_ATTRIBUTE_R(std::shared_ptr<Contents>, block);

      /*----------.
      | Operators |
      `----------*/
    public:
      friend
      std::ostream&
      operator <<(std::ostream& stream,
                  Mode const mode)
      {
        switch (mode)
          {
          case Mode::manual:
            {
              stream << "manual";
              break;
            }
          case Mode::automatic:
            {
              stream << "automatic";
              break;
            }
          case Mode::transparent:
            {
              stream << "transparent";
              break;
            }
          default:
            {
              throw Exception("unknown mode: '%s'", static_cast<int>(mode));
            }
          }

        return (stream);
      }

      friend
      std::ostream&
      operator <<(std::ostream& stream,
                  State const state)
      {
        switch (state)
          {
          case State::unloaded:
            {
              stream << "unloaded";
              break;
            }
          case State::loaded:
            {
              stream << "loaded";
              break;
            }
          default:
            {
              throw Exception("unknown state: '%s'", static_cast<int>(state));
            }
          }

        return (stream);
      }
    };
  }
}

# include <nucleus/proton/Ambit.hxx>

#endif
