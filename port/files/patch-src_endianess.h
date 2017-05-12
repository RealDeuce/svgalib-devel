--- src/endianess.h.orig	2001-05-29 09:00:22 UTC
+++ src/endianess.h
@@ -1,14 +1,13 @@
-#include <endian.h>
-#include <byteswap.h>
+#include <sys/endian.h>
 
 #if __BYTE_ORDER == __BIG_ENDIAN
 
-#define LE32(x) bswap_32(x)
+#define LE32(x) bswap32(x)
 #define BE32(x) (x)
 
 #else /* little endian */
 
 #define LE32(x) (x)
-#define BE32(x) bswap_32(x)
+#define BE32(x) bswap32(x)
 
 #endif
