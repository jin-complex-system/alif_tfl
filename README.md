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
3. Change [alif.csolution.yml](/alif_src/alif.csolution.yml) to the following:
```yml
# yaml-language-server: $schema=https://raw.githubusercontent.com/Open-CMSIS-Pack/devtools/tools/projmgr/2.6.0/tools/projmgr/schemas/csolution.schema.json
solution:
  created-for: cmsis-toolbox@2.6.0
  cdefault:

  # List of tested compilers that can be selected
  select-compiler:
    - compiler: GCC

  # Select used compiler
  compiler: GCC
  misc:
    - C:
        - -std=c99
        - -fdata-sections
        - -flax-vector-conversions
        - -fno-exceptions
        - -fno-rtti

  packs:
    - pack: AlifSemiconductor::Ensemble
    - pack: ARM::CMSIS
    - pack: ARM::CMSIS-DSP
    - pack: ARM::CMSIS-NN # Unsure which version
    - pack: ARM::ethos-u-core-driver@1.24.11

    # TensorFlow Lite Micro
    - pack: tensorflow::flatbuffers@1.24.11
    - pack: tensorflow::gemmlowp@1.24.11
    - pack: tensorflow::kissfft@1.24.11
    - pack: tensorflow::ruy@1.24.11
    - pack: tensorflow::tensorflow-lite-micro@1.24.11

  target-types:
    - type: HE
      device: Alif Semiconductor::AE722F80F55D5LS:M55_HE
      define:
        - "CORE_M55_HE"
    - type: HP
      device: Alif Semiconductor::AE722F80F55D5LS:M55_HP
      define:
        - "CORE_M55_HP"

  build-types:
    - type: debug
      optimize: speed
      debug: off
      define:
        - _DEBUG
    - type: release
      optimize: speed
      debug: off

  define:
    # Place CMake options here?
    - UNICODE
    - _UNICODE
    - ETHOSU55
    - DISABLEFLOAT16
    - Ofast

  projects:
    - project: blinky/blinky.cproject.yml
    - project: hello/hello.cproject.yml
    - project: hello_rtt/hello_rtt.cproject.yml
    - project: test_dsp_preprocessing/test_dsp_preprocessing.cproject.yml
    - project: preprocess/preprocess.cproject.yml

```
- Note that versions of different CMSIS-Packs may need to be changed in the future

4. Locate the CMSIS packs. In Windows, it is located in `C:\Users\$USER\AppData\Local\Arm\Packs\AlifSemiconductor\Ensemble\1.3.0`, where the current version is `1.3.0`

5. Copy over the following directories to `alif_src\libs`:
- `FatFS` directory from `C:\Users\$USER\AppData\Local\Arm\Packs\AlifSemiconductor\Ensemble\1.3.4\Boards\DevKit-e7\Templates\Baremetal`'

