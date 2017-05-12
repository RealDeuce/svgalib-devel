--- src/drivers/pm2.c.orig	2005-08-03 13:07:32 UTC
+++ src/drivers/pm2.c
@@ -483,7 +483,7 @@ static void initializemode(unsigned char
 }
 
 
-static int setmode(int mode, int prv_mode)
+static int pm2_setmode(int mode, int prv_mode)
 {
     unsigned char *moderegs;
     ModeTiming *modetiming;
@@ -671,7 +671,7 @@ DriverSpecs __svgalib_pm2_driverspecs =
     NULL,
     NULL,
     NULL,
-    setmode,
+    pm2_setmode,
     modeavailable,
     setdisplaystart,
     setlogicalwidth,
