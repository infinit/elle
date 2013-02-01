#ifndef ETOILE_PATH_WAY_HH
# define ETOILE_PATH_WAY_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>

# include <etoile/path/fwd.hh>

namespace etoile
{
  namespace path
  {

    ///
    /// this class represents a string-based path i.e a way.
    ///
    class Way:
      public elle::Printable
    {
    public:
      //
      // constants
      //
      static const Way                  Null;

      //
      // constructors & destructors
      //
      Way();
      Way(const Way&) = default;
      Way(const elle::Character&);
      Way(const elle::String&);
      Way(const Way&,
          Slice&);
      Way(const wchar_t * u16_str);

      //
      // methods
      //
      elle::Status              Capacity(Length&) const;

      //
      // interfaces
      //

      ELLE_OPERATOR_ASSIGNMENT(Way); // XXX

      elle::Boolean             operator==(const Way&) const;

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      // printable
      void
      print(std::ostream& stream) const;

      //
      // attributes
      //
      elle::String              path;
    };

  }
}

# include <etoile/path/Way.hxx>

#endif
