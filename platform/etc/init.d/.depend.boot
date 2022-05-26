TARGETS = console-setup mountkernfs.sh resolvconf hostname.sh udev keyboard-setup mountdevsubfs.sh procps networking urandom hwclock.sh checkroot.sh bootmisc.sh kmod mountall-bootclean.sh mountall.sh checkfs.sh checkroot-bootclean.sh mountnfs.sh mountnfs-bootclean.sh
INTERACTIVE = console-setup udev keyboard-setup checkroot.sh checkfs.sh
udev: mountkernfs.sh
keyboard-setup: mountkernfs.sh udev
mountdevsubfs.sh: mountkernfs.sh udev
procps: mountkernfs.sh udev
networking: mountkernfs.sh urandom resolvconf procps
urandom: hwclock.sh
hwclock.sh: mountdevsubfs.sh
checkroot.sh: hwclock.sh keyboard-setup mountdevsubfs.sh hostname.sh
bootmisc.sh: udev mountall-bootclean.sh mountnfs-bootclean.sh checkroot-bootclean.sh
kmod: checkroot.sh
mountall-bootclean.sh: mountall.sh
mountall.sh: checkfs.sh checkroot-bootclean.sh
checkfs.sh: checkroot.sh
checkroot-bootclean.sh: checkroot.sh
mountnfs.sh: networking
mountnfs-bootclean.sh: mountnfs.sh
