# npu_driver

High-level NPU driver to interface with NPU and Ethos-U55 to allow NPU functionality in M55 HE core.

# Necessary Libraries/Files
- `M55_HE.h`
- `M55_HE_map.h`
- `RTE_Components.h`
- Ethos-U driver from ARM pack `ethos-u-core-driver`

# Files

- [`include/NPU_definitions.h`](include/NPU_definitions.h)
- [`include/NPU_driver.h`](include/NPU_driver.h)
- [`src/npu_driver.c`](src/npu_driver.c)

# Reference

- [NPUInit() inside https://github.com/alifsemi/alif_mlek-cmsis-examples](https://github.com/alifsemi/alif_mlek-cmsis-examples/blob/251a684eac47f4ba692dceb79a558b3ebf936789/device/alif-ensemble/src/BoardInit.cpp#L46)
- [Arm® Ethos™-N Driver Stack](https://github.com/ARM-software/ethos-n-driver-stack)
- [Alifsemi's CMSIS Machine Learning Examples](https://github.com/alifsemi/alif_mlek-cmsis-examples)
