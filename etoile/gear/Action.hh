#ifndef ETOILE_GEAR_ACTION_HH
# define ETOILE_GEAR_ACTION_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/Printable.hh>
# include <elle/io/Dumpable.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Address.hh>

# include <boost/noncopyable.hpp>

namespace etoile
{
  namespace gear
  {

    /// This class defines an action related to a block performed on
    /// the storage layer.
    class Action:
      public elle::Printable,
      public elle::io::Dumpable,
      private boost::noncopyable
    {
      /*-------------.
      | Enumerations |
      `-------------*/
    public:
      enum Type
        {
          push,
          wipe
        };

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Action(Type const type);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Apply the current action on the given storage object.
      ///
      /// XXX[to re-work so as to use an object rather that a type: wait for
      ///     Depot to become instantiable]
      template <typename T>
      void
      apply() const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(Type, type);
    };

    /// Contain the action specializations.
    namespace action
    {
      /// Represent an action consisting in adding a new or modified block in
      /// the storage layer.
      class Push:
        public Action
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        Push(nucleus::proton::Address const& address,
             std::shared_ptr<nucleus::proton::Block const> block);

        /*--------.
        | Methods |
        `--------*/
      public:
        /// Returns the block associated with the action. Note that this
        /// method throws if the action is 'wipe'.
        nucleus::proton::Block const&
        block() const;

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
        ELLE_ATTRIBUTE_R(nucleus::proton::Address const, address);
        ELLE_ATTRIBUTE(std::shared_ptr<nucleus::proton::Block const>, block);
      };

      /// Represent an action consisting in removing an existing block from the
      /// storage layer.
      class Wipe:
        public Action
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        Wipe(nucleus::proton::Address const& address);

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
        ELLE_ATTRIBUTE_R(nucleus::proton::Address const, address);
      };
    }

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Action::Type type);
  }
}

# include <etoile/gear/Action.hxx>

#endif
