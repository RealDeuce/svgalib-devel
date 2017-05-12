--- src/vga.c.orig	2005-10-25 11:13:56 UTC
+++ src/vga.c
@@ -22,13 +22,13 @@
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
 #include <sys/syslog.h>
-#include <sys/io.h>
+#include <machine/cpufunc.h>
 
 #ifdef INCLUDE_VESA_DRIVER
 #include <sys/vm86.h>
@@ -767,6 +767,22 @@ static char *driver_names[] =
 /* Debug config file parsing.. */
 /*#define DEBUG_CONF */
 
+int permfd = -1;
+static int ioperm(unsigned long from, unsigned long num, int on)
+{
+	if (permfd == -1)
+		permfd = open("/dev/io", O_RDONLY);
+	if (permfd != -1)
+		return 0;
+	else
+		return 1;
+}
+
+static int iopl(int level)
+{
+	return ioperm(0, 0, 0);
+}
+
 /* open /dev/svga */
 static void open_mem(void)
 {
@@ -1083,7 +1099,7 @@ static char sig2catch[] =
  SIGTRAP, SIGIOT, SIGBUS, SIGFPE,
  SIGSEGV, SIGPIPE, SIGALRM, SIGTERM,
  SIGXCPU, SIGXFSZ, SIGVTALRM,
-/* SIGPROF ,*/ SIGPWR};
+/* SIGPROF ,*/ SIGUSR1};
 static struct sigaction old_signal_handler[sizeof(sig2catch)];
 
 struct vt_mode __svgalib_oldvtmode;
@@ -1625,12 +1641,12 @@ static void initialize(void)
 
 #ifndef SET_TERMIO
     /* save text mode termio parameters */
-    ioctl(0, TCGETS, &__svgalib_text_termio);
+    ioctl(0, TIOCGETA, &__svgalib_text_termio);
 
     __svgalib_graph_termio = __svgalib_text_termio;
 
     /* change termio parameters to allow our own I/O processing */
-    __svgalib_graph_termio.c_iflag &= ~(BRKINT | PARMRK | INPCK | IUCLC | IXON | IXOFF);
+    __svgalib_graph_termio.c_iflag &= ~(BRKINT | PARMRK | INPCK | IXON | IXOFF);
     __svgalib_graph_termio.c_iflag |= (IGNBRK | IGNPAR);
 
     __svgalib_graph_termio.c_oflag &= ~(ONOCR);
@@ -1824,7 +1840,8 @@ void vga_safety_fork(void (*shutdown_rou
     if (childpid) {
 	ioctl(__svgalib_tty_fd, (int) TIOCNOTTY, (char *)0);
 	for (;;) {
-	    while (waitpid(childpid, &child_status, WUNTRACED) != childpid);
+	    while (waitpid(childpid, &child_status, WUNTRACED) != childpid)
+		;
 
 	    if (shutdown_routine)
 		shutdown_routine();
@@ -1928,8 +1945,10 @@ int vga_setmode(int mode)
 		 * a C&T wait for retrace start
 		 */
 		if (__svgalib_getchipset() == CHIPS) {
-			while ((__svgalib_inis1() & 0x08) == 0x08 );/* wait VSync off */
-	 		while ((__svgalib_inis1() & 0x08) == 0 );   /* wait VSync on  */
+			while ((__svgalib_inis1() & 0x08) == 0x08 )
+				;/* wait VSync off */
+	 		while ((__svgalib_inis1() & 0x08) == 0 )
+				;   /* wait VSync on  */
  			__svgalib_outseq(0x07,0x00);  /* reset hsync - just in case...  */
 		}
 	}
@@ -2774,7 +2793,8 @@ int vga_getch(void)
     
     fd = __svgalib_novccontrol ? fileno(stdin) : __svgalib_tty_fd;
 
-    while ((read(fd, &c, 1) < 0) && (errno == EINTR));
+    while ((read(fd, &c, 1) < 0) && (errno == EINTR))
+	;
 
     return c;
 }
@@ -4206,7 +4226,7 @@ int vga_init(void)
 #ifdef SET_TERMIO
     if(!__svgalib_novccontrol) {
             /* save text mode termio parameters */
-            ioctl(0, TCGETS, &__svgalib_text_termio);
+            ioctl(0, TIOCGETA, &__svgalib_text_termio);
     
             __svgalib_graph_termio = __svgalib_text_termio;
     
