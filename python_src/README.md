# Summary

Collection of Python scripts that run in host's Windows and WSL.

Since tensorflow no longer exists in Windows, we have to run our Python scripts in WSL.

# Setup

## Setup (for both Windows and WSL)

1. Clone the git repository inside your WSL and navigate into the Python script directory
```bash
git clone https://github.com/jin-complex-system/alif_tfl
cd alif_tfl/python_src
```

2. Active Python virtual environment
```bash
mkdir venv
python -m venv venv/.
chmod +777 venv/bin/activate
source venv/bin/activate
```

3. Install necessary requirements if needed
```bash
pip install -r requirements.txt
```

## WSL Setup for COM port exposure

1. Follow the instructions in [StackOverflow](https://askubuntu.com/a/1508361) to expose the COM ports to WSL2. Note that the device is most likely a USB serial adaptor (check by disconnecting and connecting)

    - Console from Windows PowerShell:
```bash
PS C:\WINDOWS\system32> usbipd list
Connected:
BUSID  VID:PID    DEVICE                                                        STATE
1-3    187c:0550  USB Input Device                                              Not shared
1-5    04b4:0005  USB-Serial Adapter 1, USB-Serial Adapter 2, USB-Serial (D...  Not shared
1-7    046d:c335  G910, USB Input Device, Virtual HID Framework (VHF) HID d...  Not shared
1-8    1532:0084  Razer DeathAdder V2, USB Input Device                         Not shared
1-14   8087:0032  Intel(R) Wireless Bluetooth(R)                                Not shared

Persisted:
GUID                                  DEVICE

PS C:\WINDOWS\system32> usbipd bind --busid 1-5
PS C:\WINDOWS\system32> usbipd list
Connected:
BUSID  VID:PID    DEVICE                                                        STATE
1-3    187c:0550  USB Input Device                                              Not shared
1-5    04b4:0005  USB-Serial Adapter 1, USB-Serial Adapter 2, USB-Serial (D...  Shared
1-7    046d:c335  G910, USB Input Device, Virtual HID Framework (VHF) HID d...  Not shared
1-8    1532:0084  Razer DeathAdder V2, USB Input Device                         Not shared
1-14   8087:0032  Intel(R) Wireless Bluetooth(R)                                Not shared

Persisted:
GUID                                  DEVICE

PS C:\WINDOWS\system32> usbipd attach --wsl --busid 1-5
usbipd: info: Using WSL distribution 'Ubuntu-20.04' to attach; the device will be available in all WSL 2 distributions.
usbipd: info: Detected networking mode 'nat'.
usbipd: info: Using IP address 192.168.176.1 to reach the host.
PS C:\WINDOWS\system32>
```

- Console output from WSL:
```bash
csi_3@CSIDEV3:~$ ls /dev/ttyACM*
/dev/ttyACM0  /dev/ttyACM1
```

2. To detach the COM port, run the following in the Windows Powershell:
```bash
PS C:\WINDOWS\system32> usbipd detach --busid 1-5
PS C:\WINDOWS\system32> usbipd list
Connected:
BUSID  VID:PID    DEVICE                                                        STATE
1-3    187c:0550  USB Input Device                                              Not shared
1-5    04b4:0005  USB-Serial Adapter 1, USB-Serial Adapter 2, USB-Serial (D...  Shared
1-7    046d:c335  G910, USB Input Device, Virtual HID Framework (VHF) HID d...  Not shared
1-8    1532:0084  Razer DeathAdder V2, USB Input Device                         Not shared
1-14   8087:0032  Intel(R) Wireless Bluetooth(R)                                Not shared

Persisted:
GUID                                  DEVICE

PS C:\WINDOWS\system32>
```

# Python scripts on Windows

## Parse Output from SD card

1. To parse output from the SD card, copy the output directory to `python_src`:
```bash
cp -r <sd_card_location>/out_A <alif_tfl_repo_path>/python_src
```
- Example is using `out_A`. If needed, change the directory inside `parse_output.py` before running

2. Run [`parse_output.py`](parse_output.py) and read the output directory
```bash
python3 parse_output.py
cd _my_results
```

## Parse Output from UART

1. Run [`parse_uart_for_results.py`](parse_uart_for_results.py) and read the output directory
```bash
python3 parse_uart_for_results.py
cd _my_results
```

# Python scripts on WSL

## Export results

1. Copy directories to WSL
2. Inside `exports_outputs.py`, change the output directory if needed
3. Run [`export_results.py`](export_outputs.py)
