#import <arpa/inet.h>
#import <ifaddrs.h>
#import <netdb.h>
#import <sys/socket.h>
#import <netinet/in.h>
#import <SystemConfiguration/SCNetworkReachability.h>

#include <boost/asio/ip/address.hpp>

#include <elle/log.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/network/reachability.hh>

ELLE_LOG_COMPONENT("elle.reactor.network.Reachability");

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /*-------------.
      | Enum Printer |
      `-------------*/

      static
      std::string
      string_from_status(Reachability::NetworkStatus status)
      {
        switch (status)
        {
          case Reachability::NetworkStatus::Unreachable:
            return "Unreachable";
          case Reachability::NetworkStatus::Reachable:
            return "Reachable";
#ifdef INFINIT_IOS
          case Reachability::NetworkStatus::ReachableWWAN:
            return "ReachableWWAN";
#endif
          default:
            elle::unreachable();
        }
      }

      /*---------------.
      | Implementation |
      `---------------*/

      class Reachability::Impl
      {
      public:
        Impl(Reachability& owner, boost::optional<std::string> host);
        ~Impl();

        void
        start();
        void
        stop();

        ELLE_ATTRIBUTE_R(bool, running);
        void
        status(Reachability::NetworkStatus status);
        Reachability::NetworkStatus
        status() const;

      private:
        ELLE_ATTRIBUTE(bool, first);
        ELLE_ATTRIBUTE(Reachability*, owner);
        ELLE_ATTRIBUTE(SCNetworkReachabilityRef, reachability_ref);
        ELLE_ATTRIBUTE(CFRunLoopRef, run_loop);
        reactor::Scheduler* _sched;
        std::atomic<Reachability::NetworkStatus> _status;
        ELLE_ATTRIBUTE(std::unique_ptr<std::thread>, thread);
      };

      static
      void
      reachability_callback(SCNetworkReachabilityRef target,
                            SCNetworkReachabilityFlags flags,
                            void* info)
      {
        using NetworkStatus = Reachability::NetworkStatus;
        auto self = static_cast<Reachability::Impl*>(info);
        if (!self->running())
          return;
        // Assume unreachable.
        NetworkStatus status = NetworkStatus::Unreachable;
        if ((flags & kSCNetworkReachabilityFlagsReachable) == 0)
        { /* Do nothing, we can't reach host. */ }
        else if ((flags & kSCNetworkReachabilityFlagsConnectionRequired) == 0)
        {
          status = NetworkStatus::Reachable;
#ifdef INFINIT_IOS
          // WWAN connection.
          if ((flags & kSCNetworkReachabilityFlagsIsWWAN) ==
                kSCNetworkReachabilityFlagsIsWWAN)
          {
            status = NetworkStatus::ReachableWWAN;
          }
#endif
        }
        self->status(status);
      }

      Reachability::Impl::Impl(Reachability& owner,
                               boost::optional<std::string> host)
        : _running(false)
        , _first(true)
        , _owner(&owner)
        , _reachability_ref(nullptr)
        , _sched(&reactor::scheduler())
      {
        ELLE_TRACE_SCOPE("%s: init with host: %s",
                         *this->_owner, host ? host.get() : "none");
        boost::system::error_code err;
        if (!host)
        {
          ELLE_DEBUG("%s: host is zero address", *this->_owner);
          struct sockaddr_in sa;
          bzero(&sa, sizeof(sa));
          sa.sin_len = sizeof(sa);
          sa.sin_family = AF_INET;
          this->_reachability_ref =
            SCNetworkReachabilityCreateWithAddress(nullptr, (sockaddr*)(&sa));
        }
        else
        {
          auto addr = boost::asio::ip::address::from_string(host.get(), err);
          if (!err) // Host was IP address.
          {
            if (addr.is_v4())
            {
              ELLE_DEBUG("%s: host is IPv4 address", *this->_owner);
              struct sockaddr_in sa;
              int res = inet_pton(AF_INET, host.get().data(), &sa.sin_addr);
              sa.sin_len = sizeof(sa);
              sa.sin_family = AF_INET;
              if (res == 1)
              {
                this->_reachability_ref =
                  SCNetworkReachabilityCreateWithAddress(nullptr, (sockaddr*)(&sa));
              }
              else
                ELLE_WARN("%s: unable to create sockaddr_in", &this->_owner);
            }
            else if (addr.is_v6())
            {
              ELLE_DEBUG("%s: host is IPv6 address", *this->_owner);
              struct sockaddr_in6 sa;
              int res = inet_pton(AF_INET6, host.get().data(), &sa.sin6_addr);
              sa.sin6_len = sizeof(sa);
              sa.sin6_family = AF_INET6;
              if (res == 1)
              {
                this->_reachability_ref =
                  SCNetworkReachabilityCreateWithAddress(nullptr, (sockaddr*)(&sa));
              }
              else
                ELLE_WARN("%s: unable to create sockaddr_in6", &this->_owner);
            }
            else
              throw elle::Error("Unknown address type");
          }
          else // Host was not IP address, assume hostname.
          {
            this->_reachability_ref =
              SCNetworkReachabilityCreateWithName(nullptr, host.get().c_str());
          }
        }
        if (this->_reachability_ref)
          ELLE_DEBUG("%s: created reachability object", *this->_owner);
        else
          ELLE_WARN("%s: unable to create reachability object", *this->_owner);
      }

      Reachability::Impl::~Impl()
      {
        ELLE_TRACE_SCOPE("%s: ~Impl", *this->_owner);
        this->_owner = nullptr;
        this->stop();
        if (this->_reachability_ref)
          CFRelease(this->_reachability_ref);
      }

      void
      Reachability::Impl::start()
      {
        if (this->running())
        {
          ELLE_DEBUG("%s: already started", *this->_owner);
          return;
        }
        ELLE_TRACE_SCOPE("%s: starting...", *this->_owner);
        if (!this->_reachability_ref)
        {
          ELLE_WARN("%s: start failed, no reachability ref", *this->_owner);
          return;
        }
        this->_running = true;
        this->_first = true;
        this->_thread.reset(new std::thread([&]
        {
          SCNetworkReachabilityContext context = {0, this, nullptr, nullptr, nullptr};
          if (SCNetworkReachabilitySetCallback(this->_reachability_ref,
                                               reachability_callback,
                                               &context))
          {
            this->_run_loop = CFRunLoopGetCurrent();
            CFRetain(this->_run_loop);
            if (SCNetworkReachabilityScheduleWithRunLoop(this->_reachability_ref,
                                                         this->_run_loop,
                                                         kCFRunLoopDefaultMode))
            {
              ELLE_DEBUG("%s: started", *this->_owner);
              SCNetworkReachabilityFlags flags;
              SCNetworkReachabilityGetFlags(this->_reachability_ref, &flags);
              reachability_callback(this->_reachability_ref, flags, this);
              CFRunLoopRun();
            }
            else
            {
              ELLE_WARN("%s: start failed, unable to add to run loop",
                        *this->_owner);
            }
          }
          else
          {
            ELLE_WARN("%s: start failed, unable to set callback", *this->_owner);
          }
        }));
      }

      void
      Reachability::Impl::stop()
      {
        if (!this->running())
          return;
        ELLE_TRACE_SCOPE("%s: stopping...", *this->_owner);
        this->_running = false;
        if (this->_reachability_ref && this->_run_loop)
        {
          SCNetworkReachabilityUnscheduleFromRunLoop(this->_reachability_ref,
                                                     this->_run_loop,
                                                     kCFRunLoopDefaultMode);
          ELLE_DEBUG("%s: stopped", *this->_owner);
          CFRunLoopStop(this->_run_loop);
          CFRelease(this->_run_loop);
          this->_run_loop = nullptr;
        }
        if (this->_thread && this->_thread->joinable())
          this->_thread->join();
        this->_thread.reset(nullptr);
      }

      void
      Reachability::Impl::status(Reachability::NetworkStatus status)
      {
        if (!this->_first && this->_status == status)
          return;
        this->_first = false;
        ELLE_TRACE("%s: status changed to: %s",
                   *this->_owner, string_from_status(status));
        this->_status = status;
        this->_sched->mt_run<void>(
          "network status change",
          [&]
          {
            this->_owner->status_changed()(status);
          });
      }

      Reachability::NetworkStatus
      Reachability::Impl::status() const
      {
        return this->_status;
      }

      /*----------.
      | Interface |
      `----------*/

      Reachability::Reachability(boost::optional<std::string> host,
                                 std::function<StatusCallback> const& callback,
                                 bool start)
        : _impl(new Impl(*this, host))
      {
        if (callback)
          this->_status_changed.connect(callback);
        if (start && this->_impl)
          this->start();
      }

      Reachability::~Reachability()
      {
        if (this->_impl)
          delete this->_impl;
      }

      void
      Reachability::start()
      {
        this->_impl->start();
      }

      void
      Reachability::stop()
      {
        this->_impl->stop();
      }

      bool
      Reachability::reachable() const
      {
        return this->status() != Reachability::NetworkStatus::Unreachable;
      }

      Reachability::NetworkStatus
      Reachability::status() const
      {
        return this->_impl->status();
      }

      std::string
      Reachability::status_string() const
      {
        return string_from_status(this->status());
      }
    }
  }
}
