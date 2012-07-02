#ifndef ELLE_NETWORK_BUNDLE_HH
# define ELLE_NETWORK_BUNDLE_HH

# include <elle/radix/Entity.hh>
# include <elle/radix/Arguments.hh>
# include <elle/radix/Parameters.hh>

# include <elle/network/Tag.hh>

namespace elle
{
  namespace network
  {

    ///
    /// this class represents a set of arguments associated with a network
    /// tag.
    ///
    /// note that this class is specialized for inputs (const) and ouptuts
    /// (non-const).
    ///
    struct Bundle
    {
      ///
      /// this generic Inputs class.
      ///
      template <const Tag G,
                typename... T>
      class Inputs;

      ///
      /// this class, specialized with Parameters<T...>, takes a set
      /// of values to bundle together so as to serialize them in order
      /// to send them over a network connection.
      ///
      template <const Tag G,
                typename... T>
      class Inputs< G, radix::Parameters<T...> >
        : public radix::Entity
      {
      public:
        //
        // types
        //
        typedef radix::Parameters<const T&...>                         P;

        //
        // constructors & destructors
        //
        Inputs(const T&...);
        template <template <typename...> class E,
                  typename... U>
        Inputs(E< radix::Parameters<U...> >&);

        //
        // interfaces
        //

        // dumpable
        Status          Dump(const Natural32 = 0) const;

        //
        // attributes
        //
        Tag             tag;
        radix::Arguments<P>    arguments;
      };

      ///
      /// the generic Outputs class.
      ///
      template <const Tag G,
                typename... T>
      class Outputs;

      ///
      /// like the Inputs class, this class is specialized for Parameters<T...>
      /// and is responsible for extracting values from a network packet so
      /// as to set the variables provided through the constructor.
      ///
      template <const Tag G,
                typename... T>
      class Outputs< G, radix::Parameters<T...> >
        : public radix::Entity
      {
      public:
        //
        // types
        //
        typedef radix::Parameters<T&...>                               P;

        //
        // constructors & destructors
        //
        Outputs(T&...);
        template <template <typename...> class E,
                  typename... U>
        Outputs(E< radix::Parameters<U...> >&);

        //
        // interfaces
        //

        // dumpable
        Status          Dump(const Natural32 = 0) const;

        //
        // attributes
        //
        Tag             tag;
        radix::Arguments<P>    arguments;
      };

    };

  }
}

#include <elle/network/Bundle.hxx>

#endif
