--- utils/setmclk.c.orig	2005-07-02 15:41:30 UTC
+++ utils/setmclk.c
@@ -39,8 +39,8 @@
 #include <stdlib.h>
 #include <unistd.h>
 #include <vga.h>
-#include <sys/io.h>	/* For port I/O macros. */
-#define OUTB(a,d) outb(d,a)
+#include <machine/cpufunc.h>	/* For port I/O macros. */
+#define OUTB(a,d) outb(a,d)
 
 int
 main (void)
