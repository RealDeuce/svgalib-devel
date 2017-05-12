#include <sys/types.h>
#include <sys/param.h>
#include <sys/bus.h>
#include <machine/bus.h>
#include <sys/queue.h>
#include <sys/rman.h>
#include <machine/resource.h>
#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>
#include <sys/systm.h>

#include "svgalib_helper.h"
#include "interrupt.h"
#include "vendor_ids.h"

int
vga_test_vsync(struct sh_pci_device *dev) {
	return bus_space_read_1(dev->tag, dev->handle, 0x3c2) & 0x80;
}

void
vga_ack_vsync(struct sh_pci_device *dev)
{
	int pb;
    
	/* clear pending */
	bus_space_write_1(dev->tag, dev->handle, 0x3d4, 0x11);
	pb = bus_space_read_1(dev->tag, dev->handle, 0x3d5);
	bus_space_write_1(dev->tag, dev->handle, 0x3d4, 0x11);
	bus_space_write_1(dev->tag, dev->handle, 0x3d5, (pb & 0xef) | 0x20);

	/* disable interrupts */
	bus_space_write_1(dev->tag, dev->handle, 0x3d4, 0x11);
	pb = bus_space_read_1(dev->tag, dev->handle, 0x3d5);
	bus_space_write_1(dev->tag, dev->handle, 0x3d4, 0x11);
	bus_space_write_1(dev->tag, dev->handle, 0x3d5, pb | 0x20);
}

void
vga_enable_vsync(struct sh_pci_device *dev)
{
	int pb;

	/* enable interrupt, clear pending */
	bus_space_write_1(dev->tag, dev->handle, 0x3d4, 0x11);
	pb = bus_space_read_1(dev->tag, dev->handle, 0x3d5);
	bus_space_write_1(dev->tag, dev->handle, 0x3d4, 0x11);
	bus_space_write_1(dev->tag, dev->handle, 0x3d5, pb & 0xcf);

	/* Allow interrupts */
	bus_space_write_1(dev->tag, dev->handle, 0x3d4, 0x11);
	pb = bus_space_read_1(dev->tag, dev->handle, 0x3d5);
	bus_space_write_1(dev->tag, dev->handle, 0x3d4, 0x11);
	bus_space_write_1(dev->tag, dev->handle, 0x3d5, pb | 0x10);
}

int
s3_test_vsync(struct sh_pci_device *dev)
{
	return bus_space_read_4(dev->tag, dev->handle, 0x8504) & 1;
}

void
s3_enable_vsync(struct sh_pci_device *dev)
{
	bus_space_write_4(dev->tag, dev->handle, 0x8504, 0x101);
}

void
s3_ack_vsync(struct sh_pci_device *dev)
{
	bus_space_write_4(dev->tag, dev->handle, 0x8504, 0x001);
}

static uint32_t saved_pmc=0xffffffff;

int
nv3_test_vsync(struct sh_pci_device *dev)
{
	return bus_space_read_4(dev->tag, dev->handle, 0x400100) & 0x100;
}

void
nv3_ack_vsync(struct sh_pci_device *dev)
{
	bus_space_write_4(dev->tag, dev->handle, 0x000100, 0xffffffff);
	bus_space_write_4(dev->tag, dev->handle, 0x400100, 0x100);
	bus_space_write_4(dev->tag, dev->handle, 0x000140, 0);
	bus_space_write_4(dev->tag, dev->handle, 0x400140, 0);
	if (saved_pmc != 0xffffffff)
		bus_space_write_4(dev->tag, dev->handle, 0x000200, saved_pmc);
}

void
nv3_enable_vsync(struct sh_pci_device *dev)
{
	saved_pmc = bus_space_read_4(dev->tag, dev->handle, 0x000200);
	bus_space_write_4(dev->tag, dev->handle, 0x000200, saved_pmc | 0x1000);
	bus_space_write_4(dev->tag, dev->handle, 0x000140, 0x1);
	bus_space_write_4(dev->tag, dev->handle, 0x400140, 0x100);
	bus_space_write_4(dev->tag, dev->handle, 0x000100, 0xffffffff);
	bus_space_write_4(dev->tag, dev->handle, 0x400100, 0xffffffff);
}

int
nv4_test_vsync(struct sh_pci_device *dev)
{
	return bus_space_read_4(dev->tag, dev->handle, 0x600100) & 0x1;
}

void
nv4_ack_vsync(struct sh_pci_device *dev)
{
	bus_space_write_4(dev->tag, dev->handle, 0x000100, 0xffffffff);
	bus_space_write_4(dev->tag, dev->handle, 0x600100, 0x1);
	bus_space_write_4(dev->tag, dev->handle, 0x000140, 0);
	bus_space_write_4(dev->tag, dev->handle, 0x600140, 0);
	bus_space_write_4(dev->tag, dev->handle, 0x000200, saved_pmc);
}

