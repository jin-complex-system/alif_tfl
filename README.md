# alif_tfl

# Setup Project

## How to run the project with Visual Studio Code

1. Using Visual Studio Code, open the directory at [`alif_src`](alif_src)
2. Configure instructions according to README.md provided
3. Pull the necessary submodules:
```bash
git submodule update --init --recursive
```

## How to Program on Visual Studio Code

1. Determine your COM port of your device. The lower COM port is for programming while the upper COM port is for UART messages
- On this example, `COM6` is for programming and `COM9` is `UART2` for Alif Ensemble Dev Kit HE core output
2. Open a UART program (like Putty) to the UART (`COM9`), with the settings at 115200 baud rate 
3. Navigate to CMSIS Extension
4. Click on the 'Buid Solution` icon (looks like a hammer)
5. Press `F1` on your keyboard and select `Tasks: Run Task`
6. Select `Program with Security Toolkit (select COM port)`
7. On the terminal, when the COM port list shows up, select the lowest COM port
- Type out the entire name `COM6` rather than just `6`
8. When programming is finished, view the UART console output

# How to create a new project `preprocess`
1. Copy `blinky` project as it is and rename the directory to `preprocess`
2. Inside [tasks.json](/alif_src/.vscode/tasks.json), add the following at line 15:
```json
        {
            "label": "Clean all (including YML build files)",
            "type": "shell",
            "command": [
                "cbuild ${command:cmsis-csolution.getSolutionPath} --clean;",
                "rm -f ./blinky/*.cbuild.yml;",
                "rm -f ./hello/*.cbuild.yml;",
                "rm -f ./hello_rtt/*.cbuild.yml;",
                "rm -f ./preprocess/*.cbuild.yml;"
            ],
            "windows": {
                "command": [
                    "cbuild ${command:cmsis-csolution.getSolutionPath} --clean;",
                    "rm -Force ./blinky/*.cbuild.yml;",
                    "rm -Force ./hello/*.cbuild.yml;",
                    "rm -Force ./hello_rtt/*.cbuild.yml;",
                    "rm -Force ./preprocess/*.cbuild.yml;"
                ]
            },
            "problemMatcher": []
        },
```
3. Inside [alif.csolution.yml](/alif_src/alif.csolution.yml), add the following line at line 45:
```yml
  projects:
    - project: blinky/blinky.cproject.yml
    - project: hello/hello.cproject.yml
    - project: hello_rtt/hello_rtt.cproject.yml
    - project: preprocess/preprocess.cproject.yml
```
4. Locate the CMSIS packs. In Windows, it is located in `C:\Users\$USER\AppData\Local\Arm\Packs\AlifSemiconductor\Ensemble\1.3.4`, where the current version is `1.3.4`

5. Copy over the following directories to `alif_src\libs`:
- `FatFS` directory from `C:\Users\$USER\AppData\Local\Arm\Packs\AlifSemiconductor\Ensemble\1.3.4\Boards\DevKit-e7\Templates\Baremetal`'

