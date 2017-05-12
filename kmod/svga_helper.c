#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/conf.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/bus.h>
#include <sys/pciio.h>
#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>
#include <vm/vm.h>
#include <vm/pmap.h>
#include <machine/bus.h>
#include <machine/resource.h>

#include "vgaversion.h"
#include "svgalib_helper.h"
#include "interrupt.h"

static int svga_open(struct cdev *dev, int oflags, int devtype, struct thread *td);
static int svga_close(struct cdev *dev, int fflag, int devtype, struct thread *td);
static int svga_ioctl(struct cdev *dev, u_long cmd, caddr_t data,
    int fflag, struct thread *td);
static int svga_mmap(struct cdev *dev, vm_ooffset_t offset, vm_paddr_t *paddr,
    int nprot, vm_memattr_t *memattr);

static struct cdevsw svga_devsw = {
	.d_version = D_VERSION,
	.d_open = svga_open,
	.d_close = svga_close,
	.d_ioctl = svga_ioctl,
	.d_mmap = svga_mmap,
};

static struct sh_pci_device *sh_pci_devs[MAX_NR_DEVICES] = {NULL};
static int num_devices;
static volatile int vsync = 0;

MALLOC_DECLARE(M_SVGALIB);
MALLOC_DEFINE(M_SVGALIB, "svgalib", "SVGAlib allocations");

static int
vsync_interrupt(void *devp)
{
	struct sh_pci_device *dev = (struct sh_pci_device *)devp;

	if (dev->ack_vsync) {
		dev->ack_vsync(dev);
		dev->vsync = 0;
		wakeup(devp);
		return FILTER_HANDLED;
	}
	else {
		vsync = 0;
		wakeup(devp);
		return FILTER_HANDLED;
	}
}

static int
svga_open(struct cdev *dev, int oflags, int devtype, struct thread *td)
{
	struct sh_pci_device *sdev = dev->si_drv1;

	if (!sdev)
		return -EINVAL;

	sdev->opencount++;
	if(sdev->opencount == 1) {
		if (sdev != sh_pci_devs[0]) {
			if (sdev->irqresource) {
				if (bus_setup_intr(sdev->dev, sdev->irqresource, INTR_TYPE_MISC | INTR_MPSAFE, vsync_interrupt, NULL, sdev, &sdev->irqcookie))
					sdev->irqcookie = NULL;
			}
	 		else {
				printf("Unable to allocate VSync IRQ for %s\n", dev->si_name);
			}
		}
	}
	return 0;
}

static int
svga_close(struct cdev *dev, int fflag, int devtype, struct thread *td)
{
	struct sh_pci_device *sdev = dev->si_drv1;

	if (!sdev)
		return -EINVAL;

	sdev->opencount--;
	if(sdev->opencount == 0) {
		if (sdev->irqresource && sdev->irqcookie) {
			bus_teardown_intr(sdev->dev, sdev->irqresource, sdev->irqcookie);
			sdev->irqcookie = NULL;
		}
	}
	return 0;
}

static int
check_io_range(int port, struct sh_pci_device *dev)
{
    return 1;
}

static struct sh_pci_device *
get_dev(int pcipos, struct sh_pci_device *sdev)
{
	if (sdev->dev)
		return sdev;
	if (pcipos > 0 && pcipos < num_devices)
		return sh_pci_devs[pcipos];
	return NULL;
}

#ifdef __i386__
#define I810_SIZE 1024

static unsigned long
i810_alloc_page(void)
{
	return (unsigned long)contigmalloc(PAGE_SIZE, M_SVGALIB, 0, 0, ~0, PAGE_SIZE, PAGE_SIZE);
}

static unsigned long i810_pages[I810_SIZE];
static unsigned int i810_gttes[I810_SIZE];
static unsigned long i810_gttpage=0;

static unsigned int
i810_make_gtt(void)
{
	int i;
	unsigned int gtte;

	if(i810_gttpage)
		return vtophys(i810_gttpage);

	i810_gttpage=i810_alloc_page();

	for(i=0; i<I810_SIZE; i++) {
		i810_pages[i] = i810_alloc_page();
		gtte = vtophys((void *)i810_pages[i]);
		gtte &= 0x3ffff000;
		gtte |= 1; /* valid, system memory, not cached */
		i810_gttes[i]=gtte;
	}

	return vtophys((void *)i810_gttpage);
}
#endif

