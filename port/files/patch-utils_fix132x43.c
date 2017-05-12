--- utils/fix132x43.c.orig	2005-07-02 15:41:37 UTC
+++ utils/fix132x43.c
@@ -47,8 +47,8 @@
 #include <string.h>
 #include <unistd.h>
 #include <vga.h>
-#include "sys/io.h"	/* For port I/O macros. */
-#define OUTB(a,d) outb(d,a)
+#include <machine/cpufunc.h>	/* For port I/O macros. */
+#define OUTB(a,d) outb(a,d)
 
 static void fixfont (int);
 
