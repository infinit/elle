#ifndef NUCLEUS_PROTON_DOOR_HH
# define NUCLEUS_PROTON_DOOR_HH

# include <nucleus/proton/Handle.hh>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>

# include <boost/noncopyable.hpp>

namespace nucleus
{
  namespace proton
  {
    /// Enable on to access a Value when retrieved from a Porcupine, no
    /// matter when the value is located: in main memory, in cache or
    /// on the storage layer in which case the value would be accessed
    /// through the nest.
    template <typename T>
    class Door:
      public elle::io::Dumpable,
      public elle::Printable,
      private boost::noncopyable
    {
      /*-------------.
      | Enumerations |
      `-------------*/

      /// Define the location of the value, being in main memory or in the
      /// nest.
      enum class Location
        {
          memory,
          nest
        };

      /*-------------.
      | Construction |
      `-------------*/
    public:
      /// Construct a door for access to an in-main-memory value.
      Door(T* value);
      /// Construct a door for access to a nest-based value block.
      Door(Handle& handle,
           Nest& nest);
      /// Destruct the door, making sure to unload the value block, should it
      /// be necessary and nest-based.
      ~Door();

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Open the door for accessing the value.
      void
      open();
      /// Close the door once the value has been accessed.
      void
      close();

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Door<T>);

      /// Return the value of type T.
      ///
      /// Note that, should the door not have been opened, it is automatically.
      T const&
      operator ()() const;
      /// Return the value of type T.
      ///
      /// Note that, should the door not have been opened, it is automatically.
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
      ELLE_ATTRIBUTE(Location, location);
      union
      {
        T* _value;
        Ambit<T>* _ambit;
      };

      /*----------.
      | Operators |
      `----------*/
    public:
      friend
      std::ostream&
      operator <<(std::ostream& stream,
                  Location const location)
      {
        switch (location)
          {
          case Location::memory:
            {
              stream << "memory";
              break;
            }
          case Location::nest:
            {
              stream << "nest";
              break;
            }
          default:
            {
              throw Exception("unknown location: '%s'",
                              static_cast<int>(location));
            }
          }

        return (stream);
      }
    };
  }
}

# include <nucleus/proton/Door.hxx>

#endif
