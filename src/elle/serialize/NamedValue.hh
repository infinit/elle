#ifndef  ELLE_SERIALIZE_NAMEDVALUE_HH
# define ELLE_SERIALIZE_NAMEDVALUE_HH

namespace elle
{
  namespace serialize
  {

    ///
    /// NamedValue is necessary for certain serialization format, like JSON or
    /// XML, when values should be named to be stored and loaded back.
    ///
    /// Its use is quite simple, and do not affect other kind of archives that
    /// don't need it.
    ///
    /// -----------------------------------------------------------------------
    /// struct A
    /// {
    ///   int i;
    /// };
    ///
    /// ELLE_SERIALIZE_SIMPLE(A, archive, value, version)
    /// {
    ///   archive & NamedValue<int>("integer", value.i);
    /// }
    /// -----------------------------------------------------------------------
    ///
    /// In JSON, the previous example will generate something like:
    /// ----
    /// {
    ///   "_class_version": 0,
    ///   "integer": 42,
    /// }
    /// ----
    ///
    /// @see elle::serialize::named() for an helper
    ///
    template <typename T>
    struct NamedValue;

    /// Helper to infer the right NamedValue<T> type.
    template <typename T>
    inline
    NamedValue <T>
    named(std::string const& name,
          T& value);

    /// Helper to infer the right NamedValue<T> type.
    template <typename T> inline
    NamedValue<T const>
    named(std::string const& name,
          T const& value);

    /// Check whether or not the type T is a NamedValue.
    template <typename T> struct IsNamedValue;

  }
}

# include "NamedValue.hxx"

#endif

