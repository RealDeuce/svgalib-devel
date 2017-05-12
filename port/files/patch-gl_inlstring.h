--- gl/inlstring.h.orig	2005-08-03 13:07:31 UTC
+++ gl/inlstring.h
@@ -90,7 +90,7 @@ static inline void *
 	    "rep\n\t"
 	    "stosb"
   : :	    "a"(c), "D"(s), "c"(count)
-  :	    "cx", "di");
+  :	    /*"cx", "di"*/);
     return s;
 }
 
@@ -295,7 +295,7 @@ static inline void *
 			 "1:call __memcpyasm_regargs\n\t"
 			 "2:":
 			 :"S"(dest), "d"(src), "c"(n)
-			 :"ax", "0", "1", "2");
+			 :"ax", "0"/*, "1", "2"*/);
     return dest;
 }
 
@@ -311,7 +311,7 @@ static inline void *
 			 "1:call __memcpyasm_regargs_aligned\n\t"
 			 "2:\n\t":
 			 :"S"(dest), "d"(src), "c"(n)
-			 :"ax", "0", "1", "2");
+			 :"ax", "0"/*, "1", "2"*/);
     return dest;
 }
 
@@ -328,7 +328,7 @@ static inline void *
 			 "2:\n\t"
 			 :
 			 :"S"((long) dest), "d"((long) src), "c"((long) n)
-			 :"ax", "0", "1", "2");
+			 :"ax", "0"/*, "1", "2"*/);
     return dest;
 }
 
