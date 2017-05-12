--- gl/text.c.orig	2005-07-02 15:41:38 UTC
+++ gl/text.c
@@ -17,7 +17,7 @@
 
 static int font_width = 8;
 static int font_height = 8;
-static char *font_address;
+static uchar *font_address;
 static int font_charactersize = 64;
 static int font_writemode = WRITEMODE_OVERWRITE;
 static int compressed_font_bg = 0;
@@ -88,7 +88,7 @@ void gl_writen(int x, int y, int n, char
 /* clipping in putbox */
     int i;
     if (font_writemode & FONT_COMPRESSED) {
-	writecompressed(x, y, n, s);
+	writecompressed(x, y, n, (unsigned char *)s);
 	return;
     }
     if (!(font_writemode & WRITEMODE_MASKED)) {
@@ -122,7 +122,7 @@ static int gl_nprintf(int sx, int sy, si
     int n;
 
     buf = alloca(bufs);
-    n = vsnprintf(buf, bufs, fmt, args);
+    n = vsnprintf((char *)buf, bufs, fmt, args);
     if (n < 0)
 	return n; /* buffer did not suffice, return and retry */
 
@@ -165,7 +165,7 @@ static int gl_nprintf(int sx, int sy, si
 		y %= font_height;
 	    break;
 	default:
-	    gl_writen(x, y, 1, buf);
+	    gl_writen(x, y, 1, (char *)buf);
 	    x += font_width;
 	  chk_wrap:
 	    if (x + font_width > WIDTH)
