#pragma once

#include <elle/Duration.hh>

namespace elle
{
  namespace reactor
  {
    /// An exponential backoff.
    ///
    /// Sleep for a delay, doubling it every time until it reaches a maximum.
    class Backoff
    {
    /*------.
    | Types |
    `------*/
    public:
      using Self = Backoff;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      /// Create an exponential backoff.
      ///
      /// @param base The first back off delay.
      /// @param max  The maximum back off delay.
      Backoff(Duration base = 1s, Duration max = 1min);

    public:
      /// Wait for the current delay and double it for next time.
      void
      backoff();
      /// The number of time we backed off already.
      ELLE_ATTRIBUTE_R(int, times_backed);
      /// The first back off delay.
      ELLE_ATTRIBUTE_R(Duration, base_backoff);
      /// The maximum back off delay.
      ELLE_ATTRIBUTE_R(Duration, max_backoff);
    };
  }
}
