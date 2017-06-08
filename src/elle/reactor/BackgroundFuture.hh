#pragma once

#include <functional>
#include <type_traits>

#include <elle/reactor/BackgroundOperation.hh>

namespace elle
{
  namespace reactor
  {
    /// An action to perform, producing a result we don't need or can't produce
    /// right now.
    ///
    /// Uses BackgroundOperation, therefore, run in an std::thread.
    ///
    /// BackgroundOperation::value() will make the calling Thread to wait until
    /// the Operation completes.
    template <typename T>
    class BackgroundFuture
    {
    public:
      using Action = std::function<T ()>;
      /// Construct with a default constructed value. No operations are
      /// performed.
      template<typename = std::enable_if_exists_t<decltype(T())>>
        BackgroundFuture();
      /// Construct with a plain value. The value is stored directly, no extra
      /// operations are performed.
      ///
      /// @tparam T The type of value.
      /// @param value The value to store.
      BackgroundFuture(T value);
      /// Construct with an Action and start to BackgroundOperation. The value
      /// will be available when the Action is over.
      ///
      /// @param action The Action to perform.
      BackgroundFuture(Action action);
      ~BackgroundFuture();
      BackgroundFuture(BackgroundFuture<T> const& src);
      /// Return the value. If not yet available, block until it is.
      ///
      /// @returns A const reference to the value.
      T const&
      value() const;
      /// Return the value. If not yet available, block until it is.
      ///
      /// @returns A reference to the value.
      T&
      value();
      /// Update the Action.
      ///
      /// Current action is aborted if running, current value is reset. The new
      /// Action is started automatically.
      BackgroundFuture<T>&
      operator =(Action action);
      /// Set the current value.
      ///
      /// Current action is aborted if running, current value is set to the
      /// given value.
      BackgroundFuture<T>&
      operator =(T value);
      BackgroundFuture<T>&
      operator =(BackgroundFuture<T> const&) = delete;
      /// Whether the action is currently running.
      bool
      running() const;
      ELLE_ATTRIBUTE(boost::optional<BackgroundOperation<T>>, operation);
      ELLE_ATTRIBUTE(boost::optional<T>, value);
    private:
      void
      _resolve();
    };
  }
}

#include <elle/reactor/BackgroundFuture.hxx>
