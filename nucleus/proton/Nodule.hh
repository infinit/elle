#ifndef NUCLEUS_PROTON_NODULE_HH
# define NUCLEUS_PROTON_NODULE_HH

# include <elle/types.hh>
# include <elle/attribute.hh>

# include <cryptography/fwd.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Node.hh>
# include <nucleus/proton/Flags.hh>
# include <nucleus/proton/Porcupine.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class abstract the notion of tree node in a nodule.
    ///
    /// every nodule therefore encapsulates the type i.e seam or quill
    /// along with common information such as the addresses of the parent
    /// and neighbour nodules.
    template <typename T>
    class Nodule:
      public Node
    {
      /*-------------.
      | Enumerations |
      `-------------*/
    public:
      enum class Type
        {
          seam,
          quill
        };

      //
      // static methods
      //
    public:
      /// XXX
      template <typename X>
      static
      void
      transfer_right(X& left,
                     X& right,
                     Extent const size);
      /// XXX
      template <typename X>
      static
      void
      transfer_left(X& left,
                    X& right,
                    Extent const size);
      /// XXX
      template <typename X>
      static
      void
      optimize(X& nodule,
               typename T::K const& k);
      /// XXX[a mettre dans Node?]
      template <typename N>
      static
      Contents*
      split(N& node);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Nodule(Type const type);

      //
      // virtual methods
      //
      /// XXX
      virtual
      void
      add(typename T::K const& k,
          Handle const& v) = 0;
      /// XXX
      virtual
      void
      remove(typename T::K const& k) = 0;
      /// XXX
      virtual
      void
      update(typename T::K const& k) = 0;
      /// XXX
      virtual
      Handle
      split() = 0;
      /// XXX
      virtual
      void
      merge(Handle& handle) = 0;
      /// XXX
      virtual
      elle::Boolean
      empty() const = 0;
      /// XXX
      virtual
      elle::Boolean
      single() const = 0;
      /// XXX
      virtual
      Handle
      search(typename T::K const& k) = 0;
      /// XXX
      virtual
      Handle
      seek(Capacity const target,
           Capacity& base) = 0;
      /// XXX
      virtual
      void
      check(Flags const flags = flags::none) = 0;
      /// XXX
      virtual
      void
      seal(cryptography::SecretKey const& secret) = 0;
      /// XXX
      virtual
      void
      walk(elle::Natural32 const margin = 0) = 0;
      /// XXX
      virtual
      void
      statistics(Statistics& stats) = 0;
      /// XXX[careful not to unload the nodule because reference is returned]
      virtual
      typename T::K const&
      mayor() const = 0;
      /// XXX[careful not to unload the nodule because reference is returned]
      virtual
      typename T::K const&
      maiden() const = 0;

      //
      // interfaces
      //

      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;

      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Nodule);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(Type, type);
    };

    // XXX operator << for Type
  }
}

# include <nucleus/proton/Nodule.hxx>

#endif
