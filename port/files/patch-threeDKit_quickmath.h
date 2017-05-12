--- threeDKit/quickmath.h.orig	2005-08-03 13:07:32 UTC
+++ threeDKit/quickmath.h
@@ -60,7 +60,7 @@ typedef struct {
 #define max(x,y)     (((x) > (y)) ? (x) : (y))
 #define min(x,y)     (((x) < (y)) ? (x) : (y))
 
-#ifndef __GNUC__
+#if !defined(__GNUC__) || defined(__clang__)
 
 double fsqr (double x);
 int lsqr (int x);
