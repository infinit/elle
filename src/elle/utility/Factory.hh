#pragma once

#include <map>

#include <boost/noncopyable.hpp>

#include <elle/types.hh>
#include <elle/attribute.hh>

namespace elle
{
  namespace utility
  {
    /// A factory provides a way for generating instance of given classes
    /// depending on a product number. For instance the product 23
    /// would be associated with the type PublicKey so that calling the
    /// factory's allocate() method with 23 would return a PublicKey object
    /// freshly allocated.
    ///
    /// This class thus holds the mappings between products and the
    /// functionoids capable of generating types.
    template <typename P,
              typename... A>
    class Factory:
      private boost::noncopyable
    {
      /*--------.
      | Classes |
      `--------*/
    public:
      /// This class is the base class for factory functionoids.
      class Functionoid:
        private boost::noncopyable
      {
      public:
        // construction
        virtual
        ~Functionoid();

        // methods
      public:
        virtual
        void*
        allocate(A&&... arguments) const = 0;
      };

      /// This functionoid contains a method for generating a void object
      /// of the given type.
      template <typename T>
      class Generatoid:
        public Functionoid
      {
        // construction
      public:
        Generatoid(P const&);

        // methods
      public:
        void*
        allocate(A&&... arguments) const;

        // attributes
      private:
        ELLE_ATTRIBUTE(P const, product);
      };

      /*------.
      | Types |
      `------*/
    public:
      using Container = std::map<P, Functionoid*>;
      using Iterator = typename Container::iterator;
      using Scoutor = typename Container::const_iterator;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Factory() = default;
      Factory(Factory<P, A...>&& other);
      ~Factory();

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Create a mapping between the given product number and its type.
      template <typename T>
      void
      record(P const&);
      /// Return a freshly allocated object of the type associated with the
      /// product number and the associated arguments.
      template <typename T>
      T*
      allocate(P const& product,
               A&&... arguments) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE(Container, container);
    };
  }
}

#include <elle/utility/Factory.hxx>

