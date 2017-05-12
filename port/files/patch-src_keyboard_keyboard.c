--- src/keyboard/keyboard.c.orig	2005-10-26 21:18:41 UTC
+++ src/keyboard/keyboard.c
@@ -27,7 +27,7 @@
 #include <sys/ioctl.h>
 #include <fcntl.h>
 #include <termios.h>
-#include <sys/kd.h>
+#include <sys/kbio.h>
 #if 0
 /* linux/keyboard.h defines NR_KEYS and some scancode-like constants, so it */
 /* should also be useful for svgalib programs using the keyboard. It misses */
@@ -36,7 +36,7 @@
 #else
 #define NR_KEYS 128
 #endif
-#include <sys/vt.h>
+#include <sys/consio.h>
 /* Needed to check uid of keymap files */
 #include <sys/stat.h>
 #include <unistd.h>
@@ -349,7 +349,7 @@ int keyboard_init_return_fd(void) {
     
         tcsetattr(__svgalib_kbd_fd, TCSAFLUSH, &newkbdtermios);
     
-        ioctl(__svgalib_kbd_fd, KDSKBMODE, K_MEDIUMRAW);
+        ioctl(__svgalib_kbd_fd, KDSKBMODE, K_CODE);
     }
 
     keyboard_clearstate();
@@ -594,7 +594,7 @@ static int keyboard_getevents(int wait)
 	    /* VT switch. */
 	    /* *** what about F11 & F12? */
 	    int j, vt = 0;
-	    struct vt_stat vts;
+	    int v_active;
 	    for (j = 0; j < 12; j++)
 		if (functionkey_state & (1 << j)) {
 		    vt = j + 1;
@@ -603,9 +603,9 @@ static int keyboard_getevents(int wait)
 		}
 
 	    /* Do not switch vt's if need not to */
-	    ioctl(__svgalib_tty_fd, VT_GETSTATE, &vts);
+	    ioctl(__svgalib_tty_fd, VT_GETACTIVE, &v_active);
 
-	    if(vt != vts.v_active) { 
+	    if(vt != v_active) { 
 	        /* if switching vt's, need to clear keystates */
 	        keyboard_clearstate();
 	        /*