void
nv4_enable_vsync(struct sh_pci_device *dev)
{
	saved_pmc = bus_space_read_4(dev->tag, dev->handle, 0x000200);
	bus_space_write_4(dev->tag, dev->handle, 0x000200, saved_pmc | (1<<24));
	bus_space_write_4(dev->tag, dev->handle, 0x000140, 0x1);
	bus_space_write_4(dev->tag, dev->handle, 0x600140, 0x1);
	bus_space_write_4(dev->tag, dev->handle, 0x000100, 0xffffffff);
	bus_space_write_4(dev->tag, dev->handle, 0x600100, 0xffffffff);
}

int
r128_test_vsync(struct sh_pci_device *dev)
{
	return bus_space_read_4(dev->tag, dev->handle, 0x44) & 0x1;
}

void
r128_ack_vsync(struct sh_pci_device *dev)
{
	bus_space_write_4(dev->tag, dev->handle, 0x44, 1);
	bus_space_write_4(dev->tag, dev->handle, 0x40, 
	    bus_space_read_4(dev->tag, dev->handle, 0x40) & ~0x1);
}

void
r128_enable_vsync(struct sh_pci_device *dev)
{
	bus_space_write_4(dev->tag, dev->handle, 0x44, 1);
	bus_space_write_4(dev->tag, dev->handle, 0x40, 
	    bus_space_read_4(dev->tag, dev->handle, 0x40) | 1);
}

int
rage_test_vsync(struct sh_pci_device *dev)
{
	return bus_space_read_4(dev->tag, dev->handle, 0x18) & 0x4;
}

void
rage_ack_vsync(struct sh_pci_device *dev)
{
	bus_space_write_4(dev->tag, dev->handle, 0x18, 
	    (bus_space_read_4(dev->tag, dev->handle, 0x18) & 0xfffffff8) | 12);
}

void
rage_enable_vsync(struct sh_pci_device *dev)
{
	bus_space_write_4(dev->tag, dev->handle, 0x18, 
	    (bus_space_read_4(dev->tag, dev->handle, 0x18) & 0xfffffff8) | 14);
}

int
rendition_test_vsync(struct sh_pci_device *dev)
{
	return bus_space_read_4(dev->tag, dev->handle, 0x44) & 0x1;
}

void
rendition_ack_vsync(struct sh_pci_device *dev)
{
	bus_space_write_4(dev->tag, dev->handle, 0x44, 1);
	bus_space_write_4(dev->tag, dev->handle, 0x46, 0);
}

void
rendition_enable_vsync(struct sh_pci_device *dev)
{
	bus_space_write_4(dev->tag, dev->handle, 0x44, 1);
	bus_space_write_4(dev->tag, dev->handle, 0x46, 1);
}

static bool
do_bus_map_mem(struct sh_pci_device *dev, int rid, uint32_t offset)
{
	dev->rid = rid;
	dev->resource_type = SYS_RES_MEMORY;
	dev->resource = bus_alloc_resource_any(dev->dev, dev->resource_type, &dev->rid, RF_ACTIVE|RF_SHAREABLE);
	if (dev->resource == NULL) {
		printf("Failed to allocate memory on rid %d\n", rid);
		return false;
	}
	// TODO: Check length...
	dev->membase = rman_get_virtual(dev->resource);
	if (dev->membase == NULL) {
		bus_release_resource(dev->dev, SYS_RES_MEMORY, dev->rid, dev->resource);
		printf("rman_get_virtual() returned NULL\n");
		return false;
	}
	dev->membase += offset;
	dev->tag = rman_get_bustag(dev->resource);
	dev->handle = rman_get_bushandle(dev->resource);
	return true;
}

static bool
do_bus_map_ports(struct sh_pci_device *dev, int rid, uint32_t offset)
{
	if (rid == 0) {
		// TODO: Don't map the whole space... everything gets a -0x3c0 offset...
#if defined(__amd64__) || defined(__i386__)
		dev->tag = X86_BUS_SPACE_IO;
#else
# error "Architecture not yet supported!"
#endif
		dev->resource_type = -1;
		if (bus_space_map(dev->tag, 0, 0x3dc, 0, &dev->handle)) {
			printf("Failed to map legacy I/O ports\n");
			return false;
		}
	}
	else {
		dev->rid = rid;
		dev->resource_type = SYS_RES_IOPORT;
		dev->resource = bus_alloc_resource_any(dev->dev, dev->resource_type, &dev->rid, RF_ACTIVE|RF_SHAREABLE);
		if (dev->resource == NULL) {
			printf("Failed to allocate memory on rid %d\n", rid);
			return false;
		}
		// TODO: Can't validate?
		dev->iobase = rman_get_start(dev->resource);
		dev->iobase += offset;
		dev->tag = rman_get_bustag(dev->resource);
		dev->handle = rman_get_bushandle(dev->resource);
	}
	return true;
}

