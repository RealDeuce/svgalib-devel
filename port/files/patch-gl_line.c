--- gl/line.c.orig	2005-10-22 10:17:29 UTC
+++ gl/line.c
@@ -41,8 +41,8 @@ static inline int gl_regioncode (int x, 
 {							\
 	__asm__("btl $31,%1\n\t"			\
 		"adcl $0,%0"				\
-		: "=r" ((int) result)			\
-		: "rm" ((int) (y)), "0" ((int) result)	\
+		: "=r" (result)				\
+		: "rm" ((y)), "0" (result)		\
 		);					\
 }
 
