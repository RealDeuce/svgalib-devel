--- src/mouse/ms.c.orig	2005-08-03 13:07:32 UTC
+++ src/mouse/ms.c
@@ -120,7 +120,6 @@ static void ms_setspeed(const int old, c
     tty.c_iflag = IGNBRK | IGNPAR;
     tty.c_oflag = 0;
     tty.c_lflag = 0;
-    tty.c_line  = 0;
     tty.c_cc[VTIME] = 0;
     tty.c_cc[VMIN]  = 1;
 
@@ -526,7 +525,6 @@ static int ms_init(void)
 	tty.c_iflag = IGNBRK | IGNPAR;
 	tty.c_oflag = 0;
 	tty.c_lflag = 0;
-	tty.c_line = 0;
 	tty.c_cc[VTIME] = 0;
 	tty.c_cc[VMIN] = 1;
 	tty.c_cflag = cflag[m_type] | B1200;
@@ -711,7 +709,7 @@ static int get_ms_event(int wait) {
 	event_handled++;
 	i++;
     }
-    if ((m_type == MOUSE_SPACEBALL)) {
+    if (m_type == MOUSE_SPACEBALL) {
       j=i;
       while ((nu_bytes - j > 0) && (buf[j]!=13))
         j++;
@@ -741,7 +739,7 @@ static int get_ms_event(int wait) {
 	    fcntl(__svgalib_mouse_fd, F_SETFL, O_RDONLY);
 	    m_fdmode = 1;
 	    read(__svgalib_mouse_fd, &buf[nu_bytes], 1);
-            if ((m_type == MOUSE_SPACEBALL)) {
+            if (m_type == MOUSE_SPACEBALL) {
               nu_packets=(buf[nu_bytes]==13);
             } else {
               nu_packets=1;
