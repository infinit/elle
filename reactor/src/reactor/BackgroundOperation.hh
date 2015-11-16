#ifndef REACTOR_BACKGROUND_OPERATION_HH
# define REACTOR_BACKGROUND_OPERATION_HH

# include <reactor/operation.hh>

namespace reactor
{
  class BackgroundOperation:
    public Operation
  {
  public:
    typedef std::function<void ()> Action;
    struct Status
    {
      bool aborted;
    };

  public:
    BackgroundOperation(Action const& action);
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

#endif
