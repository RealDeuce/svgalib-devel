This is a port of the 1.9.25 development version of SVGAlib to FreeBSD.

There is a kernel module port as well to allow running as non-root.

The primary purpose is to allow an arcade monitor to be used with AdvanceMAME
on FreeBSD.  I can't think of any other reason someone would prefer to use this
to something more modern and maintained.

There are bad interactions between vt(4) and SVGAlib since KD_GRAPHICS doesn't
stop output to the VGA device.  It is reccomended that you switch to using 
syscons(4) instead by adding the line "kern.vty=sc" to /boot/loader.conf.

I'll be committing this to the ports collection once I have it working with
AdvanceMAME on my system and I do at least some testing on -STABLE.

At present, the kernel module is still not completely locked down.  It will
allow use of the entire legacy I/O port range, and the majority of low memory
areas.  Also, the ioctl()s do not properly check and sanitize permissions and
inputs.  In short, at present the kernel module as currently implemented may
actually be less secure than setuid root binaries.
