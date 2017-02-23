#pragma once

#include <elle/optional.hh>

#include <elle/reactor/operation.hh>

namespace elle
{
  namespace reactor
  {
    template <typename T>
    class BackgroundOperationResult
    {
    protected:
      void
      _result_set(T&& v);
      ELLE_ATTRIBUTE_R(boost::optional<T>, result, protected);
    };

    template <>
    class BackgroundOperationResult<void>
    {
    protected:
      void
      _result_set(bool);
    };

    template<typename T>
    class BackgroundOperation
      : public Operation
      , public BackgroundOperationResult<T>
    {
    public:
      typedef std::function<T ()> Action;
      struct Status
      {
        bool aborted;
      };

    public:
      BackgroundOperation(Action const& action);
      ~BackgroundOperation();
      ELLE_ATTRIBUTE(Action, action);
      ELLE_ATTRIBUTE(std::shared_ptr<Status>, status);

    protected:
      void
      _abort() override;
      void
      _start() override;
    };
  }
}

#include <elle/reactor/BackgroundOperation.hxx>