6. Inside your new directory `preprocess`, change `preprocess.cproject.yml` to the following:
```yml
project:
  groups:
    - group: App
      files:
        - file: main.c
    - group: Board
      files:
        - file: ../libs/board/devkit_gen2/board_init.c
        - file: ../libs/board/appkit_gen2/board_init.c
        - file: ../libs/common_app_utils/logging/uart_tracelib.c
        - file: ../libs/common_app_utils/logging/retarget.c
        - file: ../libs/common_app_utils/fault_handler/fault_handler.c
    - group: FatFS
      files:
        - file: ../libs/FatFS/diskio.h
        - file: ../libs/FatFS/diskio.c
        - file: ../libs/FatFS/ff.h
        - file: ../libs/FatFS/ff.c
        - file: ../libs/FatFS/ffconf.h
        - file: ../libs/FatFS/ffsystem.c
        - file: ../libs/FatFS/ffunicode.c

  output:
    base-name: $Project$
    type:
      - elf
      - bin
  add-path:
    - .
    - ../board/
    - ../libs/board/
    - ../libs/FatFS/
    - ../libs/common_app_utils/logging
    - ../libs/common_app_utils/fault_handler
  components:
    # needed for Alif Ensemble support
    - component: AlifSemiconductor::Device:Startup&C Startup
    - component: ARM::CMSIS:CORE

    # peripheral drivers & middleware, uncomment as needed
    # - component: AlifSemiconductor::Device:SOC Peripherals:ADC
    # - component: AlifSemiconductor::Device:SOC Peripherals:CAN-FD
    # - component: AlifSemiconductor::Device:SOC Peripherals:CDC
    # - component: AlifSemiconductor::Device:SOC Peripherals:CPI
    # - component: AlifSemiconductor::Device:SOC Peripherals:CRC
    # - component: AlifSemiconductor::Device:SOC Peripherals:DAC
    # - component: AlifSemiconductor::Device:SOC Peripherals:DMA
    # - component: AlifSemiconductor::Device:SOC Peripherals:Ethernet MAC
    - component: AlifSemiconductor::Device:SOC Peripherals:GPIO
    # - component: AlifSemiconductor::Device:SOC Peripherals:HSCMP
    # - component: AlifSemiconductor::Device:SOC Peripherals:HWSEM
    # - component: AlifSemiconductor::Device:SOC Peripherals:I2C
    # - component: AlifSemiconductor::Device:SOC Peripherals:I2C_I3C
    # - component: AlifSemiconductor::Device:SOC Peripherals:I2S
    # - component: AlifSemiconductor::Device:SOC Peripherals:I3C
    # - component: AlifSemiconductor::Device:SOC Peripherals:LPI2C
    # - component: AlifSemiconductor::Device:SOC Peripherals:LPTIMER
    - component: AlifSemiconductor::Device:SOC Peripherals:MHU
    # - component: AlifSemiconductor::Device:SOC Peripherals:MIPI DSI CSI2 DPHY
    # - component: AlifSemiconductor::Device:SOC Peripherals:MIPI CSI2
    # - component: AlifSemiconductor::Device:SOC Peripherals:MIPI DSI
    # - component: AlifSemiconductor::Device:SOC Peripherals:MRAM
    # - component: AlifSemiconductor::Device:SOC Peripherals:OSPI
    # - component: AlifSemiconductor::Device:SOC Peripherals:PDM
    - component: AlifSemiconductor::Device:SOC Peripherals:PINCONF
    # - component: AlifSemiconductor::Device:SOC Peripherals:RTC
    - component: AlifSemiconductor::Device:SOC Peripherals:SDMMC
    # - component: AlifSemiconductor::Device:SOC Peripherals:SPI
    - component: AlifSemiconductor::Device:SOC Peripherals:USART
    # - component: AlifSemiconductor::Device:SOC Peripherals:UTIMER
    # - component: AlifSemiconductor::Device:SOC Peripherals:WDT

    # - component: AlifSemiconductor::Device:OSPI FLASH XIP:core
    # - component: AlifSemiconductor::Device:OSPI FLASH XIP:utility
    # - component: AlifSemiconductor::Device:OSPI HYPERRAM XIP
    - component: AlifSemiconductor::Device:SE runtime Services:Initialization Helper&Source
    - component: AlifSemiconductor::Device:SE runtime Services:core&Source

    # - component: AlifSemiconductor::BSP:External peripherals:CAMERA Sensor MT9M114
    # - component: AlifSemiconductor::BSP:External peripherals:CAMERA Sensor ARX3A0
    # - component: AlifSemiconductor::BSP:External peripherals:CAMERA Sensor AR0144
    # - component: AlifSemiconductor::BSP:External peripherals:Ethernet PHY
    # - component: AlifSemiconductor::BSP:External peripherals:GT911 Touch Controller
    # - component: AlifSemiconductor::BSP:External peripherals:ILI9806E LCD panel
    # - component: AlifSemiconductor::BSP:External peripherals:OSPI Flash ISSI
    # - component: AlifSemiconductor::Device:Conductor Tool support
    # - component: AlifSemiconductor::Device:Power Management
    # - component: AlifSemiconductor::Device:Retarget IO:STDERR
    # - component: AlifSemiconductor::Device:Retarget IO:STDIN
    # - component: AlifSemiconductor::Device:Retarget IO:STDOUT
```
7. Pull in the DSP Preprocessing Library into `alif_src/libs`:
```bash
cd alif_src/libs
git submodule add --name dsp_preprocess https://github.com/jin-complex-system/dsp_preprocessing
cd dsp_preprocessing && git checkout <desired_branch>
```
