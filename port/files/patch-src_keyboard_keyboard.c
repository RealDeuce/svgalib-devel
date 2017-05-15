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
@@ -743,6 +743,76 @@ void keyboard_translatekeys(int mode)
 
 static int kbd_mapkey(int inscancode)
 {
+	/* Map to Linux codes... */
+	switch(inscancode) {
+		case 0x5c:	/* Print Screen */
+			inscancode = SCANCODE_PRINTSCREEN;
+			break;
+		case 0x68:	/* Pause */
+			inscancode = SCANCODE_BREAK;
+			break;
+		case 0x66:	/* Insert */
+			inscancode = SCANCODE_INSERT;
+			break;
+		case 0x5e:	/* Home */
+			inscancode = SCANCODE_HOME;
+			break;
+		case 0x60:	/* Page Up */
+			inscancode = SCANCODE_PAGEUP;
+			break;
+		case 0x67:	/* Delete */
+			inscancode = SCANCODE_REMOVE;
+			break;
+		case 0x63:	/* End */
+			inscancode = SCANCODE_END;
+			break;
+		case 0x65:	/* Page Down */
+			inscancode = SCANCODE_PAGEDOWN;
+			break;
+		case 0x5f:	/* Up Arrow */
+			inscancode = SCANCODE_CURSORBLOCKUP;
+			break;
+		case 0x64:	/* Down Arrow */
+			inscancode = SCANCODE_CURSORBLOCKDOWN;
+			break;
+		case 0x61:	/* Left Arrow */
+			inscancode = SCANCODE_CURSORBLOCKLEFT;
+			break;
+		case 0x62:	/* Right Arrow */
+			inscancode = SCANCODE_CURSORBLOCKRIGHT;
+			break;
+		case 0x5b:	/* Keypad Divide */
+			inscancode = SCANCODE_KEYPADDIVIDE;
+			break;
+		case 0x59:	/* Keypad Enter */
+			inscancode = SCANCODE_KEYPADENTER;
+			break;
+		case 0x5d:	/* Right Alt (Alt Graph) */
+			inscancode = SCANCODE_RIGHTALT;
+			break;
+		case 0x69:	/* Left Meta */
+			inscancode = SCANCODE_LEFTWIN;
+			break;
+		case 0x6a:	/* Right Meta */
+			inscancode = SCANCODE_RIGHTWIN;
+			break;
+		case 0x6b:	/* Compose */
+			inscancode = SCANCODE_WINMENU;
+			break;
+		/* Yes, I have a Sun keyboard, Just mapping "unused" codes */
+		case 0x6d:	/* Stop */
+			inscancode = 89;
+			break;
+		case 0x6e:	/* Again */
+			inscancode = 90;
+			break;
+		case 0x6f:	/* Copy */
+			inscancode = 91;
+			break;
+		case 0x73:	/* Open */
+			inscancode = 92;
+			break;
+	}
     if (usekeymap)
         return keymap[inscancode];
     else
