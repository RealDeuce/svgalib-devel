--- src/vga_helper.c.orig	2005-07-10 11:18:32 UTC
+++ src/vga_helper.c
@@ -1,5 +1,6 @@
-#include <sys/io.h>
+#include <sys/types.h>
 #include <sys/ioctl.h>
+#include <machine/cpufunc.h>
 #include "svgalib_helper.h"
 #include "libvga.h"
 
@@ -25,7 +26,7 @@ void __svgalib_port_out(int value, int p
 {
   if(__svgalib_nohelper)
   {
-    outb(value, port);
+    outb(port, value);
   }
   else
   {
@@ -41,7 +42,7 @@ void __svgalib_port_outw(int value, int 
 {
   if(__svgalib_nohelper)
   {
-    outw(value, port);
+    outw(port, value);
   }
   else
   {
@@ -57,7 +58,7 @@ void __svgalib_port_outl(int value, int 
 {
   if(__svgalib_nohelper)
   {
-    outl(value, port);
+    outl(port, value);
   }
   else
   {