int
vga_init_vsync(struct sh_pci_device *dev)
{
	int i;
	int id;

	switch(dev->vendor) {
	case PCI_VENDOR_ID_MATROX:
		i=0;
		if(dev->len[0] >= 1048576)
			i=1;
		if (!do_bus_map_mem(dev, PCIR_BAR(i), 0x1c00))
			return -ENODEV;
		dev->test_vsync = vga_test_vsync;
		dev->ack_vsync = vga_ack_vsync;
		dev->enable_vsync = vga_enable_vsync;            
		dev->disable_vsync = vga_ack_vsync;
		break;
	case PCI_VENDOR_ID_SI: /* SiS */
		if (!do_bus_map_ports(dev, PCIR_BAR(2), -0x380))
			return -ENODEV;
		dev->test_vsync = vga_test_vsync;
		dev->ack_vsync = vga_ack_vsync;
		dev->enable_vsync = vga_enable_vsync;
		dev->disable_vsync = vga_ack_vsync;
		break;
	case PCI_VENDOR_ID_NVIDIA_SGS:
		if (!do_bus_map_mem(dev, PCIR_BAR(0), 0))
			return -ENODEV;
		if(dev->id < 0x20) {
			dev->test_vsync = nv3_test_vsync;
			dev->ack_vsync = nv3_ack_vsync;
			dev->enable_vsync = nv3_enable_vsync;
			dev->disable_vsync = nv3_ack_vsync;
		} else {
			dev->test_vsync = nv4_test_vsync;
			dev->ack_vsync = nv4_ack_vsync;
			dev->enable_vsync = nv4_enable_vsync;
			dev->disable_vsync = nv4_ack_vsync;
		}
		break;
	case PCI_VENDOR_ID_NVIDIA:
		if (!do_bus_map_mem(dev, PCIR_BAR(0), 0))
			return -ENODEV;
		dev->test_vsync = nv4_test_vsync;
		dev->ack_vsync = nv4_ack_vsync;
		dev->enable_vsync = nv4_enable_vsync;
		dev->disable_vsync = nv4_ack_vsync;
		break;
	case PCI_VENDOR_ID_ATI:
		id=dev->id;
    
		if( (id==0x4c45) ||
		    (id==0x4c46) ||
		    (id==0x4c57) ||
		    (id==0x4c59) ||
		    (id==0x4c5a) ||
		    (id==0x4d46) ||
		    (id==0x4d4c) ||
		    (id==0x4242) ||
		    ((id>>8)==0x50) ||
		    ((id>>8)==0x51) || 
		    ((id>>8)==0x52) ||
		    ((id>>8)==0x53) ||
		    ((id>>8)==0x54)) {
			if (!do_bus_map_mem(dev, PCIR_BAR(2), 0))
				return -ENODEV;
			dev->test_vsync = r128_test_vsync;
			dev->ack_vsync = r128_ack_vsync;
			dev->enable_vsync = r128_enable_vsync;
			dev->disable_vsync = r128_ack_vsync;
		} else {
			if (!do_bus_map_ports(dev, PCIR_BAR(1), 0))
				return -ENODEV;
			dev->test_vsync = rage_test_vsync;
			dev->ack_vsync = rage_ack_vsync;
			dev->enable_vsync = rage_enable_vsync;
			dev->disable_vsync = rage_ack_vsync;
		}
		break;
	case PCI_VENDOR_ID_RENDITION:
		if (!do_bus_map_ports(dev, PCIR_BAR(1), 0))
			return -ENODEV;
		dev->test_vsync = rendition_test_vsync;
		dev->ack_vsync = rendition_ack_vsync;
		dev->enable_vsync = rendition_enable_vsync;
		dev->disable_vsync = rendition_ack_vsync;
		break;
	case PCI_VENDOR_ID_S3:
		if (!do_bus_map_mem(dev, PCIR_BAR(0), 0x1000000))
			return -ENODEV;
		dev->test_vsync = s3_test_vsync;
		dev->ack_vsync = s3_ack_vsync;
		dev->enable_vsync = s3_enable_vsync;
		dev->disable_vsync = s3_ack_vsync;
		break;
	default:
		if (!do_bus_map_ports(dev, 0, 0))
			return -ENODEV;
		dev->test_vsync = vga_test_vsync;
		dev->ack_vsync = vga_ack_vsync;
		dev->enable_vsync = vga_enable_vsync;
		dev->disable_vsync = vga_ack_vsync;
	}
	if (PCI_INTERRUPT_VALID(dev->irq))
		dev->irqresource = bus_alloc_resource_any(dev->dev, SYS_RES_IRQ, &dev->irqid, RF_ACTIVE | RF_SHAREABLE);

	return 0;
}
