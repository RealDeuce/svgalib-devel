--- src/drivers/g400.c.orig	2005-10-22 10:13:32 UTC
+++ src/drivers/g400.c
@@ -13,6 +13,7 @@ TODO: SDRAM, reference frequency checkin
 #include <stdio.h>		
 #include <string.h>
 #include <unistd.h>
+#include <math.h>
 #include "vga.h"
 #include "libvga.h"
 #include "driver.h"
@@ -292,8 +293,8 @@ MGACalcClock (int f_out,
 			/*
 			 * Pick the closest frequency.
 			 */
-			if ( abs(calc_f - f_vco) < m_err ) {
-				m_err = abs(calc_f - f_vco);
+			if ( fabs(calc_f - f_vco) < m_err ) {
+				m_err = fabs(calc_f - f_vco);
 				*best_m = m;
 				*best_n = n;
 			}
