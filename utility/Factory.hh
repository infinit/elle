#ifndef ELLE_UTILITY_FACTORY_HH
# define ELLE_UTILITY_FACTORY_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>
# include <elle/radix/Entity.hh>

# include <elle/utility/Product.hh>

# include <elle/idiom/Close.hh>
#  include <map>
# include <elle/idiom/Open.hh>

namespace elle
{
  namespace utility
  {

    ///
    /// a factory provides a way for generating instance of given classes
    /// depending on a product identifier. for instance the identifier 23
    /// would be associated with the type PublicKey so that calling the
    /// factory's Build() method with 23 would return a PublicKey object
    /// freshly allocated.
    ///
    /// this class thus holds the mappings between identifiers and the
    /// functionoids capable of generating types.
    ///
    class Factory:
      public radix::Object
    {
    public:
      //
      // classes
      //

      ///
      /// this class is the base class for factory functionoids.
      ///
      class Functionoid:
        public radix::Entity
      {
      public:
        //
        // constructors & destructors
        //
        virtual ~Functionoid()
        {
        }

        //
        // methods
        //

        ///
        /// this method must be redefined in every sub-class.
        ///
        /// note that using a template method would have been better but
        /// since template methods cannot be virtual, the argument is assumed
        /// to be a derived entity.
        ///
        virtual Status  Allocate(void*&) const = 0;
      };

      ///
      /// this functionoid contains a method for generating a void object
      /// of the given type.
      ///
      template <typename T>
      class Generatoid:
        public Functionoid
      {
      public:
        //
        // constructors & destructors
        //
        Generatoid(const Product&);

        //
        // methods
        //
        Status          Allocate(void*&) const;

        //
        // attributes
        //
        Product         identifier;
      };

      //
      // types
      //
      typedef std::map<const Product, Functionoid*>     Container;
      typedef Container::iterator                       Iterator;
      typedef Container::const_iterator                 Scoutor;

      //
      // constructors & destructors
      //
      ~Factory();

      //
      // methods
      //
      template <typename T>
      Status            Register(const Product&);
      template <typename U>
      Status            Build(const Product&,
                              U*&) const;

      Status            Clear();

      //
      // interfaces
      //

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Container         container;
    };

  }
}

#include <elle/utility/Factory.hxx>

#endif
