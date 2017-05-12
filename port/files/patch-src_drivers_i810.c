--- src/drivers/i810.c.orig	2005-08-03 13:07:32 UTC
+++ src/drivers/i810.c
@@ -435,7 +435,7 @@ static void initializemode(unsigned char
 
 static void setdisplaystart(int address);
 
-static int setmode(int mode, int prv_mode)
+static int i810_setmode(int mode, int prv_mode)
 {
     unsigned char *moderegs;
     ModeTiming *modetiming;
@@ -636,7 +636,7 @@ DriverSpecs __svgalib_i810_driverspecs =
     setpage,
     NULL,
     NULL,
-    setmode,
+    i810_setmode,
     modeavailable,
     setdisplaystart,
     setlogicalwidth,
