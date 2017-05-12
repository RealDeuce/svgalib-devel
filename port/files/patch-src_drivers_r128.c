--- src/drivers/r128.c.orig	2006-07-14 11:49:50 UTC
+++ src/drivers/r128.c
@@ -19,11 +19,13 @@ Rage 128 chipset driver
 #include "r128_reg.h"
 #include "xf86PciInfo.h"
 
+#if 0
 static int RADEONProbePLLParameters(void);
+#endif
 
 static int id;
 
-static enum { Rage128=0, Radeon } chiptype; /* r128io needs to know */
+static enum { UnknownChip=-1, Rage128=0, Radeon } chiptype=-1; /* r128io needs to know */
 static enum {
 	CHIP_FAMILY_UNKNOW,
 	CHIP_FAMILY_LEGACY,
@@ -93,7 +95,9 @@ typedef int Bool;
 static int r128_ramtype;
 static int BusCntl, CRTOnly, HasPanelRegs, IsIGP, IsMobility, HasSingleDAC, HasCRTC2;
 
+#if 0
 static float mclk, sclk;
+#endif
 static uint32_t ChipErrata;
 
 typedef struct {
@@ -400,7 +404,8 @@ static void R128PLLWaitForReadUpdateComp
 
 static void R128PLLWriteUpdate(void)
 {
-	while (INPLL ( R128_PPLL_REF_DIV) & R128_PPLL_ATOMIC_UPDATE_R);
+	while (INPLL ( R128_PPLL_REF_DIV) & R128_PPLL_ATOMIC_UPDATE_R)
+		;
     OUTPLLP(R128_PPLL_REF_DIV, R128_PPLL_ATOMIC_UPDATE_W, ~(R128_PPLL_ATOMIC_UPDATE_W));
 }
 
@@ -553,6 +558,8 @@ static void R128RestoreMode(R128SavePtr 
         case Radeon:
             RADEONRestorePLLRegisters(restore);
             break;
+	case UnknownChip:
+	    break;
     }
             
     if(chiptype == Rage128) {
@@ -1120,6 +1127,8 @@ static Bool R128Init(ModeTiming *mode, M
         case Radeon:
             RADEONInitPLLRegisters(save, &pll, dot_clock);
             break;
+	case UnknownChip:
+	    break;
     }
     
 	if(chiptype == Rage128) {
@@ -1467,7 +1476,7 @@ static int r128_init(int force, int par1
     unsigned char *BIOS_POINTER;
 
     r128_memory=0;
-	chiptype=-1;
+	chiptype=UnknownChip;
     if (force) {
 		r128_memory = par1;
         chiptype = par2;
@@ -1520,7 +1529,7 @@ static int r128_init(int force, int par1
 		(id == 0x5354)
 		) return 1; /* Mach64/Mach32 */
 	
-    if(chiptype==-1) return 1;
+    if(chiptype==UnknownChip) return 1;
     
 	ChipFamily = CHIP_FAMILY_LEGACY;
 	if(chiptype == Radeon) {
@@ -1844,6 +1853,8 @@ static int r128_init(int force, int par1
 				if (pll.reference_div < 2) pll.reference_div = 12;
 										
                 break;
+	    case UnknownChip:
+		break;
         }
     } 
 #if 0
@@ -1887,6 +1898,7 @@ fprintf(stderr,"pll: %i %i %i %i %i\n",p
     return 0;
 }
 
+#if 0
 static void xf86getsecs(long * secs, long * usecs)
 {
 	struct timeval tv;
@@ -2103,5 +2115,4 @@ static int RADEONProbePLLParameters(void
 
     return 1;
 }
-
-
+#endif
