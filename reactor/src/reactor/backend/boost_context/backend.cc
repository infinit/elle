#include <boost/context/fcontext.hpp>

#include <elle/Backtrace.hh>
#include <elle/assert.hh>
#include <elle/log.hh>

#include <reactor/backend/boost_context/backend.hh>
#include <reactor/exception.hh>

ELLE_LOG_COMPONENT("reactor.backend");

namespace reactor
{
  namespace backend
  {
    namespace boost_context
    {
      /*----------------.
      | Stack Allocator |
      `----------------*/
      template <std::size_t Max, std::size_t Default, std::size_t Min>
      class TemplatedStackAllocator
      {
      public:
        static
        std::size_t
        maximum_stack_size()
        {
          return Max;
        }
        static
        std::size_t
        default_stack_size()
        {
          return Default;
        }
        static
        std::size_t
        minimum_stack_size()
        {
          return Min;
        }

        void*
        allocate(std::size_t size) const
        {
          ELLE_ASSERT(minimum_stack_size() <= size);
          ELLE_ASSERT(maximum_stack_size() >= size);

          void* limit = std::malloc(size);
          if (!limit)
            throw std::bad_alloc();

          return static_cast<char*>(limit) + size;
        }

        void
        deallocate(void* vp, std::size_t size) const
        {
          ELLE_ASSERT(vp);
          ELLE_ASSERT(minimum_stack_size() <= size);
          ELLE_ASSERT(maximum_stack_size() >= size);

          void* limit = static_cast<char*>(vp) - size;
          std::free(limit);
        }
      };

      /*-------.
      | Thread |
      `-------*/
      /// Default allocator type.
      typedef TemplatedStackAllocator<
        8 * 1024 * 1024,  // Max: 8 MiB
        128 * 1024,       // Default: 128 kiB
        8 * 1024          // Min: 8 kiB
        > StackAllocator;
      /// Type of context pointer used.
      typedef boost::context::fcontext_t Context;

      /// Allocator.
      static StackAllocator stack_allocator;

      static
      void
      wrapped_run(intptr_t arg);

      using boost::context::make_fcontext;
      using boost::context::jump_fcontext;

