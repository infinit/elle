#ifndef INFINIT_REACTOR_BARRIER_HH
# define INFINIT_REACTOR_BARRIER_HH

# include <reactor/waitable.hh>

namespace reactor
{
  /// A closed or opened checkpoint threads can wait upon.
  ///
  /// Threads waiting on a barrier will sleep until the barrier is opened. If
  /// the barrier is already opened, they will carry on without even stopping.
  ///
  /// A Barrier is similar to a Signal, except it holds a state, enabling to
  /// detect if an event happens or already happened, while signals only pulse
  /// when triggered.
  class Barrier
    : public Waitable
  {
  /*------.
  | Types |
  `------*/
  public:
    /// Our own type.
    typedef Barrier Self;
    /// The parent type.
    typedef Waitable Super;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// Create a closed Barrier with the given name.
    /// @param name The barrier name, for pretty-printing purpose.
    Barrier(const std::string& name = std::string());

  /*---------.
  | Openness |
  `---------*/
  public:
    /// Whether this is opened.
    operator bool() const;
  private:
    /// Whether this is opened.
    ELLE_ATTRIBUTE_R(bool, opened);

  /*--------.
  | Waiting |
  `--------*/
  public:
    /// Open this, letting current and future threads go past it.
    void open();
    /// Close this, stopping future threads waiting it.
    void close();
  protected:
    /// Stop the thread if and only if this is closed.
    virtual bool _wait(Thread* thread);

  /*----------.
  | Printable |
  `----------*/
  public:
    /// Pretty print this.
    virtual
    void
    print(std::ostream& stream) const override;
  };
}

#endif
