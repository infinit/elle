#ifndef INFINIT_REACTOR_BARRIER_HH
# define INFINIT_REACTOR_BARRIER_HH

# include <reactor/waitable.hh>

namespace reactor
{
  class Barrier
    : public Waitable
  {
  /*------.
  | Types |
  `------*/
  public:
    typedef Barrier Self;
    typedef Waitable Super;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    Barrier(const std::string& name = std::string());

  /*--------.
  | Waiting |
  `--------*/
  public:
    void open();
    void close();
  protected:
    virtual bool _wait(Thread* thread);
  private:
    ELLE_ATTRIBUTE_R(bool, opened);

  /*----------.
  | Printable |
  `----------*/
  public:
    virtual
    void
    print(std::ostream& stream) const override;
  };
}

#endif
