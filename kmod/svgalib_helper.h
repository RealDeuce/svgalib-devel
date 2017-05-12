#ifndef SVGALIB_HELPER_H
#define SVGALIB_HELPER_H

#ifdef _KERNEL

#define MAX_NR_DEVICES 15

struct sh_pci_device {
	unsigned short vendor;
	unsigned short id;
	unsigned char revision;
	device_t dev;			// PCI device
	struct cdev *cdev;		// Character device
	vm_paddr_t len[6];
	vm_paddr_t mem[6];
#if 0
   vm_paddr_t mask[6];
   int flags[6];
#endif
	uint8_t irq;
	int rid;
	int irqid;
	int resource_type;
	struct resource *resource;
	struct resource *irqresource;
	void *irqcookie;
	bus_space_tag_t tag;
	bus_space_handle_t handle;
	rman_res_t iobase;		// For Port I/O
	uint8_t * membase;		// For I/O memory
	int (*test_vsync)(struct sh_pci_device *);
	void (*ack_vsync)(struct sh_pci_device *);
	void (*enable_vsync)(struct sh_pci_device *);
	void (*disable_vsync)(struct sh_pci_device *);
	int opencount;
	int vsync;
};

extern int debug;

#endif

typedef struct {
  int port;
  int length;
  unsigned char* string;
} io_string_t;

typedef struct {
   int port;
   unsigned int val;
} io_t;

typedef struct {
   int pcipos;
   unsigned int address;
   unsigned long val;
} pcic_t;

typedef struct {
   void *win;
   void *lfb;
} windowing_t;

typedef struct dhahelper_vmi_s
{
    void *	virtaddr;
    unsigned long length;
    unsigned long *realaddr;
}dhahelper_vmi_t;

typedef struct dhahelper_mem_s
{
    void *	addr;
    unsigned long length;
}dhahelper_mem_t;

#define SVGAHELPER_MAGIC 0xB3

#define SVGAHELPER_OUTB		_IOW(SVGAHELPER_MAGIC,1,io_t)
#define SVGAHELPER_OUTW		_IOW(SVGAHELPER_MAGIC,2,io_t)
#define SVGAHELPER_OUTL		_IOW(SVGAHELPER_MAGIC,3,io_t)
#define SVGAHELPER_INB		_IOWR(SVGAHELPER_MAGIC,4,io_t)
#define SVGAHELPER_INW		_IOWR(SVGAHELPER_MAGIC,5,io_t)
#define SVGAHELPER_INL		_IOWR(SVGAHELPER_MAGIC,6,io_t)

#define SVGAHELPER_PCIOUTB	_IOW(SVGAHELPER_MAGIC,11,pcic_t)
#define SVGAHELPER_PCIOUTW	_IOW(SVGAHELPER_MAGIC,12,pcic_t)
#define SVGAHELPER_PCIOUTL	_IOW(SVGAHELPER_MAGIC,13,pcic_t)
#define SVGAHELPER_PCIINB	_IOWR(SVGAHELPER_MAGIC,14,pcic_t)
#define SVGAHELPER_PCIINW	_IOWR(SVGAHELPER_MAGIC,15,pcic_t)
#define SVGAHELPER_PCIINL	_IOWR(SVGAHELPER_MAGIC,16,pcic_t)
#define SVGAHELPER_PCIAPLEN	_IOWR(SVGAHELPER_MAGIC,17,pcic_t)

#define SVGAHELPER_DVMA		_IO(SVGAHELPER_MAGIC,7)
#define SVGAHELPER_WIND		_IOR(SVGAHELPER_MAGIC,8,windowing_t)

#define SVGAHELPER_IOPERM	_IO(SVGAHELPER_MAGIC,9)
#define SVGAHELPER_REPOUTB	_IOR(SVGAHELPER_MAGIC,10,io_t)

#define SVGAHELPER_I810GTT	_IOR(SVGAHELPER_MAGIC,128,unsigned int *)
#define SVGAHELPER_I810GTTE	_IOWR(SVGAHELPER_MAGIC,129,unsigned int *)

#define SVGAHELPER_WRITEB	_IOW(SVGAHELPER_MAGIC,21,io_t)
#define SVGAHELPER_WRITEW	_IOW(SVGAHELPER_MAGIC,22,io_t)
#define SVGAHELPER_WRITEL	_IOW(SVGAHELPER_MAGIC,23,io_t)
#define SVGAHELPER_READB	_IOWR(SVGAHELPER_MAGIC,24,io_t)
#define SVGAHELPER_READW	_IOWR(SVGAHELPER_MAGIC,25,io_t)
#define SVGAHELPER_READL	_IOWR(SVGAHELPER_MAGIC,26,io_t)

#define SVGAHELPER_WAITRETRACE	_IO(SVGAHELPER_MAGIC,31)
#define SVGAHELPER_SETDISPLAYSTART _IOW(SVGAHELPER_MAGIC,32, int)

#endif /* SVGALIB_HELPER_H */
