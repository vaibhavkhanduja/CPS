cmd_/mnt/d/work/projects/cps/CPS/src/cps_kmodule/Module.symvers := sed 's/\.ko$$/\.o/' /mnt/d/work/projects/cps/CPS/src/cps_kmodule/modules.order | scripts/mod/modpost    -o /mnt/d/work/projects/cps/CPS/src/cps_kmodule/Module.symvers -e    -w -T -
