#ifndef REACTOR_BACKGROUND_OPERATION_HH
# define REACTOR_BACKGROUND_OPERATION_HH

# include <boost/optional/optional_io.hpp>

# include <reactor/operation.hh>

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
    virtual
    void
    _abort() override;
    virtual
    void
    _start() override;
  };
}

# include <reactor/BackgroundOperation.hxx>

#endif
