#pragma once

#include <elle/serialization/Error.hh>

// FIXME:
//
// - inconsistent json:: namespace use
//   Or move some of these errors elsewhere.  E.g. FS (grpc, Paxos,
//   S3) uses serialization::MissingKey for something unrelated to
//   JSON.
//
// - these errors all could derived from FieldError.
//
// - Overflow could be a form of TypeError.

namespace elle
{
  namespace serialization
  {
    /// Error related to a missing key.
    class MissingKey
      : public Error
    {
    public:
      MissingKey(std::string const& field);
      ELLE_ATTRIBUTE_R(std::string, field);
    };

    /// An error related to a mismatching type.
    class TypeError
      : public Error
    {
    public:
      TypeError(std::string const& field,
                std::type_info const& expected,
                std::type_info const& effective);
      ELLE_ATTRIBUTE_R(std::string, field);
      ELLE_ATTRIBUTE_R(std::type_info const*, expected);
      ELLE_ATTRIBUTE_R(std::type_info const*, effective);
    };

    namespace json
    {
      /// Serialization error related to a specific JSON field.
      class FieldError
        : public Error
      {
      public:
        using Self = FieldError;
        using Super = Error;
        FieldError(std::string const& field, std::string const& message);
        ELLE_ATTRIBUTE_R(std::string, field);
      };

      /// Error related to an overflow.
      class Overflow
        : public serialization::Error
      {
      public:
        /// Build an overflow/underflow error.
        ///
        /// @param field  the field whose serialization failed.
        /// @param size   available number of bits.
        /// @param over   whether overflow, as opposed to underflow.
        Overflow(std::string const& field, int size, bool over);

        /// Build an overflow/underflow error.
        ///
        /// @param field  the field whose serialization failed.
        /// @param size   available number of bits.
        /// @param over   whether overflow, as opposed to underflow.
        /// @param value  the value that overflowed.
        template <typename T>
        Overflow(std::string const& field, int size, bool over,
                 T value)
          : Error(elle::sprintf("%s-bit %s on field \"%s\": %s",
                                size, over ? "overflow" : "underflow", field,
                                value))
          , _field(field)
        {}
        ELLE_ATTRIBUTE_R(std::string, field);
      };
    }
  }
}
