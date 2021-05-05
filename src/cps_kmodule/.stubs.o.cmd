cmd_/mnt/d/work/projects/cps/CPS/src/cps_kmodule/stubs.o := gcc -Wp,-MMD,/mnt/d/work/projects/cps/CPS/src/cps_kmodule/.stubs.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/7/include -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -D__KERNEL__ -D__ASSEMBLY__ -fno-PIE -m64  -DMODULE  -c -o /mnt/d/work/projects/cps/CPS/src/cps_kmodule/stubs.o /mnt/d/work/projects/cps/CPS/src/cps_kmodule/stubs.S

source_/mnt/d/work/projects/cps/CPS/src/cps_kmodule/stubs.o := /mnt/d/work/projects/cps/CPS/src/cps_kmodule/stubs.S

deps_/mnt/d/work/projects/cps/CPS/src/cps_kmodule/stubs.o := \
    $(wildcard include/config/X86_32) \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/IA32_EMULATION) \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \

/mnt/d/work/projects/cps/CPS/src/cps_kmodule/stubs.o: $(deps_/mnt/d/work/projects/cps/CPS/src/cps_kmodule/stubs.o)

$(deps_/mnt/d/work/projects/cps/CPS/src/cps_kmodule/stubs.o):
