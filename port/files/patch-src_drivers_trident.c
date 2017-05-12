--- src/drivers/trident.c.orig	2005-12-17 20:21:02 UTC
+++ src/drivers/trident.c
@@ -135,7 +135,7 @@ static void getmodeinfo(int mode, vga_mo
 
 static int saveregs(unsigned char regs[])
 { 
-    int temp, i;
+    int temp;
     
     TRIDENTRegPtr tridentReg;
     
@@ -289,7 +289,7 @@ static int saveregs(unsigned char regs[]
 
 static void setregs(const unsigned char regs[], int mode)
 {  
-    int temp,i;
+    int temp;
     TRIDENTRegPtr tridentReg;
     
     tridentReg = (TRIDENTRegPtr)(regs+60);
@@ -873,7 +873,7 @@ static void initializemode(unsigned char
 }
 
 
-static int setmode(int mode, int prv_mode)
+static int trident_setmode(int mode, int prv_mode)
 {
     unsigned char *moderegs;
     ModeTiming *modetiming;
@@ -1152,7 +1152,7 @@ DriverSpecs __svgalib_trident_driverspec
     setpage,
     NULL,
     NULL,
-    setmode,
+    trident_setmode,
     modeavailable,
     setdisplaystart,
     setlogicalwidth,
