#include <boost/context/fcontext.hpp>

#ifdef VALGRIND
# include <valgrind/valgrind.h>
#endif

#if defined(__has_feature)
# if __has_feature(address_sanitizer)
#  include <sanitizer/common_interface_defs.h>
# else
#  define __sanitizer_start_switch_fiber(...) {}
#  define __sanitizer_finish_switch_fiber(...) {}
# endif
#else
# define __sanitizer_start_switch_fiber(...) {}
# define __sanitizer_finish_switch_fiber(...) {}
#endif

#include <elle/Backtrace.hh>
#include <elle/assert.hh>
#include <elle/log.hh>
#include <elle/reactor/backend/boost/backend.hh>
#include <elle/reactor/exception.hh>

ELLE_LOG_COMPONENT("elle.reactor.backend.boost");

namespace elle
{
  namespace reactor
  {
    namespace backend
    {
      namespace boost
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

            if (auto limit = std::malloc(size))
              return static_cast<char*>(limit) + size;
            else
              throw std::bad_alloc();
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
        using StackAllocator = TemplatedStackAllocator<
          8 * 1024 * 1024,  // Max: 8 MiB
          4 * 128 * 1024,   // Default: 128 kiB
          8 * 1024          // Min: 8 kiB
          >;
        /// Type of context pointer used.
        using Context = ::boost::context::fcontext_t;

        /// Allocator.
        static StackAllocator stack_allocator;

        /// Invoke thread_ptr->_run().
        static
        void
        wrapped_run(intptr_t thread_ptr);

        using ::boost::context::make_fcontext;
        using ::boost::context::jump_fcontext;

        class Backend::Thread
          : public backend::Thread
        {
        /*---------.
        | Types.   |
        `---------*/
        public:
          using Self = Thread;
          using Super = backend::Thread;
        /*-------------.
        | Construction |
        `-------------*/
        public:
          Thread(Backend& backend,
                 const std::string& name,
                 Action action)
            : Super(name, std::move(action))
            , _backend(backend)
            , _stack_pointer(stack_allocator.allocate(this->_stack_size))
            , _context(make_fcontext(this->_stack_pointer,
                                     this->_stack_size, wrapped_run))
            , _root(false)
          {
#ifdef VALGRIND
            this->_valgrind_stack =
              VALGRIND_STACK_REGISTER(this->_base_pointer(),
                                      this->_stack_pointer);
#endif
            ELLE_DUMP("%s: create: sp = %x, ss = %x",
                      this, this->_stack_pointer, this->_stack_size);
          }

          ~Thread() override
          {
            ELLE_ASSERT(this->status() == Status::done ||
                        this->status() == Status::starting ||
                        this->_root);
            ELLE_TRACE("%s: die", this);
            ELLE_DUMP("%s: die: sp = %x, ss = %x",
                      this, this->_stack_pointer, this->_stack_size);
            if (this->_context)
            {
              this->_context = nullptr;
              stack_allocator.deallocate(this->_stack_pointer,
                                         this->_stack_size);
            }
#ifdef VALGRIND
            VALGRIND_STACK_DEREGISTER(this->_valgrind_stack);
#endif
          }

        private:
          Thread(Backend& backend)
            : Thread(backend, "<root>", Action())
          {
            this->_root = true;
            this->status(Status::running);
            ELLE_ASSERT(this->_context);
          }

          /// The bottom of the stack.
          void* _base_pointer() const
          {
            return static_cast<char*>(this->_stack_pointer) - this->_stack_size;
          }

          /// Pass the execution from @from to @to.
          void _jump(Thread* from, Thread* to)
          {
            ELLE_DUMP("%s: %s: _jump: start: sp = %x, ss = %x",
                      from, to, to->_stack_pointer, to->_stack_size);
            __sanitizer_start_switch_fiber(nullptr,
                                           to->_base_pointer(),
                                           to->_stack_size);
            jump_fcontext(&from->_context, to->_context,
                          reinterpret_cast<intptr_t>(this));
            ELLE_DUMP("%s: %s: _jump: finish", from, to);
            __sanitizer_finish_switch_fiber(nullptr, nullptr, nullptr);
          }



        /*----------.
        | Switching |
        `----------*/
        public:
          void
          step() override
          {
            // go from current to this
            ELLE_ASSERT(this->_caller == nullptr);

            bool starting = this->status() == Status::starting;
            auto const current = this->_backend._current;
            this->_caller = current;
            this->_backend._current = this;
            current->_unwinding = std::uncaught_exception();
            if (current->_unwinding)
              ELLE_DUMP("step %s with in-flight exception", *current);
            if (starting)
            {
              this->status(Status::running);
              ELLE_ASSERT(this->_context);
              ELLE_TRACE("%s: start %s", *current , this);
              this->_jump(this->_caller, this);
              ELLE_TRACE("%s: back from %s", *current, this);
            }
            else
            {
              ELLE_ASSERT_EQ(this->status(), Status::waiting);
              this->status(Status::running);
              ELLE_TRACE("%s: step from %s", this, *_caller);
              this->_jump(current, this);
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


          void
          yield() override
          {
            // go from this to caller
            ELLE_ASSERT_EQ(this->_backend._current, this);
            ELLE_ASSERT_EQ(this->status(), Status::running);
            this->status(Status::waiting);
            // Store current exception and stack unwinding state
            this->_unwinding = std::uncaught_exception();
            this->_exception = std::current_exception();
            this->_backend._current = this->_caller;
            ELLE_TRACE("%s: yield back to %s", this, *this->_backend._current);
            this->_caller = nullptr;

            if (this->_unwinding)
              ELLE_DUMP("yielding %s with in-flight exception", this);
            auto& next = this->_backend._current;
            this->_jump(this, next);

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
            ELLE_DUMP("%s: sanitizer: finish: _run", this);
            __sanitizer_finish_switch_fiber(nullptr, nullptr, nullptr);
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
            ELLE_TRACE("%s: done", this);
            this->_jump(this, caller);
          }

          /// Owning backend.
          Backend& _backend;
          /// Context stack size.
          std::size_t const _stack_size = StackAllocator::default_stack_size();
          /// Context stack pointer.
          void* _stack_pointer;
          /// Underlying IO context.
          Context _context;
          /// The thread that stepped us.
          Thread* _caller = nullptr;
          /// Wrap run function so that it can be passed when doing
          /// make_fcontext.
          friend void wrapped_run(intptr_t);
          ELLE_ATTRIBUTE(bool, root);
#ifdef VALGRIND
          ELLE_ATTRIBUTE(unsigned int, valgrind_stack);
#endif
        };

        static
        void
        wrapped_run(intptr_t thread_ptr)
        {
          auto* thread = reinterpret_cast<Backend::Thread*>(thread_ptr);
          thread->_run();
        }

        /*--------.
        | Backend |
        `--------*/

        Backend::Backend()
          : _self(new Thread(*this))
          , _current(this->_self.get())
        {}

        Backend::~Backend()
        = default;

        std::unique_ptr<backend::Thread>
        Backend::make_thread(const std::string& name, Action action)
        {
          return std::unique_ptr<backend::Thread>(
            new Thread(*this, name, std::move(action)));
        }

        Thread*
        Backend::current() const
        {
          return this->_current;
        }
      }
    }
  }
}
