--- gl/vgagl.h.orig	2001-02-02 22:26:09 UTC
+++ gl/vgagl.h
@@ -55,7 +55,7 @@ extern "C"
 	int colors;		/* number of colors */
 	int bitsperpixel;	/* bits per pixel (8, 15, 16 or 24) */
 	int bytewidth;		/* length of a scanline in bytes */
-	char *vbuf;		/* address of framebuffer */
+	unsigned char *vbuf;	/* address of framebuffer */
 	int clip;		/* clipping enabled? */
 	int clipx1;		/* top-left coordinate of clip window */
 	int clipy1;
