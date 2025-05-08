# alif_tfl

# Setup for Ubuntu 22.04 Docker
- Modified from [Evaluation Setup for Ubuntu](https://github.com/alifsemi/alif_ml-embedded-evaluation-kit/blob/main/ML_Embedded_Evaluation_Kit.md#ubuntu-setup)

1. Create a docker Linux container:
```bash
mkdir alif_stuff && cd alif_stuff
docker run -it -v "$(pwd):/mydir" --gpus all --network host -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix ubuntu:22.04
```

2. Update and install the necessary packages:
```bash
apt update && apt upgrade -y

# For pulling the project files
apt install git cmake xz-utils wget

# Install arm-related depencies
apt install libncurses5 libc6-i386 lib32gcc-s1 lib32stdc++6 # lib32z
apt install curl dos2unix # python-is-python

# Install python
apt-get install python3.10 python3.10-venv libpython3.10 libpython3.10-dev

# Verify python3 version 
python3 --version
```

3. From https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads, download for x86_64 Linux hosted crosstoolchain arm-gnu-toolchain:
```bash
mkdir arm_gnu_compiler && cd arm_gnu_compiler
wget https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-aarch64-arm-none-eabi.tar.xz
```

4. Extract files from `.tar.xz` and place it into `/usr/local`:
```bash
tar xf arm-gnu-toolchain-14.2.rel1-x86_64-arm
-none-eabi.tar.xz -C . 
cd arm-gnu-toolchain-14.2.rel1-x86_64-arm-non
e-eabi/

# Probably a better way to copy over files
cp -r . /usr/local/.
```

5. Add compiler to path:
```
sh -c "echo export PATH=/usr/local/bin:$PATH > /etc/profile.d/arm-compiler.sh"
```

6. Clone the git repository to the directory
```bash
cd /mydir
git clone https://github.com/alifsemi/alif_ml-embedded-evaluation-kit.git
```

7. Initialise and update the required submodules
```bash
cd alif_ml-embedded-evaluation-kit
# git checkout 24.05 # Unsure if this is needed
git submodule update --init
```

8. Download and setup the required AI/ML resources
```bash
python3.10 set_up_default_resources.py --additional-ethos-u-config-name ethos-u55-128
```

9. Convert your `.tflite` model to target the NPU, and into a header file:
```bash
chmod +777 resources_downloaded/env/bin/activate
resources_downloaded/env/bin/activate

mkdir models && cd models
cp <target_directory>/CNN_litert.tflite . 

chmod +777 resources_downloaded/env/bin/vela
../resources_downloaded/env/bin/vela --accelerator-config=ethos-u55-128 \
  --optimise Performance \
  --config ../scripts/vela/default_vela.ini \
  --memory-mode=Shared_Sram \
  --system-config=Ethos_U55_High_End_Embedded \
  --output-dir=. \
  CNN_litert.tflite

# Verify vela file exist before we convert into a header file
ls | grep CNN_litert_vela.tflite 
xxd -i CNN_litert_vela.tflite > CNN_litert_vela.h
```

10. Create a build directory for M55-HE core for a Generic Inference Runner
```bash
cd /mydir/alif_ml-embedded-evaluation-kit
mkdir build_he_infrun && cd build_he_infrun
```

11. Address known issues as described in [known_isses.md](/known_issues.md)

12. Configure the build using `CMake` and build:
```bash
cmake .. \
  -Dinference_runner_MODEL_TFLITE_PATH=/mydir/alif_ml-embedded-evaluation-kit/models/CNN_litert_vela.tflite \
  -DUSE_CASE_BUILD=inference_runner \
  -DTARGET_PLATFORM=ensemble \
  -DGLCD_UI=OFF \
  -DTARGET_SUBSYSTEM=RTSS-HE \
  -DTARGET_BOARD=DevKit \
  -DTARGET_REVISION=B \
  -DLINKER_SCRIPT_NAME=ensemble-RTSS-HE-infrun \
  -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-gcc.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DLOG_LEVEL=LOG_LEVEL_Debug
make -j16
```
