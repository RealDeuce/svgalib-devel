--- gl/driver.c.orig	2005-08-03 13:07:31 UTC
+++ gl/driver.c
@@ -1,6 +1,6 @@
 /* driver.c     Framebuffer primitives */
 
-
+#include <sys/endian.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
@@ -33,22 +33,12 @@ static inline int RGB2BGR(int c)
 /* However bswap is not supported by 386 */
 
     if (MODEFLAGS & MODEFLAG_24BPP_REVERSED)
-#ifdef NO_ASSEMBLY
-	c = ((c >> 0) & 0xff) << 16 |
-	    ((c >> 8) & 0xff) << 8 |
-	    ((c >> 16) & 0xff) << 0;
-#else
-	asm("rorw  $8, %0\n"	/* 0RGB -> 0RBG */
-	    "rorl $16, %0\n"	/* 0RBG -> BG0R */
-	    "rorw  $8, %0\n"	/* BG0R -> BGR0 */
-	    "shrl  $8, %0\n"	/* 0BGR -> 0BGR */
-      : "=q"(c):"0"(c));
-#endif
+	c = bswap32(c) >> 8;
     return c;
 }
 
 /* RGB_swapped_memcopy returns the amount of bytes unhandled */
-static inline int RGB_swapped_memcpy(char *dest, char *source, int len)
+static inline int RGB_swapped_memcpy(uchar *dest, uchar *source, int len)
 {
     int rest, tmp;
 
@@ -80,7 +70,7 @@ static void notimplemented(char *s)
 
 void __svgalib_driver8_setpixel(int x, int y, int c)
 {
-    char *vp;
+    uchar *vp;
     ASSIGNVP8(x, y, vp);
     *vp = c;
 }
@@ -95,7 +85,7 @@ void __svgalib_driver8p_setpixel(int x, 
 
 int __svgalib_driver8_getpixel(int x, int y)
 {
-    char *vp;
+    uchar *vp;
     ASSIGNVP8(x, y, vp);
     return *vp;
 }
@@ -110,7 +100,7 @@ int __svgalib_driver8p_getpixel(int x, i
 
 void __svgalib_driver8_hline(int x1, int y, int x2, int c)
 {
-    char *vp;
+    uchar *vp;
     ASSIGNVP8(x1, y, vp);
     __memset(vp, c, x2 - x1 + 1);
 }
@@ -118,7 +108,7 @@ void __svgalib_driver8_hline(int x1, int
 void __svgalib_driver8p_hline(int x1, int y, int x2, int c)
 {
     int vp;
-    char *rvp;
+    uchar *rvp;
     int l;
     int chunksize, page;
     ASSIGNVPOFFSET8(x1, y, vp);
@@ -135,7 +125,7 @@ void __svgalib_driver8p_hline(int x1, in
 
 void __svgalib_driver8_fillbox(int x, int y, int w, int h, int c)
 {
-    char *vp;
+    uchar *vp;
     int i;
     ASSIGNVP8(x, y, vp);
     for (i = 0; i < h; i++) {
@@ -185,8 +175,8 @@ void __svgalib_driver8p_fillbox(int x, i
 
 void __svgalib_driver8_putbox(int x, int y, int w, int h, void *b, int bw)
 {
-    char *vp;			/* screen pointer */
-    char *bp;			/* bitmap pointer */
+    uchar *vp;			/* screen pointer */
+    uchar *bp;			/* bitmap pointer */
     int i;
     ASSIGNVP8(x, y, vp);
     bp = b;
@@ -202,7 +192,7 @@ void __svgalib_driver8p_putbox(int x, in
 /* extra argument width of source bitmap, so that putboxpart can use this */
     int vp;
     int page;
-    char *bp = b;
+    uchar *bp = b;
     int i;
     ASSIGNVPOFFSET8(x, y, vp);
     page = vp >> 16;
@@ -233,8 +223,8 @@ void __svgalib_driver8p_putbox(int x, in
 
 void __svgalib_driver8_getbox(int x, int y, int w, int h, void *b, int bw)
 {
-    char *vp;			/* screen pointer */
-    char *bp;			/* bitmap pointer */
+    uchar *vp;			/* screen pointer */
+    uchar *bp;			/* bitmap pointer */
     int i;
     ASSIGNVP8(x, y, vp);
     bp = b;
@@ -249,7 +239,7 @@ void __svgalib_driver8p_getbox(int x, in
 {
     int vp;
     int page;
-    char *bp = b;
+    uchar *bp = b;
     int i;
     ASSIGNVPOFFSET8(x, y, vp);
     page = vp >> 16;
@@ -339,7 +329,7 @@ void __svgalib_driver8p_getboxpart(int x
 
 void __svgalib_driver8_copybox(int x1, int y1, int w, int h, int x2, int y2)
 {
-    char *svp, *dvp;
+    uchar *svp, *dvp;
     /* I hope this works now. */
     if (y1 >= y2) {
 	if (y1 == y2 && x2 >= x1) {	/* tricky */
@@ -393,7 +383,7 @@ void __svgalib_driver8a_copybox(int x1, 
 
 void __svgalib_driver16_setpixel(int x, int y, int c)
 {
-    char *vp;
+    uchar *vp;
     ASSIGNVP16(x, y, vp);
     *(unsigned short *) vp = c;
 }
@@ -408,7 +398,7 @@ void __svgalib_driver16p_setpixel(int x,
 
 int __svgalib_driver16_getpixel(int x, int y)
 {
-    char *vp;
+    uchar *vp;
     ASSIGNVP16(x, y, vp);
     return *(unsigned short *) vp;
 }
@@ -423,7 +413,7 @@ int __svgalib_driver16p_getpixel(int x, 
 
 void __svgalib_driver16_hline(int x1, int y, int x2, int c)
 {
-    char *vp;
+    uchar *vp;
     ASSIGNVP16(x1, y, vp);
     __memset2(vp, c, x2 - x1 + 1);
 }
@@ -431,7 +421,7 @@ void __svgalib_driver16_hline(int x1, in
 void __svgalib_driver16p_hline(int x1, int y, int x2, int c)
 {
     int vp;
-    char *rvp;
+    uchar *rvp;
     int l;
     int chunksize, page;
     ASSIGNVPOFFSET16(x1, y, vp);
@@ -448,7 +438,7 @@ void __svgalib_driver16p_hline(int x1, i
 
 void __svgalib_driver16_fillbox(int x, int y, int w, int h, int c)
 {
-    char *vp;
+    uchar *vp;
     int i;
     ASSIGNVP16(x, y, vp);
     for (i = 0; i < h; i++) {
@@ -488,8 +478,8 @@ void __svgalib_driver16p_fillbox(int x, 
 
 void __svgalib_driver16_putbox(int x, int y, int w, int h, void *b, int bw)
 {
-    char *vp;			/* screen pointer */
-    char *bp;			/* bitmap pointer */
+    uchar *vp;			/* screen pointer */
+    uchar *bp;			/* bitmap pointer */
     int i;
     ASSIGNVP16(x, y, vp);
     bp = b;
@@ -507,8 +497,8 @@ void __svgalib_driver16p_putbox(int x, i
 
 void __svgalib_driver16_getbox(int x, int y, int w, int h, void *b, int bw)
 {
-    char *vp;			/* screen pointer */
-    char *bp;			/* bitmap pointer */
+    uchar *vp;			/* screen pointer */
+    uchar *bp;			/* bitmap pointer */
     int i;
     ASSIGNVP16(x, y, vp);
     bp = b;
@@ -608,7 +598,7 @@ void __svgalib_driver16a_copybox(int x1,
 
 void __svgalib_driver24_setpixel(int x, int y, int c)
 {
-    char *vp;
+    uchar *vp;
     c = RGB2BGR(c);
     ASSIGNVP24(x, y, vp);
     *(unsigned short *) vp = c;
@@ -618,7 +608,7 @@ void __svgalib_driver24_setpixel(int x, 
 void __svgalib_driver24p_setpixel(int x, int y, int c)
 {
     int vp, vpo;
-    char *vbuf;
+    uchar *vbuf;
     int page;
     c = RGB2BGR(c);
     ASSIGNVPOFFSET24(x, y, vp);
@@ -642,7 +632,7 @@ void __svgalib_driver24p_setpixel(int x,
 
 int __svgalib_driver24_getpixel(int x, int y)
 {
-    char *vp;
+    uchar *vp;
     ASSIGNVP24(x, y, vp);
     return RGB2BGR(*(unsigned short *) vp + (*(unsigned char *) (vp + 2) << 16));
 }
@@ -650,7 +640,7 @@ int __svgalib_driver24_getpixel(int x, i
 int __svgalib_driver24p_getpixel(int x, int y)
 {
     int vp, vpo;
-    char *vbuf;
+    uchar *vbuf;
     int page;
     ASSIGNVPOFFSET24(x, y, vp);
     vbuf = VBUF;
@@ -675,7 +665,7 @@ int __svgalib_driver24p_getpixel(int x, 
 
 void __svgalib_driver24_hline(int x1, int y, int x2, int c)
 {
-    char *vp;
+    uchar *vp;
     c = RGB2BGR(c);
     ASSIGNVP24(x1, y, vp);
     if (RGBEQUAL(c))
@@ -687,7 +677,7 @@ void __svgalib_driver24_hline(int x1, in
 void __svgalib_driver24p_hline(int x1, int y, int x2, int c)
 {
     int vp;
-    char *rvp;
+    uchar *rvp;
     int l;
     int chunksize, page;
     c = RGB2BGR(c);
@@ -722,7 +712,7 @@ void __svgalib_driver24p_hline(int x1, i
 
 void __svgalib_driver24_fillbox(int x, int y, int w, int h, int c)
 {
-    char *vp;
+    uchar *vp;
     int i, j;
     c = RGB2BGR(c);
     ASSIGNVP24(x, y, vp);
@@ -806,8 +796,8 @@ void __svgalib_driver24p_fillbox(int x, 
 
 void __svgalib_driver24_putbox(int x, int y, int w, int h, void *b, int bw)
 {
-    char *vp;			/* screen pointer */
-    char *bp;			/* bitmap pointer */
+    uchar *vp;			/* screen pointer */
+    uchar *bp;			/* bitmap pointer */
     int i;
     ASSIGNVP24(x, y, vp);
     bp = b;
@@ -831,7 +821,7 @@ static void driver24_rev_putbox(int x, i
 /* extra argument width of source bitmap, so that putboxpart can use this */
     int vp;
     int page;
-    char *bp = b, *bp2;
+    uchar *bp = b, *bp2;
     int i, left;
 
     ASSIGNVPOFFSET8(x, y, vp);
@@ -897,8 +887,8 @@ INLINE void __svgalib_driver24p_putbox(i
 
 void __svgalib_driver24_putbox32(int x, int y, int w, int h, void *b, int bw)
 {
-    char *vp;			/* screen pointer */
-    char *bp;			/* bitmap pointer */
+    uchar *vp;			/* screen pointer */
+    uchar *bp;			/* bitmap pointer */
     int i;
     ASSIGNVP24(x, y, vp);
     bp = b;
@@ -911,8 +901,8 @@ void __svgalib_driver24_putbox32(int x, 
 
 void __svgalib_driver24_getbox(int x, int y, int w, int h, void *b, int bw)
 {
-    char *vp;			/* screen pointer */
-    char *bp;			/* bitmap pointer */
+    uchar *vp;			/* screen pointer */
+    uchar *bp;			/* bitmap pointer */
     int i;
     ASSIGNVP24(x, y, vp);
     bp = b;
@@ -936,7 +926,7 @@ static void driver24_rev_getbox(int x, i
 /* extra argument width of source bitmap, so that putboxpart can use this */
     int vp;
     int page;
-    char *bp = b, *bp2;
+    uchar *bp = b, *bp2;
     int i, left;
 
     ASSIGNVPOFFSET8(x, y, vp);
@@ -1103,7 +1093,7 @@ void __svgalib_driver24a_copybox(int x1,
 
 void __svgalib_driver32_setpixel(int x, int y, int c)
 {
-    char *vp;
+    uchar *vp;
     ASSIGNVP32(x, y, vp);
     *(unsigned *) vp = c;
 }
@@ -1118,7 +1108,7 @@ void __svgalib_driver32p_setpixel(int x,
 
 int __svgalib_driver32_getpixel(int x, int y)
 {
-    char *vp;
+    uchar *vp;
     ASSIGNVP32(x, y, vp);
     return *(unsigned *) vp;
 }
@@ -1133,7 +1123,7 @@ int __svgalib_driver32p_getpixel(int x, 
 
 void __svgalib_driver32_hline(int x1, int y, int x2, int c)
 {
-    char *vp;
+    uchar *vp;
     ASSIGNVP32(x1, y, vp);
     __memset4(vp, c, x2 - x1 + 1);
 }
@@ -1141,7 +1131,7 @@ void __svgalib_driver32_hline(int x1, in
 void __svgalib_driver32p_hline(int x1, int y, int x2, int c)
 {
     int vp;
-    char *rvp;
+    uchar *rvp;
     int l;
     int chunksize, page;
     ASSIGNVPOFFSET32(x1, y, vp);
@@ -1158,7 +1148,7 @@ void __svgalib_driver32p_hline(int x1, i
 
 void __svgalib_driver32_fillbox(int x, int y, int w, int h, int c)
 {
-    char *vp;
+    uchar *vp;
     int i;
     ASSIGNVP32(x, y, vp);
     for (i = 0; i < h; i++) {