static int
svga_ioctl(struct cdev *dev, u_long cmd, caddr_t data,
    int fflag, struct thread *td)
{
	struct sh_pci_device *sdev = dev->si_drv1;
	int i;
	io_t *iov=(io_t *)data;
	pcic_t *pciv=(pcic_t *)data;
	struct sh_pci_device *pdev;
	io_string_t *iostr=(io_string_t *)data;
	unsigned char *outb_str;
	int rc = 0;

	if(IOCGROUP(cmd)!=SVGAHELPER_MAGIC)
		return -EINVAL;

	switch (cmd) {
	case SVGAHELPER_REPOUTB:
		if (iostr->length>4096) {
			rc = -EINVAL;
			break;
		}
		outb_str = malloc(iostr->length, M_SVGALIB, M_WAITOK);
		if (outb_str == NULL ) {
			rc = -ENOMEM;
			break;
		}
		if (copyin(iostr->string, outb_str, iostr->length)) {
			free(outb_str, M_SVGALIB);
			rc = -EPERM;
			break;
		}
		if(check_io_range(iostr->port, sdev)) {
			for(i=0; i<iostr->length; i++)
				bus_space_write_1(sdev->tag, sdev->handle, iostr->port, outb_str[i]);
		}
		else
			rc = -EPERM;
		free(outb_str, M_SVGALIB);
		break;
	case SVGAHELPER_WRITEB:
	case SVGAHELPER_OUTB:
		if(check_io_range(iov->port, sdev))
			bus_space_write_1(sdev->tag, sdev->handle, iov->port, iov->val);
		else
			rc = -EPERM;
		break;
	case SVGAHELPER_WRITEW:
	case SVGAHELPER_OUTW:
		if(check_io_range(iov->port, sdev))
			bus_space_write_2(sdev->tag, sdev->handle, iov->port, iov->val);
		else
			rc = -EPERM;
		break;
	case SVGAHELPER_WRITEL:
	case SVGAHELPER_OUTL:
		if(check_io_range(iov->port, sdev))
			bus_space_write_4(sdev->tag, sdev->handle, iov->port, iov->val);
		else
			rc = -EPERM;
		break;
	case SVGAHELPER_READB:
	case SVGAHELPER_INB:
		if(check_io_range(iov->port, sdev)) {
			iov->val = bus_space_read_1(sdev->tag, sdev->handle, iov->port);
		}
		else
			rc = -EPERM;
		break;
	case SVGAHELPER_READW:
	case SVGAHELPER_INW:
		if(check_io_range(iov->port, sdev)) {
			iov->val = bus_space_read_2(sdev->tag, sdev->handle, iov->port);
		}
		else
			rc = -EPERM;
		break;
	case SVGAHELPER_READL:
	case SVGAHELPER_INL:
		if(check_io_range(iov->port, sdev)) {
			iov->val = bus_space_read_4(sdev->tag, sdev->handle, iov->port);
		}
		else
			rc = -EPERM;
		break;
	case SVGAHELPER_PCIINB:
		pdev = get_dev(pciv->pcipos, sdev);
		if (pdev)
			pciv->val = pci_read_config(pdev->dev, pciv->address, 1);
		else
			rc = -EINVAL;
		break;
	case SVGAHELPER_PCIINW:
		pdev = get_dev(pciv->pcipos, sdev);
		if (pdev)
			pciv->val = pci_read_config(pdev->dev, pciv->address, 2);
		else
			rc = -EINVAL;
		break;
	case SVGAHELPER_PCIINL:
		pdev = get_dev(pciv->pcipos, sdev);
		if (pdev)
			pciv->val = pci_read_config(pdev->dev, pciv->address, 4);
		else
			rc = -EINVAL;
		break;
	case SVGAHELPER_PCIAPLEN:
		if (pciv->address > 5) {
			rc = -EINVAL;
			break;
		}
		pdev = get_dev(pciv->pcipos, sdev);
		if (pdev)
			pciv->val = pdev->len[pciv->address];
		else
			rc = -EINVAL;
		break;
	case SVGAHELPER_PCIOUTB:
		pdev = get_dev(pciv->pcipos, sdev);
		if (pdev)
			pci_write_config(pdev->dev, pciv->address, 1, pciv->val);
		else
			rc = -EINVAL;
		break;
	case SVGAHELPER_PCIOUTW:
		pdev = get_dev(pciv->pcipos, sdev);
		if (pdev)
			pci_write_config(pdev->dev, pciv->address, 2, pciv->val);
		else
			rc = -EINVAL;
		break;
	case SVGAHELPER_PCIOUTL:
		pdev = get_dev(pciv->pcipos, sdev);
		if (pdev)
			pci_write_config(pdev->dev, pciv->address, 4, pciv->val);
		else
			rc = -EINVAL;
		break;
#ifdef __i386__
	case SVGAHELPER_I810GTT:
		*(unsigned int *)data = i810_make_gtt();
		break;
	case SVGAHELPER_I810GTTE:
		if (*(unsigned int *)data < I810_SIZE)
			*(unsigned int *)data = (unsigned int)&i810_gttes[*(unsigned int *)data];
		else
			return -EINVAL;
		break;
#endif
	case SVGAHELPER_WAITRETRACE:
		if (sdev->irqresource && sdev->irqcookie && sdev->enable_vsync) {
			sdev->enable_vsync(sdev);
			rc = tsleep(sdev, PCATCH, "vsync", hz);
		}
		else if (sdev == sh_pci_devs[0]) {
			void *cookies[MAX_NR_DEVICES] = {NULL};

			/* This waits for literally any vsync interrupt after enabling them all */
			vsync = 1;
			for (i=0; i<MAX_NR_DEVICES; i++) {
				if (sh_pci_devs[i]->dev == NULL || sh_pci_devs[i]->irqresource == NULL || sh_pci_devs[i]->enable_vsync != vga_enable_vsync)
					continue;
				if (bus_setup_intr(sh_pci_devs[i]->dev, sh_pci_devs[i]->irqresource, INTR_TYPE_MISC | INTR_MPSAFE, vsync_interrupt, NULL, sdev, &cookies[i]))
					cookies[i] = NULL;
			}
			sh_pci_devs[i]->enable_vsync(sh_pci_devs[i]);
			rc = tsleep(sdev, PCATCH, "vsync", hz);
			for (i=0; i<MAX_NR_DEVICES; i++) {
				if (sh_pci_devs[i]->irqcookie == NULL || sh_pci_devs[i]->dev == NULL || sh_pci_devs[i]->irqresource == NULL || sh_pci_devs[i]->enable_vsync != vga_enable_vsync)
					continue;
				bus_teardown_intr(sdev->dev, sdev->irqresource, sdev->irqcookie);
				vga_ack_vsync(sh_pci_devs[i]);
				cookies[i] = NULL;
			}
		}
		break;
#ifndef NO_TASK
	case SVGAHELPER_SETDISPLAYSTART:
		printf("TODO: Set Display Start\n");
		break;
#endif
	default:
		rc = -EINVAL;
		break;
	}

	return rc;
}

