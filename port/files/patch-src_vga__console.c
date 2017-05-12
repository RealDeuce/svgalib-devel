--- src/vga_console.c.orig	2005-07-02 15:41:37 UTC
+++ src/vga_console.c
@@ -8,10 +8,10 @@
 #include <unistd.h>
 #include <stdarg.h>
 #include <sys/mman.h>
-#include <sys/kd.h>
+#include <sys/kbio.h>
 #include <sys/ioctl.h>
 #include <sys/stat.h>
-#include <sys/vt.h>
+#include <sys/consio.h>
 #include <sys/wait.h>
 #include <errno.h>
 #include <ctype.h>
@@ -39,7 +39,7 @@ void __svgalib_set_graphtermio(void)
     /* Leave keyboard alone when rawkeyboard is enabled! */
     if (__svgalib_kbd_fd < 0) {
 	/* set graphics mode termio parameters */
-	ioctl(0, TCSETSW, &__svgalib_graph_termio);
+	ioctl(0, TIOCSETAW, &__svgalib_graph_termio);
     }
 }
 
@@ -51,7 +51,7 @@ void __svgalib_set_texttermio(void)
     /* Leave keyboard alone when rawkeyboard is enabled! */
     if (__svgalib_kbd_fd < 0) {
 	/* restore text mode termio parameters */
-	ioctl(0, TCSETSW, &__svgalib_text_termio);
+	ioctl(0, TIOCSETAW, &__svgalib_text_termio);
     }
 }
 
@@ -62,9 +62,9 @@ void __svgalib_disable_interrupt(void)
 
     /* Well, one could argue that sigint is not enabled at all when in __svgalib_nosigint
        but sometimes they *still* are enabled b4 graph_termio is set.. */
-    ioctl(0, TCGETS, &cur_termio);
+    ioctl(0, TIOCGETA, &cur_termio);
     cur_termio.c_lflag &= ~ISIG;
-    ioctl(0, TCSETSW, &cur_termio);
+    ioctl(0, TIOCSETAW, &cur_termio);
 }
 
 
@@ -74,9 +74,9 @@ void __svgalib_enable_interrupt(void)
 
     if (__svgalib_nosigint) /* do not reenable, they are often reenabled by text_termio */
 	return; 
-    ioctl(0, TCGETS, &cur_termio);
+    ioctl(0, TIOCGETA, &cur_termio);
     cur_termio.c_lflag |= ISIG;
-    ioctl(0, TCSETSW, &cur_termio);
+    ioctl(0, TIOCSETAW, &cur_termio);
 }
 
 /* The following is rather messy and inelegant. The only solution I can */
@@ -105,7 +105,7 @@ static int check_owner(int vc)
     if (!getuid())
         return 1;               /* root can do it always */
 #endif
-    sprintf(fname, "/dev/tty%d", vc);
+    sprintf(fname, "/dev/ttyv%d", vc-1);
     if ((stat(fname, &sbuf) >= 0) && (getuid() == sbuf.st_uid)) {
         return 1;
     }
@@ -116,7 +116,7 @@ static int check_owner(int vc)
 void __svgalib_open_devconsole(void)
 {
     struct vt_mode vtm;
-    struct vt_stat vts;
+    int v_active;
     struct stat sbuf;
     char fname[30];
 
@@ -168,15 +168,15 @@ void __svgalib_open_devconsole(void)
         goto error;
     if (__svgalib_vc <= 0)
         goto error;
-    sprintf(fname, "/dev/tty%d", __svgalib_vc);
+    sprintf(fname, "/dev/ttyv%d", __svgalib_vc-1);
     close(__svgalib_tty_fd);
     /* change our control terminal: */
     setpgid(0,getppid());
     setsid();
     /* We must use RDWR to allow for output... */
     if (((__svgalib_tty_fd = open(fname, O_RDWR)) >= 0) &&
-        (ioctl(__svgalib_tty_fd, VT_GETSTATE, &vts) >= 0)) {
-        if (!check_owner(vts.v_active)) {
+        (ioctl(__svgalib_tty_fd, VT_GETACTIVE, &v_active) >= 0)) {
+        if (!check_owner(v_active)) {
             
             goto error;
         }
@@ -195,8 +195,8 @@ void __svgalib_open_devconsole(void)
         /* clear screen and switch to it */
         fwrite("\e[H\e[J", 6, 1, stderr);
         fflush(stderr);
-        if (__svgalib_vc != vts.v_active) {
-            __svgalib_startup_vc = vts.v_active;
+        if (__svgalib_vc != v_active) {
+            __svgalib_startup_vc = v_active;
 	    ioctl(__svgalib_tty_fd, VT_ACTIVATE, __svgalib_vc);
             __svgalib_waitvtactive();
 	}
