--- src/regextr.c.orig	2005-10-22 10:09:29 UTC
+++ src/regextr.c
@@ -115,7 +115,7 @@ static void store_regs(ModeList ** root,
 }
 
 
-static void __store_regs(ModeList ** root, int mnum, const char *r)
+static void __store_regs(ModeList ** root, int mnum, const unsigned char *r)
 {
     WordStr x, y, c;
 
