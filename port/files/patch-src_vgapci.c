--- src/vgapci.c.orig	2005-07-30 18:21:47 UTC
+++ src/vgapci.c
@@ -3,6 +3,8 @@
 #include <unistd.h>
 #include <fcntl.h>
 #include <sys/ioctl.h>
+#include <sys/pciio.h>
+#include <dev/pci/pcireg.h>
 #include "endianess.h"
 #include "libvga.h"
 #include "svgalib_helper.h"
@@ -70,20 +72,25 @@ unsigned int __svgalib_pci_read_config_d
 {
     if(__svgalib_nohelper) {
 		int f;
-		unsigned int n, d;
-		int bus, device, fn;
-		char filename[256];
-		
-		bus=(pos&0xff00)>>8;
-		device=(pos&0xf8)>>3;
-		fn=pos&0x07;
-		sprintf(filename,"/proc/bus/pci/%02i/%02x.%i",bus,device,fn);
-		f=open(filename,O_RDONLY);
-		lseek(f, address, SEEK_SET);
-		read(f, &n, 4);
+		struct pci_io pi;
+
+		pi.pi_sel.pc_domain = 0;
+		pi.pi_sel.pc_bus = (pos&0xff00)>>8;
+		pi.pi_sel.pc_dev = (pos&0xf8)>>3;
+		pi.pi_sel.pc_func = 0;
+		pi.pi_reg = address;
+		pi.pi_width = 4;
+	
+		f = open("/dev/pci", O_RDWR);
+		if (f < 0)
+			return 0;
+		if (ioctl(f, PCIOCREAD, &pi) == -1) {
+			close(f);
+			return 0;
+		}
 		close(f);
-		d=LE32(n);
-		return d;
+		// TODO: Do we need to byte-swap using LE32() as in Linux code?
+		return pi.pi_data;
 	} else {
 		pcic_t p;
  		
@@ -99,22 +106,24 @@ unsigned int __svgalib_pci_read_config_d
 unsigned long __svgalib_pci_read_aperture_len(int pos, int address)
 {
     if(__svgalib_nohelper) {
-		FILE *f;
-		char buf[512];
+		int f;
+		struct pci_bar_io pb;
 
-		f=fopen("/proc/bus/pci/devices", "r");
-		while (fgets(buf, sizeof(buf)-1, f)) {
-			int cnt;
-			unsigned int dev, di;
-			unsigned long lens[6], dl;
-			cnt = sscanf(buf, "%x %x %x %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx",
-					&dev, &di, &di, 
-					&dl, &dl, &dl, &dl, &dl, &dl, &dl,
-					&lens[0], &lens[1], &lens[2], &lens[3], &lens[4], &lens[5],
-					&dl);
-			if(dev==pos)
-				return lens[address];				
+		pb.pbi_sel.pc_domain = 0;
+		pb.pbi_sel.pc_bus = (pos&0xff00)>>8;
+		pb.pbi_sel.pc_dev = (pos&0xf8)>>3;
+		pb.pbi_sel.pc_func = 0;
+		pb.pbi_reg = PCIR_BAR(address);
+
+		f = open("/dev/pci", O_RDWR);
+		if (f < 0)
+			return 0;
+		if (ioctl(f, PCIOCGETBAR, &pb) == -1) {
+			close(f);
+			return 0;
 		}
+		close(f);
+		return pb.pbi_length;
 	} else {
 		pcic_t p;
 		   
@@ -131,22 +140,27 @@ unsigned long __svgalib_pci_read_apertur
 
 void __svgalib_pci_write_config_dword(int pos, int address, unsigned int data)
 {
-    if(__svgalib_nohelper)
-    {
-	int f;
-	unsigned int d;
-	int bus, device, fn;
-	char filename[256];
+    if(__svgalib_nohelper) {
+		int f;
+		struct pci_io pi;
+
+		pi.pi_sel.pc_domain = 0;
+		pi.pi_sel.pc_bus = (pos&0xff00)>>8;
+		pi.pi_sel.pc_dev = (pos&0xf8)>>3;
+		pi.pi_sel.pc_func = 0;
+		pi.pi_reg = address;
+		pi.pi_width = 4;
+		// TODO: Do we need to byte-swap using LE32() as in Linux code?
+		pi.pi_data = data;
 	
-	d=LE32(data);
-	bus=(pos&0xff00)>>8;
-	device=(pos&0xf8)>>3;
-	fn=pos&0x07;
-	sprintf(filename,"/proc/bus/pci/%02i/%02x.%i",bus,device,fn);
-	f=open(filename,O_WRONLY);
-	lseek(f, address, SEEK_SET);
-	write(f, &d, 4);
-	close(f);
+		f = open("/dev/pci", O_RDWR);
+		if (f < 0)
+			return;
+		if (ioctl(f, PCIOCWRITE, &pi) == -1) {
+			close(f);
+			return;
+		}
+		close(f);
     }
     else
     {
