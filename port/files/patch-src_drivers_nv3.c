--- src/drivers/nv3.c.orig	2006-04-27 21:44:43 UTC
+++ src/drivers/nv3.c
@@ -398,7 +398,7 @@ static void initializemode(unsigned char
 }
 
 
-static int setmode(int mode, int prv_mode)
+static int nv3_setmode(int mode, int prv_mode)
 {
     unsigned char *moderegs;
     ModeTiming *modetiming;
@@ -631,7 +631,7 @@ DriverSpecs __svgalib_nv3_driverspecs =
     setpage,
     0,
     0,
-    setmode,
+    nv3_setmode,
     modeavailable,
     setdisplaystart,
     setlogicalwidth,
@@ -878,7 +878,7 @@ static int CalcVClock
     for (P = 0; P <= P_MAX; P ++)
     {
         Freq = VClk << P;
-        if ((Freq >= 128000) && (Freq <= MAXVCLOCK))
+        if ((Freq >= 128000 || P == P_MAX) && (Freq <= MAXVCLOCK))
         {
             for (M = M_MIN; M <= M_MAX; M++)
             {
