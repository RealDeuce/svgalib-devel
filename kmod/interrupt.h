int vga_init_vsync(struct sh_pci_device *);
int vga_test_vsync(struct sh_pci_device *);
void vga_ack_vsync(struct sh_pci_device *);
void vga_enable_vsync(struct sh_pci_device *);

int s3_test_vsync(struct sh_pci_device *dev);
void s3_ack_vsync(struct sh_pci_device *);
void s3_enable_vsync(struct sh_pci_device *);

int nv3_test_vsync(struct sh_pci_device *dev);
void nv3_ack_vsync(struct sh_pci_device *);
void nv3_enable_vsync(struct sh_pci_device *);

int nv4_test_vsync(struct sh_pci_device *dev);
void nv4_ack_vsync(struct sh_pci_device *);
void nv4_enable_vsync(struct sh_pci_device *);

int r128_test_vsync(struct sh_pci_device *dev);
void r128_ack_vsync(struct sh_pci_device *);
void r128_enable_vsync(struct sh_pci_device *);

int rage_test_vsync(struct sh_pci_device *dev);
void rage_ack_vsync(struct sh_pci_device *);
void rage_enable_vsync(struct sh_pci_device *);

int rendition_test_vsync(struct sh_pci_device *dev);
void rendition_ack_vsync(struct sh_pci_device *);
void rendition_enable_vsync(struct sh_pci_device *);