      class Backend::Thread:
        public backend::Thread
      {
      /*---------.
      | Typedefs |
      `---------*/
      public:
        typedef Thread Self;
        typedef backend::Thread Super;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        Thread(Backend& backend,
               const std::string& name,
               const Action& action)
          : Super(name, action)
          , _backend(backend)
          , _stack_store(
              stack_allocator.allocate(StackAllocator::default_stack_size()))
          , _context(
              make_fcontext(this->_stack_store,
                            StackAllocator::default_stack_size(),
                            wrapped_run))
          , _caller(nullptr)
          , _root(false)
          , _unwinding(false)
        {}

        ~Thread()
        {
          ELLE_ASSERT(this->status() == Status::done ||
                      this->status() == Status::starting ||
                      this->_root);
          ELLE_TRACE("%s: die", *this);
          if (this->_context)
          {
            this->_context = nullptr;
            stack_allocator.deallocate(this->_stack_store,
                                       StackAllocator::default_stack_size());
          }
        }

      private:
        Thread(Backend& backend)
          : Thread(backend, "<root>", Action())
        {
          this->_root = true;
          this->status(Status::running);
          ELLE_ASSERT(this->_context);
        }

      /*----------.
      | Switching |
      `----------*/
      public:
        virtual
        void
        step() override
        {
          // go from current to this
          ELLE_ASSERT(this->_caller == nullptr);

          bool starting = this->status() == Status::starting;
          Thread* current = this->_backend._current;
          this->_caller = current;
          this->_backend._current = this;
          current->_unwinding = std::uncaught_exception();
          if (current->_unwinding)
            ELLE_DUMP("step %s with in-flight exception", *current);
          if (starting)
          {
            this->status(Status::running);
            ELLE_ASSERT(this->_context);
            ELLE_TRACE("%s: start %s", *current , *this);
            jump_fcontext(&this->_caller->_context,
                          this->_context,
                          reinterpret_cast<intptr_t>(this));
            ELLE_TRACE("%s: back from %s", *current, *this);
          }
          else
          {
            ELLE_ASSERT_EQ(this->status(), Status::waiting);
            this->status(Status::running);
            ELLE_TRACE("%s: step from %s", *this, *_caller);
            if (this->_exception)
            {
              /* The stack we are switching to expects std::current_exception()
              * to return this->_exception (which contains current_exception()
              * at the time of yield).
              * So wrap the switch into a catch clause to trick the compiler
              * into restoring it.
              */
              try
              {
                std::rethrow_exception(this->_exception);
              }
              catch(...)
              {
                jump_fcontext(&current->_context,
                              this->_context,
                              reinterpret_cast<intptr_t>(this));
              }
            }
            else
            {
              jump_fcontext(&current->_context,
                            this->_context,
                            reinterpret_cast<intptr_t>(this));
            }
          }
          // It is unclear whether an uncaught_exception mismatch has any
          // consequence if the code does not explicitly depend on its result.
          // Warn about it just in case.
          if (this->_backend._current->_unwinding != std::uncaught_exception())
          {
            ELLE_TRACE("step %s: unwind mismatch, expect %s, got %s",
              *this->_backend._current, this->_backend._current->_unwinding,
              std::uncaught_exception());
          }
        }


        virtual
        void
        yield() override
        {
          // go from this to caller
          ELLE_ASSERT_EQ(this->_backend._current, this);
          ELLE_ASSERT_EQ(this->status(), Status::running);
          this->status(Status::waiting);
          this->_backend._current = this->_caller;
          ELLE_TRACE("%s: yield back to %s", *this, *this->_backend._current);
          this->_caller = nullptr;
          // Store current exception and stack unwinding state
          this->_unwinding = std::uncaught_exception();
          this->_exception = std::current_exception();
          if (this->_unwinding)
            ELLE_DUMP("yielding %s with in-flight exception", *this);
          jump_fcontext(&this->_context,
                        this->_backend._current->_context,
                        reinterpret_cast<intptr_t>(this));
          if (this->_backend._current->_unwinding != std::uncaught_exception())
          {
            ELLE_TRACE("yield %s: unwind mismatch, expect %s, got %s",
              *this->_backend._current, this->_backend._current->_unwinding,
              std::uncaught_exception());
          }
        }


      /*--------.
      | Details |
      `--------*/
      private:
        /// Let the backend use our private constructor.
        friend class Backend;
        /// Main routine of this thread of execution.
        void
        _run()
        {
          this->status(Status::running);
          try
          {
            this->action()();
          }
          catch (reactor::Exception const& e)
          {
            std::cerr << "Thread " << this->name()
                      << " killed by exception "
                      << elle::demangle(typeid(e).name()) << ": "
                      << e.what() << "." << std::endl;
            std::cerr << e.backtrace() << std::endl;
            std::abort();
          }
          catch (const std::exception& e)
          {
            std::cerr << "Thread " << this->name()
                      << " killed by exception "
                      << elle::demangle(typeid(e).name()) << ": "
                      << e.what() << "." << std::endl;
            std::abort();
          }
          catch (...)
          {
            std::cerr << "Thread " << this->name()
                      << " killed by unknown exception."
                      << std::endl;
            std::abort();
          }
          Thread* caller = this->_caller;
          this->_caller = nullptr;
          this->status(Status::done);
          this->_backend._current = caller;
          ELLE_TRACE("%s: done", *this);
          jump_fcontext(&this->_context,
                        caller->_context,
                        reinterpret_cast<intptr_t>(this));
        }

        /// Owning backend.
        Backend& _backend;
        /// Context stack store.
        void* _stack_store;
        /// Underlying IO context.
        Context _context;
        /// The thread that stepped us.
        Thread* _caller;
        /// Wrap run function so that it can be passed when doing make_fcontext.
        friend void wrapped_run(intptr_t);
        ELLE_ATTRIBUTE(bool, root);
        ELLE_ATTRIBUTE(bool, unwinding);
        ELLE_ATTRIBUTE(std::exception_ptr, exception); // stored when yielding
      };

      static
      void
      wrapped_run(intptr_t arg)
      {
        Backend::Thread* thread = reinterpret_cast<Backend::Thread*>(arg);
        thread->_run();
      }

      /*--------.
      | Backend |
      `--------*/

      Backend::Backend():
        _self(new Thread(*this)),
        _current(this->_self.get())
      {}

      Backend::~Backend()
      {}

      std::unique_ptr<backend::Thread>
      Backend::make_thread(const std::string& name,
                           const Action& action)
      {
        return std::unique_ptr<backend::Thread>(
          new Thread(*this, name, action));
      }

      Thread*
      Backend::current() const
      {
        return this->_current;
      }
    }
  }
}
