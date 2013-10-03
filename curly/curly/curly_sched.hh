#ifndef CURLY_SCHED_HH
# define CURLY_SCHED_HH

# include <reactor/operation.hh>

# include <curly/asio_request.hh>

namespace curly
{
  class sched_request: public reactor::Operation
  {
  private:
    curly::asio_request _request;

    void
    raise_if_ecode(CURL* easy,
                   CURLcode code,
                   std::string const& message);

  public:
    sched_request(reactor::Scheduler& sched,
                  curly::request_configuration conf);

  protected:
    virtual void _start() override;
    virtual void _abort() override;

    ELLE_ATTRIBUTE_R(long, code);
    ELLE_ATTRIBUTE_R(bool, succeed);
  };
} /* curly */


#endif /* end of include guard: CURLY_SCHED_HH */