static int
check_mem(struct sh_pci_device *dev, unsigned long start, unsigned long len)
{
	int j;
	unsigned long rstart, rlen;
#ifdef __alpha__
	int type;
#endif

	if (start + len < start || len >= 0x40000000)
		return -3;

	rstart=start;
	rlen=len;

#ifdef __alpha__
	type = start>>32;
	switch(type) {
	case 2:
		start = start & 0xffffffff;
		start = start >> 5;
		len = len >> 5;
		break;
	case 3:
		start = start & 0xffffffff;
		break;
	default:
		return -1;
	}
#endif

	if(dev == sh_pci_devs[0]) {
		for(j=1;j<num_devices;j++) {
			if(!check_mem(sh_pci_devs[j], rstart, rlen))
				return 0;
		}
	}
	else if(dev && dev->dev) {
		/* TODO: Always allow the video aperture */
		if( (start >= 0xa0000 && start<0x110000) && (start+len<0x110000) )
			return 0;
		for(j=0;j<6;j++) {
			if(dev->mem[j]) {
				if((start >= dev->mem[j]) &&
				    (start + len <= dev->mem[j] + dev->len[j])) {
					return 0;
				}
			}
		}
	}

	return -2;
}

static int
svga_mmap(struct cdev *dev, vm_ooffset_t offset, vm_paddr_t *paddr,
		     int nprot, vm_memattr_t *memattr)
{
	struct sh_pci_device *sdev = dev->si_drv1;

	if(check_mem(sdev, offset, PAGE_SIZE))
		return -EPERM;
	*paddr = offset;
	return 0;
}

