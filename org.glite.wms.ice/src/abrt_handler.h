/**
 * This source file is used to print out a stack-trace when your program
 * segfaults. It is relatively reliable and spot-on accurate.
 *
 * This code is in the public domain. Use it as you see fit, some credit
 * would be appreciated, but is not a prerequisite for usage. Feedback
 * on it's use would encourage further development and maintenance.
 *
 * Due to a bug in gcc-4.x.x you currently have to compile as C++ if you want
 * demangling to work.
 *
 * Please note that it's been ported into my ULS library, thus the check for
 * HAS_ULSLIB and the use of the sigabrt_outp macro based on that define.
 *
 * Author: Jaco Kroon <jaco@kroon.co.za>
 *
 * Copyright (C) 2005 - 2009 Jaco Kroon
 */

#include "config.h"

#ifdef RUN_ON_LINUX
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* Bug in gcc prevents from using CPP_DEMANGLE in pure "C" */
#if !defined(__cplusplus) && !defined(NO_CPP_DEMANGLE)
#define NO_CPP_DEMANGLE
#endif

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <ucontext.h>
#include <dlfcn.h>
#ifndef NO_CPP_DEMANGLE
#include <cxxabi.h>
#ifdef __cplusplus
using __cxxabiv1::__cxa_demangle;
#endif
#endif

#define sigabrt_outp(x, ...)    fprintf(outfile, x "\n", ##__VA_ARGS__)

/* extern char **argv; */
/* extern int argc; */

#if defined(REG_RIP)
# define SIGABRT_STACK_IA64
# define REGFORMAT "%016lx"
#elif defined(REG_EIP)
# define SIGABRT_STACK_X86
# define REGFORMAT "%08x"
#else
# define SIGABRT_STACK_GENERIC
# define REGFORMAT "%x"
#endif

void signal_abrt(int signum, siginfo_t* info, void*ptr) {

	int i, f = 0;
	ucontext_t *ucontext = (ucontext_t*)ptr;
	Dl_info dlinfo;
	void **bp = 0;
	void *ip = 0;
	static const char *si_codes[3] = {"", "ABRT_MAPERR", "ABRT_ACCERR"};

  FILE *outfile = fopen("/tmp/ice_sigabrt_report", "w+");

  if (!outfile)
    outfile = stderr;

  sigabrt_outp("Abort!");
  sigabrt_outp("info.si_signo = %d", signum);
  sigabrt_outp("info.si_errno = %d", info->si_errno);
  sigabrt_outp("info.si_code  = %d (%s)", info->si_code, si_codes[info->si_code]);
  sigabrt_outp("info.si_addr  = %p", info->si_addr);

#if  defined(SIGABRT_STACK_IA64) || defined(SIGABRT_STACK_X86)
#if defined(SIGABRT_STACK_IA64)
  ip = (void*)ucontext->uc_mcontext.gregs[REG_RIP];
  bp = (void**)ucontext->uc_mcontext.gregs[REG_RBP];
#elif defined(SIGABRT_STACK_X86)
  ip = (void*)ucontext->uc_mcontext.gregs[REG_EIP];
  bp = (void**)ucontext->uc_mcontext.gregs[REG_EBP];
#endif

  sigabrt_outp("Stack trace:");
  while(bp && ip) {
    if(!dladdr(ip, &dlinfo))
      break;

    {
      const char *symname = dlinfo.dli_sname;

#ifndef NO_CPP_DEMANGLE
      int status;
      char * tmp = __cxa_demangle(symname, NULL, 0, &status);

      if (status == 0 && tmp)
        symname = tmp;
#endif

      sigabrt_outp("% 2d: %p <%s+%lu> (%s)",
                   ++f,
                   ip,
                   symname,
                   (unsigned long)ip - (unsigned long)dlinfo.dli_saddr,
                   dlinfo.dli_fname);

#ifndef NO_CPP_DEMANGLE
      if (tmp)
        free(tmp);
#endif
    }

    if(dlinfo.dli_sname && !strcmp(dlinfo.dli_sname, "main"))
      break;

    ip = bp[1];
    bp = (void**)bp[0];
  }
#else
  sigabrt_outp("Stack trace (non-dedicated):");
  {
    int sz;
    char *bt[21];
    char **strings;

    sz = backtrace(bt, 20);
    strings = backtrace_symbols(bt, sz);
    for(i = 0; i < sz; ++i) {
      sigabrt_outp("%s", strings[i]);
    }
  }
#endif
  sigabrt_outp("End of stack trace.");

  if (outfile != stderr)
    fclose(outfile);
  

  fprintf(stderr, "Segmentation Fault!\nThe program had a serious failure.\nIf you wish to help the developers fix it,\nplease send the /tmp/ice_sigabrt_report file\n to a@cnaf.infn.it.\nThe file contains no personally identifying informations.\nThanks for your help!\n");

	_exit (-1);
}

void __attribute__((constructor)) setup_sigabrt() {
	struct sigaction action;
	memset(&action, 0, sizeof(action));
	action.sa_sigaction = signal_abrt;
	action.sa_flags = SA_SIGINFO;
	if(sigaction(SIGABRT, &action, NULL) < 0)
		perror("sigaction");
}
#else
//static int dummy = 0;
#endif
