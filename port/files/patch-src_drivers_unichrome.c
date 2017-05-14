--- src/drivers/unichrome.c.orig	2017-05-14 13:38:38 UTC
+++ src/drivers/unichrome.c
@@ -6,6 +6,8 @@ VIA Unichrome driver 
 #include <stdio.h>		/* for printf */
 #include <string.h>		/* for memset */
 #include <unistd.h>
+#include <math.h>
+#include <limits.h>
 #include "vga.h"
 #include "libvga.h"
 #include "driver.h"
@@ -23,6 +25,7 @@ static void unichrome_lock(void);
 
 static int unichrome_memory, unichrome_io_base;
 static unsigned int unichrome_linear_base;
+static int lastmode = 0;
 
 static CardSpecs *cardspecs;
 typedef struct {
@@ -30,9 +33,9 @@ typedef struct {
     unsigned char crt[0xc0];
 } UNIRegRec, *UNIRegPtr;
 
-enum { CLE266=1, CLE266CX, KM400, KM800, PM800} chiptype;
+enum { CLE266=1, CLE266CX, KM400, KM800, PM800, CN700} chiptype;
 
-//static void unichrome_setpage(int page) {}
+static void unichrome_setpage(int page) {}
 
 /* Fill in chipset specific mode information */
 static void unichrome_getmodeinfo(int mode, vga_modeinfo *modeinfo)
@@ -45,6 +48,7 @@ static void unichrome_getmodeinfo(int mo
     modeinfo->startaddressrange = unichrome_memory * 1024 - 1;
     modeinfo->haveblit = 0;
     modeinfo->flags &= ~HAVE_RWPAGE;
+    __svgalib_modeinfo_linearset |= IS_LINEAR;
 
     if (modeinfo->bytesperpixel >= 1) {
 		if(unichrome_linear_base)modeinfo->flags |= CAPABLE_LINEAR;
@@ -67,6 +71,29 @@ static int unichrome_saveregs(uint8_t re
   return sizeof(UNIRegRec);
 }
 
+void
+rViaMask(uint8_t *regs, uint8_t index, uint8_t value, uint8_t mask)
+{
+	uint8_t tmp;
+
+	tmp = regs[index];
+	tmp &= ~mask;
+	tmp |= (value & mask);
+
+	regs[index] = tmp;
+}
+
+/* Set chipset-specific registers */
+void ViaCrtcMask(int index, int value, int mask) {
+	int tmp;
+	
+	tmp = __svgalib_incrtc(index);
+	tmp &= ~mask;
+	tmp |= (value & mask);
+	
+	__svgalib_outcrtc(index, tmp);
+}
+
 /* Set chipset-specific registers */
 void ViaSeqMask(int index, int value, int mask) {
 	int tmp;
@@ -113,21 +140,12 @@ static int unichrome_modeavailable(int m
     ModeTiming *modetiming;
     ModeInfo *modeinfo;
 
-
-    if (IS_IN_STANDARD_VGA_DRIVER(mode))
-	return __svgalib_vga_driverspecs.modeavailable(mode);
-
     info = &__svgalib_infotable[mode];
     if (unichrome_memory * 1024 < info->ydim * info->xbytes)
 	return 0;
 
     modeinfo = __svgalib_createModeInfoStructureForSvgalibMode(mode);
 
-    if((modeinfo->bitsPerPixel==16)&&(modeinfo->greenWeight==5)) {
-		free(modeinfo);
-        return 0;
-    }
-
     modetiming = malloc(sizeof(ModeTiming));
     
 	if (__svgalib_getmodetiming(modetiming, modeinfo, cardspecs)) {
@@ -160,6 +178,8 @@ static void unichrome_initializemode(uin
 
     __svgalib_setup_VGA_registers(moderegs, modetiming, modeinfo);
 
+	rViaMask(regs->crt, 0x03, 0x80, 0x80);	// enable vertical retrace access
+
     hd  = (modetiming->CrtcHDisplay>>3)-1;
     hss = (modetiming->CrtcHSyncStart>>3);
     hse = (modetiming->CrtcHSyncEnd>>3);
@@ -183,35 +203,56 @@ static void unichrome_initializemode(uin
 	regs->crt[0x35] |= (vbs&0x400)>>9;
 	regs->crt[0x35] |= (vd&0x400)>>8;
 	regs->crt[0x35] |= (vbe&0x400)>>7;
-	
-	of=modeinfo->lineWidth/8;
+
+	of=(modetiming->CrtcHDisplay * (modeinfo->bitsPerPixel >> 3)) >> 3;
 	moderegs[VGA_CR13] = of&0xff;
 	regs->crt[0x35] |= (of&0x700)>>3;
 
 	regs->crt[0x34] = 0; /* start address */
 
+	/* Extended Display Mode */
+	rViaMask(regs->seq, 0x15, 0x02, 0x02);
+
+	/* 6-bit LUT */
+	rViaMask(regs->seq, 0x15, 0x00, 0x80);
+
+	/* "Wrap Around Disable" */
+	rViaMask(regs->seq, 0x15, 0x20, 0x20);
+
     switch (modeinfo->bitsPerPixel) {
 		case 8:
-			regs->seq[0x15] = 0x22;	
+			rViaMask(regs->seq, 0x15, 0x00, 0x1C);
 		    break;
 	    case 15: 
-	    case 16:if(modeinfo->greenWeight==5){
-                        regs->seq[0x15] = 0xb6;
-                    } else regs->seq[0x15] = 0xb6;
+	    case 16:
+			if(modeinfo->greenWeight==5)
+				rViaMask(regs->seq, 0x15, 0x04, 0x1C);
+			else
+				rViaMask(regs->seq, 0x15, 0x14, 0x1C);
 		    break;
 //	    case 24: 
 //               	    regs->vidProcCfg|=2<<18;
 //		    break;
 	    case 32: 
-			regs->seq[0x15] = 0xae;
+			rViaMask(regs->seq, 0x15, 0x1C, 0x1C);
 			break;
 	    default: 
 		    break;
     }
 
+	if (chiptype == CN700)
+		rViaMask(regs->crt, 0x43, 0x00, 0x04);
+
 	tmp=CalcPLL(modetiming->pixelClock, 0);
-	regs->seq[0x46] = tmp>>8;
-	regs->seq[0x47] = tmp&0xff;
+	if (chiptype==CN700) {
+		regs->seq[0x44] = tmp & 0xff;
+		regs->seq[0x45] = (tmp >> 8) & 0xff;
+		regs->seq[0x46] = (tmp >> 16) & 0xff;
+	}
+	else {
+		regs->seq[0x46] = tmp>>8;
+		regs->seq[0x47] = tmp&0xff;
+	}
     
 //	moderegs[VGA_GR5]=0;
 	moderegs[VGA_MISCOUTPUT]|=0x0c; /* pixel clock = pllCtrl0 */
@@ -221,20 +262,30 @@ static void unichrome_initializemode(uin
 	regs->seq[0x1a] = 0x08;
 
 	if(hd>=1600) {
-		regs->seq[0x16] = 0x0f;	
+		rViaMask(regs->seq, 0x16, 0x0f, 0xff /*0xbf*/);
 		regs->seq[0x18] = 0x4f;
 	} else if(hd>=1024) {
-		regs->seq[0x16] = 0x0c;	
+		rViaMask(regs->seq, 0x16, 0x0c, 0xff /*0xbf*/);
 		regs->seq[0x18] = 0x4c;
 	} else {
-		regs->seq[0x16] = 0x08;	
+		rViaMask(regs->seq, 0x16, 0x08, 0xff /*0xbf*/);
 		regs->seq[0x18] = 0x4e;
 	}
 
+	if (chiptype==CN700) {
+		regs->seq[0x17] = 0x2f;
+		rViaMask(regs->seq, 0x16, 0x14, 0xbf);
+		rViaMask(regs->seq, 0x18, 0x08, 0xbf);
+		if (hd >= 1400 && modeinfo->bitsPerPixel == 32)
+			rViaMask(regs->seq, 0x22, 0x10, 0x1f);
+		else
+			rViaMask(regs->seq, 0x22, 0x00, 0x1f);
+	}
+
 	/* Fetch count */
-	if(of&3)of=(of+3)&~3;
-	regs->seq[0x1c] = (of>>1)&0xff;
-	regs->seq[0x1d] = (of>>9)&0xff;
+	of = (modetiming->CrtcHDisplay * (modeinfo->bitsPerPixel >> 3)) >> 4;
+	regs->seq[0x1c] = (of)&0xff;
+	rViaMask(regs->seq, 0x1d, of>>8, 0x03);
 
 }
 
@@ -244,10 +295,6 @@ static int unichrome_setmode(int mode, i
     ModeTiming modetiming;
     ModeInfo *modeinfo;
 
-    if (IS_IN_STANDARD_VGA_DRIVER(mode)) {
-		return __svgalib_vga_driverspecs.setmode(mode, prv_mode);
-    }
-    
 	if (!unichrome_modeavailable(mode))
 		return 1;
 
@@ -268,6 +315,8 @@ static int unichrome_setmode(int mode, i
 
     free(modeinfo);
 
+	lastmode = mode;
+
     return 0;
 }
 
@@ -298,6 +347,11 @@ static int unichrome_test(void)
 
 static void unichrome_setdisplaystart(int address)
 { 
+
+    vga_modeinfo *modeinfo;
+    modeinfo = vga_getmodeinfo(lastmode);
+	if (chiptype == CN700)
+		__svgalib_outcrtc(0x48, ((address>>2) & 0x1F000000) >> 24);
   __svgalib_outcrtc(0x23, ((address>>2) & 0xFF0000)>>16);
   __svgalib_outcrtc(0x0c, ((address>>2) & 0x00FF00)>>8);
   __svgalib_outcrtc(0x0d, (address>>2) & 0x00FF);
@@ -444,7 +498,8 @@ static int unichrome_init(int force, int
             ((buf[0]>>16)==0x3108)||
             ((buf[0]>>16)==0x3118)||
             ((buf[0]>>16)==0x3122)||
-            ((buf[0]>>16)==0x7205));
+            ((buf[0]>>16)==0x7205)||
+            ((buf[0]>>16)==0x3344));
 	if(!found) return 1;
 
     switch(buf[0]>>16) {
@@ -461,6 +516,9 @@ static int unichrome_init(int force, int
 		case 0x7205:
 			chiptype=KM400;
 			break;
+		case 0x3344:
+			chiptype=CN700;
+			break;
 	}		
     
     if (found){
@@ -496,7 +554,17 @@ static int unichrome_init(int force, int
     __svgalib_driverspecs = &__svgalib_unichrome_driverspecs;
     __svgalib_linear_mem_base=unichrome_linear_base;
     __svgalib_linear_mem_size=unichrome_memory*0x400;
-    __svgalib_emulatepage = 2;
+    if (chiptype == CN700) {
+       __svgalib_emulatepage = 0;
+	__svgalib_banked_mem_base=unichrome_linear_base;
+	__svgalib_banked_mem_size=__svgalib_linear_mem_size;
+	__svgalib_unichrome_driverspecs.__svgalib_setpage = unichrome_setpage;
+	__svgalib_unichrome_driverspecs.__svgalib_setrdpage = unichrome_setpage;
+	__svgalib_unichrome_driverspecs.__svgalib_setwrpage = unichrome_setpage;
+	__svgalib_modeinfo_linearset = LINEAR_CAN;
+    }
+    else
+       __svgalib_emulatepage = 2;
     return 0;
 }
 
@@ -507,6 +575,37 @@ CalcPLL(int freq, int isBanshee) {
 	int m, n, k, best_m, best_n, best_k, f_cur, best_error;
 	int minm, maxm, minn, maxn;
 
+	if (chiptype==CN700) {
+		uint32_t BestDivider = 0, BestShift = 0, BestMultiplier = 0;
+		uint32_t Divider, Shift, Multiplier;
+		uint32_t RoundUp;
+		long long BestDiff = INT_MAX, Diff;
+
+		for (Shift = 0; Shift < 4; Shift++) {
+			for (Divider = 2; Divider < 33; Divider++) {
+				for (RoundUp = 0; RoundUp < 2; RoundUp++) {
+					if (RoundUp)
+						Multiplier = (((freq * Divider) << Shift) + 14317) / 14318;
+					else
+						Multiplier = ((freq * Divider) << Shift) / 14318;
+
+					if ((Multiplier > 257) || (Multiplier < 2))
+						continue;
+
+					Diff = llabs((long)((14318000 * Multiplier) / (Divider << Shift)) - (freq * 1000));
+
+					if (Diff < BestDiff) {
+						BestDiff = Diff;
+						BestMultiplier = Multiplier;
+						BestDivider = Divider;
+						BestShift = Shift;
+					}
+				}
+			}
+		}
+
+		return ((BestDivider - 2) << 16) | (BestShift << 10) | (BestMultiplier - 2);
+	} else {
   	best_error=freq;
   	best_n=best_m=best_k=0;
 
@@ -543,6 +642,7 @@ CalcPLL(int freq, int isBanshee) {
 			best_k=k;
 		}
   	}
+	}
 	
   	return best_n|(best_m<<8)|(best_k<<14);
 }
