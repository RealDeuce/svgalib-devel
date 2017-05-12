--- threeDKit/wrapdemo.c.orig	2005-08-03 13:07:32 UTC
+++ threeDKit/wrapdemo.c
@@ -626,5 +626,5 @@ el_getchar ();
 
     EL_cleanup (ellip);
     vga_setmode (TEXT);
-    return;
+    return 0;
 }
