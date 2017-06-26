#pragma once

#include <elle/chrono.hh>

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
      Backoff(
        std::chrono::milliseconds base = std::chrono::milliseconds(1000),
        std::chrono::milliseconds max = std::chrono::milliseconds(1000 * 60));

    public:
      /// Wait for the current delay and double it for next time.
      void
      backoff();
      /// The number of time we backed off already.
      ELLE_ATTRIBUTE_R(int, times_backed);
      /// The first back off delay.
      ELLE_ATTRIBUTE_R(std::chrono::milliseconds, base_backoff);
      /// The maximum back off delay.
      ELLE_ATTRIBUTE_R(std::chrono::milliseconds, max_backoff);
    };
  }
}
