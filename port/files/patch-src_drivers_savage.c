--- src/drivers/savage.c.orig	2005-08-03 13:07:32 UTC
+++ src/drivers/savage.c
@@ -152,11 +152,6 @@ static void setpage(int page)
     __svgalib_outcrtc(0x6a, page);
 }
 
-static int inlinearmode(void)
-{
-return is_linear;
-}
-
 /* Fill in chipset specific mode information */
 
 static void getmodeinfo(int mode, vga_modeinfo *modeinfo)
@@ -913,7 +908,7 @@ static void initializemode(unsigned char
 }
 
 
-static int setmode(int mode, int prv_mode)
+static int savage_setmode(int mode, int prv_mode)
 {
     unsigned char *moderegs;
     ModeTiming *modetiming;
@@ -1226,7 +1221,7 @@ DriverSpecs __svgalib_savage_driverspecs
     setpage,
     NULL,
     NULL,
-    setmode,
+    savage_setmode,
     modeavailable,
     setdisplaystart,
     setlogicalwidth,
