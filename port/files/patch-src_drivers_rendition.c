--- src/drivers/rendition.c.orig	2005-07-29 15:18:03 UTC
+++ src/drivers/rendition.c
@@ -330,7 +330,7 @@ static void initializemode(unsigned char
 }
 
 
-static int setmode(int mode, int prv_mode)
+static int rendition_setmode(int mode, int prv_mode)
 {
     unsigned char *moderegs;
     ModeTiming *modetiming;
@@ -463,7 +463,7 @@ DriverSpecs __svgalib_rendition_driversp
     setpage,
     NULL,
     NULL,
-    setmode,
+    rendition_setmode,
     modeavailable,
     setdisplaystart,
     setlogicalwidth,
