--- src/vgapal.c.orig	2004-09-10 11:02:16 UTC
+++ src/vgapal.c
@@ -32,7 +32,7 @@ static int set_lut(int index, int red, i
     if (__svgalib_novga) return 1;
     
     /* prevents lockups */
-    if ((__svgalib_chipset == MACH64)) {
+    if (__svgalib_chipset == MACH64) {
         port_out_r(0x02ec+0x5c00,index);
         port_out_r(0x02ec+0x5c01,red);
         port_out_r(0x02ec+0x5c01,green);
@@ -51,7 +51,7 @@ static int get_lut(int index, int *red, 
     if (__svgalib_novga) return 0;
 
     /* prevents lockups on mach64 */
-    if ((__svgalib_chipset == MACH64)) {
+    if (__svgalib_chipset == MACH64) {
         port_out_r(0x02ec+0x5c00,index);
         *red=port_in(0x02ec+0x5c01);
         *green=port_in(0x02ec+0x5c01);