6. Pull in the DSP Preprocessing Library into `alif_src/libs`:
```bash
cd alif_src/libs
git submodule add --name dsp_preprocess https://github.com/jin-complex-system/dsp_preprocessing
cd dsp_preprocessing && git checkout <desired_branch>

7. Inside your new directory `preprocess`, change `preprocess.cproject.yml` to the following:
```yml
# yaml-language-server: $schema=https://raw.githubusercontent.com/Open-CMSIS-Pack/devtools/tools/projmgr/2.6.0/tools/projmgr/schemas/cproject.schema.json
project:
  groups:
    - group: App
      files:
        - file: parameters.h
        - file: main.c
        - file: app/app.h
        - file: app/app.c
        - file: led/led.h
        - file: led/led.c
        - file: sd_card/sd_card.h
        - file: sd_card/sd_card.c
        - file: inference/inference_definitions.h
        - file: inference/inference_tf.h
        - file: inference/inference_tf.cpp
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
    - group: models
      files:
        - file: ../models/model_orbw_19_Q_HE_vela.h
    - group: dsp_preprocess
      files:
        # # supporting library for dsp_preprocess
        # - file: ../libs/dsp_preprocessing/external/kissfft/kiss_fft.h
        # - file: ../libs/dsp_preprocessing/external/kissfft/kiss_fft.c
        # - file: ../libs/dsp_preprocessing/external/kissfft/kiss_fftr.h
        # - file: ../libs/dsp_preprocessing/external/kissfft/kiss_fftr.c
        # - file: ../libs/dsp_preprocessing/external/kissfft/_kiss_fft_guts.h

        # utils
        - file: ../libs/dsp_preprocessing/utils/include/convert_complex.h
        - file: ../libs/dsp_preprocessing/utils/src/convert_complex.c
        - file: ../libs/dsp_preprocessing/utils/include/insertion_sort.h
        - file: ../libs/dsp_preprocessing/utils/src/insertion_sort.c
        - file: ../libs/dsp_preprocessing/utils/include/log_approximation.h
        - file: ../libs/dsp_preprocessing/utils/src/log_approximation.c
        - file: ../libs/dsp_preprocessing/utils/include/square_root_approximation.h
        - file: ../libs/dsp_preprocessing/utils/src/square_root_approximation.c

        # statistics
        - file: ../libs/dsp_preprocessing/statistics/include/statistics.h
        - file: ../libs/dsp_preprocessing/statistics/src/statistics.c

        # audio_dsp
        - file: ../libs/dsp_preprocessing/audio_dsp/include/audio_dsp_fft.h
        - file: ../libs/dsp_preprocessing/audio_dsp/src/audio_dsp_fft.c
        - file: ../libs/dsp_preprocessing/audio_dsp/include/hann_window_compute.h
        - file: ../libs/dsp_preprocessing/audio_dsp/src/hann_window_compute.c
        - file: ../libs/dsp_preprocessing/audio_dsp/include/mel_spectrogram.h
        - file: ../libs/dsp_preprocessing/audio_dsp/src/mel_get_precomputed.c
        - file: ../libs/dsp_preprocessing/audio_dsp/src/mel_spectrogram.c
        - file: ../libs/dsp_preprocessing/audio_dsp/src/mel_utils.c
        - file: ../libs/dsp_preprocessing/audio_dsp/include/power_spectrum.h
        - file: ../libs/dsp_preprocessing/audio_dsp/src/power_spectrum.c
        - file: ../libs/dsp_preprocessing/audio_dsp/include/power_to_decibel.h
        - file: ../libs/dsp_preprocessing/audio_dsp/src/power_to_decibel.c

        # audio_dsp constants - hann window
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_window/hann_window/hann_window_scale_1024.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_window/hann_window/hann_window_no_scale_1024.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_window/hann_window/hann_window_scale_2048.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_window/hann_window/hann_window_no_scale_2048.h

        # audio_dsp constants - mel float
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_32_fft_1024_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_32_fft_1024_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_32_fft_1024_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_32_fft_1024_sr_44100_fmax_8000.h

        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_64_fft_1024_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_64_fft_1024_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_64_fft_1024_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_64_fft_1024_sr_44100_fmax_8000.h

        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_32_fft_2048_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_32_fft_2048_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_32_fft_2048_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_32_fft_2048_sr_44100_fmax_8000.h

        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_64_fft_2048_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_64_fft_2048_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_64_fft_2048_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_64_fft_2048_sr_44100_fmax_8000.h

        # audio_dsp constants - Mel next bin
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_32_fft_1024_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_32_fft_1024_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_32_fft_1024_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_32_fft_1024_sr_44100_fmax_8000.h

        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_64_fft_1024_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_64_fft_1024_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_64_fft_1024_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_64_fft_1024_sr_44100_fmax_8000.h

        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_32_fft_2048_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_32_fft_2048_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_32_fft_2048_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_32_fft_2048_sr_44100_fmax_8000.h

        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_64_fft_2048_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_64_fft_2048_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_64_fft_2048_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_64_fft_2048_sr_44100_fmax_8000.h

        # Precomputed Constants - Mel prev bin
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_32_fft_1024_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_32_fft_1024_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_32_fft_1024_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_32_fft_1024_sr_44100_fmax_8000.h

        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_64_fft_1024_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_64_fft_1024_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_64_fft_1024_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_64_fft_1024_sr_44100_fmax_8000.h

        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_32_fft_2048_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_32_fft_2048_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_32_fft_2048_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_32_fft_2048_sr_44100_fmax_8000.h

        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_64_fft_2048_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_64_fft_2048_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_64_fft_2048_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_64_fft_2048_sr_44100_fmax_8000.h

        # Precomputed Constants - Mel weights
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_32_fft_1024_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_32_fft_1024_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_32_fft_1024_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_32_fft_1024_sr_44100_fmax_8000.h

        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_64_fft_1024_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_64_fft_1024_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_64_fft_1024_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_64_fft_1024_sr_44100_fmax_8000.h

        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_32_fft_2048_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_32_fft_2048_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_32_fft_2048_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_32_fft_2048_sr_44100_fmax_8000.h

        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_64_fft_2048_sr_22048_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_64_fft_2048_sr_22048_fmax_8000.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_64_fft_2048_sr_44100_fmax_0.h
        - file: ../libs/dsp_preprocessing/audio_dsp/include/precomputed_mel/mel_weights/mel_frequency_weights_mel_64_fft_2048_sr_44100_fmax_8000.h

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

    # Models
    - ../models

    # # dsp_preprocess: supporting library for dsp_preprocess
    # - ../libs/dsp_preprocessing/external/kissfft/

    # dsp_preprocess: utils
    - ../libs/dsp_preprocessing/utils/include/

    # dsp_preprocess: statistics
    - ../libs/dsp_preprocessing/statistics/include/

    # dsp_preprocess: audio_dsp
    - ../libs/dsp_preprocessing/audio_dsp/include/

    # dsp_preprocess: audio_dsp constants - hann window
    - ./libs/dsp_preprocessing/audio_dsp/include/precomputed_window/hann_window/hann_window_no_scale_1024.h

    # dsp_preprocess: audio_dsp constants - mel constants
    - ../libs/audio_dsp/include/precomputed_mel/mel_centre_frequencies_float
    - ../libs/audio_dsp/include/precomputed_mel/mel_centre_frequencies_next_bin
    - ../libs/audio_dsp/include/precomputed_mel/mel_centre_frequencies_prev_bin
    - ../libs/audio_dsp/include/precomputed_mel/mel_weights

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
    - component: AlifSemiconductor::Device:SE runtime Services:Initialization
        Helper&Source
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

    # CMSIS
    - component: ARM::CMSIS:DSP
    - component: ARM::CMSIS:NN Lib

    # TensorFlow Lite Micro
    - component: Machine Learning:TensorFlow:Kernel&Ethos-U
    - component: Arm::Machine Learning:NPU Support:Ethos-U Driver&Generic U55
    - component: tensorflow::Data Exchange:Serialization:flatbuffers
    - component: tensorflow::Data Processing:Math:gemmlowp fixed-point
    - component: tensorflow::Data Processing:Math:kissfft
    - component: tensorflow::Data Processing:Math:ruy
    - component: tensorflow::Machine Learning:TensorFlow:Kernel Utils
  packs:
    - pack: Arm::ethos-u-core-driver

```

8. Copy the memory configuration from [`alif-e7-m55-he.ld`](https://github.com/alifsemi/alif_mlek-cmsis-examples/blob/main/kws/linker/alif-e7-m55-he.ld) to [`preprocess/RTE/Device/AE722F80F55D5LS_M55_HE/gcc_M55_HE.ld`](alif_tfl/preprocess/RTE/Device/AE722F80F55D5LS_M55_HE/gcc_M55_HE.ld)
- Add and remove buffers as needed in `.bss.at_sram0`

9. See [Known Issues](#known-issues) for any issues during building process

## Known Issues

1. [float16 support failure in CMSIS-DSP](github.com/ARM-software/CMSIS-DSP/issues/242) with [float16_issue.md](/float16_issue.md)
2. Pass C flag `-flax-vector-conversions`
3. Address [TFL Micro issues](tfl_micro_issues.md)

# References

- [ML Developers Guide for Cortex-M Processors
and Ethos-U NPU](https://documentation-service.arm.com/static/6821edae8f79851ff2c3e485?token=)

