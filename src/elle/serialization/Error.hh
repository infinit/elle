#pragma once

#include <elle/Error.hh>

namespace elle
{
  namespace serialization
  {
    /// The base of serialization errors.
    class ELLE_API Error
      : public elle::Error
    {
    public:
      /// Construct a serialization Error with \a message.
      ///
      /// @param message The message.
      Error(std::string const& message);
      /// Construct a serialization Error from a Serializer.
      ///
      /// @param input The serializer containing the representation of the
      ///              Error.
      Error(SerializerIn& input);
    };
  }
}
