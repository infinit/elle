//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        Alexandre Bique   [wed sep 21 20:12:51 2011]
//

// http://www.codeproject.com/KB/threads/ucontext.aspx
/* testcontext.c : demo of ucontex_t operations */

#include <stdio.h>

#include <stdlib.h>

#include <elle/system/Platform.hh>
#if INFINIT_UNIX
# include <ucontext.h>
#else INFINIT_WIN32
# include <elle/thirdparty/ucontext-win32.hh>
#endif

ucontext_t auc,buc,mainuc;

void a()
{
    int i;

    for (i = 0; i < 10; i++)
    {
        printf("a");
        swapcontext(&auc, &buc);        /* switch to thread B */
    }

    printf("\nswitching to main\n");
    swapcontext(&auc, &mainuc);         /* switch to main thread */
}

void b()
{
    int i;

    for (i = 0; i < 10; i++)
    {
        printf("b");
        swapcontext(&buc, &auc);        /* switch to thread A */
    }
}

int main(void)
{
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
    return 0;
}
