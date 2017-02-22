#ifndef ELLE_DEBUG_TRACKER_HH
# define ELLE_DEBUG_TRACKER_HH

# include <elle/log.hh>

namespace elle
{
  namespace debug
  {
    template <class T>
    class Tracker
      : public T
    {
    public:
      template <typename ... Args>
      Tracker(Args&& ... args)
        : T(std::forward<Args>(args)...)
      {
        ELLE_LOG("%s: created", this);
      }

      Tracker(Tracker const& t)
        : T(t)
      {
        ELLE_LOG("%s: copied", this);
      }

      Tracker(Tracker&& t)
        : T(std::move(t))
      {
        ELLE_LOG("%s: moved", this);
      }

      ~Tracker()
      {
        ELLE_LOG("%s: destroyed", this);
      }
    };
  }
}


#endif
