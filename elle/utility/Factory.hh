#ifndef ELLE_UTILITY_FACTORY_HH
# define ELLE_UTILITY_FACTORY_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/io/Dumpable.hh>

# include <map>

# include <boost/noncopyable.hpp>

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
    template <typename P>
    class Factory:
      public elle::io::Dumpable,
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
        ~Functionoid()
        {
        }

        // methods
      public:
        virtual
        void*
        allocate() const = 0;
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
        allocate() const;

        // attributes
      private:
        ELLE_ATTRIBUTE(P const, product);
      };

      /*------.
      | Types |
      `------*/
    public:
      typedef std::map<P const, Functionoid*> Container;
      typedef typename Container::iterator Iterator;
      typedef typename Container::const_iterator Scoutor;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      ~Factory();

      /*---------.
      |  Methods |
      `---------*/
    public:
      /// Create a mapping between the given product number and its type.
      template <typename T>
      void
      record(P const&);
      /// Return a freshly allocated object of the type associated with the
      /// product number.
      ///
      /// XXX[to remove when load constructors will be used: instead add an
      ///     archive attribute to this method]]
      template <typename T>
      T*
      allocate(P const&) const;

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      Status
      Dump(const Natural32 = 0) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE(Container, container);
    };

  }
}

# include <elle/utility/Factory.hxx>

#endif
