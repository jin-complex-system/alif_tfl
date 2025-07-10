# alif_src

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

## Project Recreation

To re-create the project from scratch, see [`recreate_new_project_from_blinky.md`](docs/recreate_new_project_from_blinky.md)