static struct cdev *svga_alias;
static int
svga_load(module_t module, int cmd, void *arg)
{
	int i;
	int rc = 0;
	uint32_t result, t, len;
	struct pci_devinfo *devinfo;

	switch (cmd) {
	case MOD_LOAD:
    		printf("svgalib_helper: Initializing, version %s\n", versionstr);

    		sh_pci_devs[0]=malloc(sizeof(*sh_pci_devs[0]), M_SVGALIB, M_WAITOK);
		if (sh_pci_devs[0] == NULL) {
			printf("Unable to allocate memory for svga_helper/0\n");
			return -ENOMEM;
		}

		memset(sh_pci_devs[0],0,sizeof(*sh_pci_devs[0]));
		sh_pci_devs[0]->irq = PCI_INVALID_IRQ;
		num_devices=1;

		sh_pci_devs[0]->cdev = make_dev(&svga_devsw, 0, UID_ROOT, GID_WHEEL, 0600, "svga_helper/0");
		if (sh_pci_devs[0]->cdev == NULL) {
			printf("Unable to create /dev/svga_helper/0\n");
			return -ENODEV;
		}
		sh_pci_devs[0]->cdev->si_drv1 = sh_pci_devs[0];

		svga_alias = make_dev_alias(sh_pci_devs[0]->cdev, "svga");
		if (svga_alias == NULL) {
			printf("Unable to alias /dev/svga_helper/0 to /dev/svga\n");
			return -ENODEV;
		}
		svga_alias->si_drv1 = sh_pci_devs[0];

		STAILQ_FOREACH(devinfo, &pci_devq, pci_links) {
			if (devinfo->conf.pc_class != PCIC_DISPLAY || devinfo->conf.pc_subclass != PCIS_DISPLAY_VGA)
				continue;
			if ((sh_pci_devs[num_devices] = malloc(sizeof(*sh_pci_devs[0]), M_SVGALIB, M_WAITOK))==NULL) {
				printf("Unable to allocate memory for svga_helper/%d\n", num_devices);
				return -ENOMEM;
			}
			memset(sh_pci_devs[num_devices], 0, sizeof(*sh_pci_devs[0]));
			sh_pci_devs[num_devices]->dev = devinfo->cfg.dev;
			sh_pci_devs[num_devices]->irq = devinfo->cfg.intline;
			sh_pci_devs[num_devices]->vendor = devinfo->conf.pc_vendor;
			sh_pci_devs[num_devices]->id = devinfo->conf.pc_device;
			sh_pci_devs[num_devices]->revision = devinfo->conf.pc_revid;
			printf("svgalib_helper: device%d: vendor:%.4x id:%.4x\n", num_devices,
			    sh_pci_devs[num_devices]->vendor, sh_pci_devs[num_devices]->id);
			for (i=0; i<6; i++) {
				result = pci_read_config(sh_pci_devs[num_devices]->dev, 16+4*i, 4);
				if (result) {
					pci_write_config(sh_pci_devs[num_devices]->dev, 16 + 4 * i, 0xffffffff, 4);
					t = pci_read_config(sh_pci_devs[num_devices]->dev, 16 + 4 * i, 4);
					pci_write_config(sh_pci_devs[num_devices]->dev, 16 + 4 * i, result, 4);
					len = ~(t & ~0xf) + 1;
					if (len) {
						/* TODO: We likely don't need this... */
						sh_pci_devs[num_devices]->len[i]=len;
						sh_pci_devs[num_devices]->mem[i]=result&~0xf;
#ifdef notyet
						sh_pci_devs[num_devices]->flags[i]=0x80 | (result&0xf);
						sh_pci_devs[num_devices]->mask[i]=t&~0xf;
#endif
						printf("device%d: region%d, base=%.8x len=%d type=%d\n",
						    num_devices, i, result&(~0xf), len, result&0xf);
					}
				}
			}

			rc = vga_init_vsync(sh_pci_devs[num_devices]);
			if (rc)
				return rc;
			sh_pci_devs[num_devices]->opencount=0;
			sh_pci_devs[num_devices]->cdev = make_dev(&svga_devsw, 0, UID_ROOT, GID_WHEEL, 0600, "svga_helper/%d", num_devices);
			if (sh_pci_devs[num_devices]->cdev == NULL) {
				printf("Unable to create /dev/svga_helper/0\n");
				return -ENODEV;
			}
			sh_pci_devs[num_devices]->cdev->si_drv1 = sh_pci_devs[num_devices];

			num_devices++;
		}

		break;
	case MOD_UNLOAD:
	case MOD_SHUTDOWN:
		destroy_dev(svga_alias);
		for (i = 0; i < MAX_NR_DEVICES; i++) {
			if (sh_pci_devs[i] != NULL) {
				destroy_dev(sh_pci_devs[i]->cdev);
				if (sh_pci_devs[i]->irqcookie)
					bus_teardown_intr(sh_pci_devs[i]->dev, sh_pci_devs[i]->irqresource, sh_pci_devs[i]->irqcookie);
				if (sh_pci_devs[i]->resource) {
					switch(sh_pci_devs[i]->resource_type) {
					case -1:
						bus_space_unmap(sh_pci_devs[i]->tag, sh_pci_devs[i]->handle, 0x3dc);
						break;
					default:
						bus_release_resource(sh_pci_devs[i]->dev, sh_pci_devs[i]->resource_type, sh_pci_devs[i]->rid, sh_pci_devs[i]->resource);
						break;
					}
				}
				if (sh_pci_devs[i]->irqresource)
						bus_release_resource(sh_pci_devs[i]->dev, SYS_RES_IRQ, sh_pci_devs[i]->irqid, sh_pci_devs[i]->irqresource);
				free(sh_pci_devs[i], M_SVGALIB);
				sh_pci_devs[i] = NULL;
			}
		}
		break;
	default:
		rc = EINVAL;
		break;
	}

	return(rc);
}

DEV_MODULE(svga, svga_load, NULL);
