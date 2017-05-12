--- demos/linearfork.c.orig	2000-01-06 12:12:36 UTC
+++ demos/linearfork.c
@@ -18,7 +18,7 @@ static char sig2release[] =
  SIGTRAP, SIGIOT, SIGBUS, SIGFPE,
  SIGSEGV, SIGPIPE, SIGALRM, SIGTERM,
  SIGXCPU, SIGXFSZ, SIGVTALRM,
- SIGPROF, SIGPWR};
+ SIGPROF, SIGUSR1};
 
 
 static int newcolor(void)
