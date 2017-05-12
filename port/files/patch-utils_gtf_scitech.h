--- utils/gtf/scitech.h.orig	2017-05-12 07:10:58 UTC
+++ utils/gtf/scitech.h
@@ -49,7 +49,7 @@
  *  __UNIX__   		Compiling for Unix
  *  __QNX__         Compiling for the QNX realtime OS (Unix compatible)
  *  __LINUX__		Compiling for the Linux OS (Unix compatible)
- *  __FREEBSD__		Compiling for the FreeBSD OS (Unix compatible)
+ *  __FreeSD__		Compiling for the FreeBSD OS (Unix compatible)
  *	__BEOS__		Compiling for the BeOS (Unix compatible)
  *  __SMX32__       Compiling for the SMX 32-bit Real Time OS
  *  __DRIVER__      Compiling for a 32-bit binary compatible driver
@@ -228,7 +228,7 @@
 #endif
 
 /* 32-bit FreeBSD compile environment */
-#elif	defined(__FREEBSD__)
+#elif	defined(__FreeBSD__)
 #ifndef	__32BIT__
 #define __32BIT__
 #endif
