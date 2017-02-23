#pragma once
#ifdef INFINIT_WINDOWS
# include <winsock2.h>
#endif
#include <boost/thread.hpp>

namespace elle
{
  namespace reactor
  {
    namespace backend
    {
      namespace pthread
      {
        class Manager;
        class Thread;

        /** Thread is a cooperative thread.
         *
         * A Thread runs a callback in a separate thread of execution in
         * a cooperative way. That is, no code will be executed until
         * someone calls the step() method, at which point only this
         * thread will execute until it calls its yields() method or its
         * main routine ends.
         */
        class Thread
        {
          /*---------.
          | Typedefs |
          `---------*/
          public:
            /// Ourselves.
            typedef Thread Self;

          /*-------------.
          | Construction |
          `-------------*/
          public:
            /** Construct a Thread.
             *
             * @param manager The manager.
             * @param name This name for debugging purpose.
             * @param action The main routine this thread will run.
             */
            Thread(Manager& manager,
                   const std::string& name,
                   const Action& action);
            /** Dispose of a thread.
             *
             * A thread may only be destroyed once it finished
             * executing.
             */
            ~Thread();
          private:
            /** Constructor for the first root Thread.
             *
             * This thread represents the main thread - the one that
             * instantiated the manager.
             */
            Thread(Manager& manager);

          /*------.
          | State |
          `------*/
          public:
            /// Pretty name.
            std::string name() const;
            /// Current status.
            Status status() const;
            /// Whether our status is 'done'.
            bool done() const;
          private:
            std::string _name;
            Status _status;

          /*----------.
          | Switching |
          `----------*/
          public:
            /** Start or resume execution.
             *
             * Start execution by running the action or resume it at the
             * point where yield was called. May only be called on a
             * waiting or starting thread. Switch status to
             * running. Make this thread the current thread.
             */
            void step();
            /** Give execution back to our caller.
             *
             * Suspend our execution and give it back to the thread that
             * called our step method. May only be called on the current
             * thread (whose status is thus running). Switch status to
             * waiting. Make the caller the current thread.
             */
            void yield();

          /*--------.
          | Details |
          `--------*/
          private:
            /// Helper to bounce pthreads on Thread::_run.
            friend void* starter(void*);
            /// Let the manager use our private constructor.
            friend class Manager;
            /// Main routine of this thread of execution.
            void _run();
            /// Owning manager.
            Manager& _manager;
            /// Routine callback provided by the user.
            Action _action;
            /// Underlying pthread.
            boost::thread _thread;
            /// The condition used to block/resume execution.
            std::condition_variable _cond;
            /// The mutex protecting the condition.
            std::mutex _mutex;
            /// The thread that stepped us.
            Thread* _caller;
        };

        /** Pool of thread that can switch execution.
         *
         * All thread are affiliated with a Manager, and can only switch
         * execution to threads in the same manager. The context which
         * instantiated the Manager is a valid thread (the root thread).
         */
        class Manager
        {
          /*-------------.
          | Construction |
          `-------------*/
          public:
            /** Construct a manager.
             *
             * Makes the current execution context the current thread.
             */
            Manager();
            /** Dispose of a manager.
             *
             * All managed thread must be disposed of beforehand.
             */
            ~Manager();

          /*------.
          | State |
          `------*/
          public:
            /// The currently running thread.
            Thread* current() const;

          /*--------.
          | Details |
          `--------*/
          private:
            /// Let threads manipulate the current thread and the root thread.
            friend class Thread;
            /// Root thread, which instantiated the Manager.
            Thread _self;
            /// Current thread.
            Thread* _current;
            /// The condition used to block/resume execution.
            std::condition_variable _cond;
            /// The mutex protecting the condition.
            std::mutex _mutex;
        };
      }
    }
  }
}
