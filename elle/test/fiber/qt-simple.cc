//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        Alexandre Bique   [fri sep 23 19:25:36 2011]
//

#include <QCoreApplication>
#include <QTimer>

#include "QtObj.hh"

#include <elle/Elle.hh>

using namespace elle;

Status fiber2()
{
  enter();
  log_here;
  leave();
}

Status fiber1()
{
  enter();
  log_here;
  Closure< Status,
           Parameters<> >       closure(Callback<>::Infer(&fiber2));

  Fiber::Spawn(closure);
  log_here;
  leave();
}

ucontext_t auc,buc,mainuc;

void a()
{
  log_here;
  swapcontext(&auc, &buc);         /* switch to main thread */
  log_here;
  swapcontext(&auc, &buc);         /* switch to main thread */
  log_here;
  swapcontext(&auc, &mainuc);         /* switch to main thread */
}

void b()
{
  log_here;
  swapcontext(&buc, &auc);         /* switch to main thread */
  log_here;
  swapcontext(&buc, &auc);         /* switch to main thread */
  log_here;
}

void
QtObj::tetons()
{
#if 0
  printf("start\n");                  /* main thread starts */

  /* Set up context for thread A (Unix code, see manpages) */
  getcontext(&auc);
  auc.uc_stack.ss_size = 16 * 1024;

  if ((auc.uc_stack.ss_sp = malloc(auc.uc_stack.ss_size)) == NULL)
    perror("malloc"), exit(1);

  auc.uc_stack.ss_flags = 0;
  makecontext(&auc, a, 0);

  /* Set up context for thread B */
  getcontext(&buc);
  buc.uc_stack.ss_size = 16 * 1024;

  if ((buc.uc_stack.ss_sp = malloc(buc.uc_stack.ss_size)) == NULL)
    perror("malloc"), exit(1);

  buc.uc_stack.ss_flags = 0;
  makecontext(&buc, b, 0);

  /* Switch to A */
  getcontext(&mainuc);           /* Save the context of main thread */
  swapcontext(&mainuc, &auc);    /* Switch to thread A */

  printf("\ndone\n");  /* Execution control returned to main thread */
#endif

  log_here;
  Fiber::Initialize();
  Closure< Status,
           Parameters<> >       closure(Callback<>::Infer(&fiber1));

  Fiber::Spawn(closure);
  log_here;
  QCoreApplication::quit();
}

int main(int argc, char ** argv)
{
  QCoreApplication app(argc, argv);

  QtObj obj;

  QTimer::singleShot(100, &obj, SLOT(tetons()));
  return app.exec();
}
