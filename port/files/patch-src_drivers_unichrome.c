--- src/drivers/unichrome.c.orig	2005-08-03 13:07:32 UTC
+++ src/drivers/unichrome.c
@@ -30,7 +30,7 @@ typedef struct {
     unsigned char crt[0xc0];
 } UNIRegRec, *UNIRegPtr;
 
-enum { CLE266=1, CLE266CX, KM400, KM800, PM800} chiptype;
+enum { CLE266=1, CLE266CX, KM400, KM800, PM800, CN700} chiptype;
 
 //static void unichrome_setpage(int page) {}
 
@@ -444,7 +444,8 @@ static int unichrome_init(int force, int
             ((buf[0]>>16)==0x3108)||
             ((buf[0]>>16)==0x3118)||
             ((buf[0]>>16)==0x3122)||
-            ((buf[0]>>16)==0x7205));
+            ((buf[0]>>16)==0x7205)||
+            ((buf[0]>>16)==0x3344));
 	if(!found) return 1;
 
     switch(buf[0]>>16) {
@@ -461,6 +462,9 @@ static int unichrome_init(int force, int
 		case 0x7205:
 			chiptype=KM400;
 			break;
+		case 0x3344:
+			chiptype=CN700;
+			break;
 	}		
     
     if (found){
