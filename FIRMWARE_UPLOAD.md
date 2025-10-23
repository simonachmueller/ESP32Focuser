# Firmware Upload Guide

This guide explains how to upload firmware to your ESP32Focuser microcontroller.

## Prerequisites

Before uploading firmware, ensure you have the following:

### Hardware Requirements
- ESP32 development board (as used in the ESP32Focuser hardware)
- USB cable (typically USB-A to Micro-USB or USB-C, depending on your board)
- Computer with USB port

### Software Requirements
- **USB to UART Bridge Driver**: CP210x driver from [Silicon Labs](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)
  - Windows usually installs this automatically
  - Linux and macOS typically have built-in support

## Method 1: Using PlatformIO (Recommended for Developers)

PlatformIO is the recommended method if you want to build from source or make modifications.

### Installation

1. **Install Visual Studio Code**
   - Download from [https://code.visualstudio.com/](https://code.visualstudio.com/)

2. **Install PlatformIO IDE Extension**
   - Open VS Code
   - Go to Extensions (Ctrl+Shift+X / Cmd+Shift+X)
   - Search for "PlatformIO IDE"
   - Click Install

### Uploading Firmware

1. **Clone or Download the Repository**
   ```bash
   git clone https://github.com/simonachmueller/ESP32Focuser.git
   cd ESP32Focuser
   ```

2. **Open Project in VS Code**
   - Open VS Code
   - File → Open Folder
   - Select the ESP32Focuser directory

3. **Connect Your ESP32**
   - Connect your ESP32 board to your computer via USB
   - The board should be recognized as a serial port

4. **Configure Upload Port (if needed)**
   - Edit `platformio.ini` and update the `upload_port` line:
     - **Windows**: `COM3`, `COM4`, etc.
     - **Linux**: `/dev/ttyUSB0`, `/dev/ttyUSB1`, etc.
     - **macOS**: `/dev/cu.usbserial-*` or `/dev/cu.SLAB_USBtoUART`
   
   You can also let PlatformIO auto-detect the port by commenting out or removing the `upload_port` line.

5. **Build and Upload**
   - Click the PlatformIO icon in the left sidebar
   - Click "Upload" under "PROJECT TASKS" → "esp32dev"
   - Or use the shortcut: Press Ctrl+Alt+U (Cmd+Alt+U on macOS)
   - Or use the command line:
     ```bash
     platformio run --target upload
     ```

6. **Monitor Serial Output (Optional)**
   - Click "Monitor" under "PROJECT TASKS" → "esp32dev"
   - Or press Ctrl+Alt+S (Cmd+Alt+S on macOS)
   - Or use the command line:
     ```bash
     platformio device monitor
     ```

## Method 2: Using esptool.py (For Pre-built Binaries)

This method is useful when you have a pre-built firmware binary (e.g., from GitHub releases).

### Installation

1. **Install Python** (if not already installed)
   - Download from [https://www.python.org/downloads/](https://www.python.org/downloads/)
   - Make sure to check "Add Python to PATH" during installation

2. **Install esptool**
   ```bash
   pip install esptool
   ```

### Downloading Firmware

1. **From GitHub Releases**
   - Visit [https://github.com/simonachmueller/ESP32Focuser/releases](https://github.com/simonachmueller/ESP32Focuser/releases)
   - Download the latest `firmware.bin` file

2. **From GitHub Actions Artifacts**
   - Go to [https://github.com/simonachmueller/ESP32Focuser/actions](https://github.com/simonachmueller/ESP32Focuser/actions)
   - Click on the latest successful workflow run
   - Download the `firmware.bin` artifact
   - Extract the zip file to get `firmware.bin`

### Uploading Firmware

1. **Connect Your ESP32**
   - Connect your ESP32 board via USB

2. **Find the Serial Port**
   - **Windows**: Check Device Manager → Ports (COM & LPT)
   - **Linux**: Run `ls /dev/ttyUSB*` or `ls /dev/ttyACM*`
   - **macOS**: Run `ls /dev/cu.*`

3. **Upload the Firmware**
   
   Replace `<PORT>` with your serial port (e.g., `COM3`, `/dev/ttyUSB0`, `/dev/cu.SLAB_USBtoUART`):
   
   ```bash
   esptool.py --chip esp32 --port <PORT> --baud 460800 write_flash -z 0x10000 firmware.bin
   ```

   Example for Windows:
   ```bash
   esptool.py --chip esp32 --port COM3 --baud 460800 write_flash -z 0x10000 firmware.bin
   ```

   Example for Linux/macOS:
   ```bash
   esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 460800 write_flash -z 0x10000 firmware.bin
   ```

4. **Wait for Upload to Complete**
   - The process typically takes 10-30 seconds
   - You'll see progress indicators during the upload

## Method 3: Using Arduino IDE (Alternative Method)

If you prefer Arduino IDE:

### Installation

1. **Install Arduino IDE**
   - Download from [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)

2. **Add ESP32 Board Support**
   - Open Arduino IDE
   - File → Preferences
   - Add to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Tools → Board → Boards Manager
   - Search for "esp32"
   - Install "esp32 by Espressif Systems"

3. **Install Required Libraries**
   - Tools → Manage Libraries
   - Search and install: "ESP32Encoder"

### Uploading Firmware

1. **Open the Project**
   - Open `src/main.cpp` in Arduino IDE

2. **Configure Board Settings**
   - Tools → Board → ESP32 Arduino → ESP32 Dev Module
   - Tools → Port → Select your ESP32's port
   - Tools → Upload Speed → 115200

3. **Upload**
   - Click the Upload button (→) or press Ctrl+U
   - Wait for the upload to complete

## Troubleshooting

### Device Not Detected

**Problem**: Computer doesn't recognize the ESP32 board.

**Solutions**:
- Install or reinstall the CP210x USB to UART bridge driver
- Try a different USB cable (some cables are power-only)
- Try a different USB port
- Press and hold the BOOT button on the ESP32 while connecting

### Upload Fails / Timeout Errors

**Problem**: Upload starts but fails with timeout or connection errors.

**Solutions**:
1. **Put ESP32 in Download Mode**:
   - Hold the BOOT button
   - Press and release the EN (reset) button
   - Release the BOOT button
   - Try uploading again

2. **Reduce Upload Speed**:
   - For PlatformIO, add to `platformio.ini`:
     ```ini
     upload_speed = 115200
     ```
   - For esptool, change `--baud 460800` to `--baud 115200`

3. **Check Port Access** (Linux/macOS):
   - Add your user to the dialout group:
     ```bash
     sudo usermod -a -G dialout $USER
     ```
   - Log out and log back in
   - Or use sudo: `sudo esptool.py ...`

### Wrong Port Selected

**Problem**: Cannot find the correct serial port.

**Solutions**:
- Disconnect the ESP32, note available ports
- Reconnect the ESP32, check which new port appears
- On Linux, check `dmesg | tail` after connecting
- On Windows, check Device Manager → Ports (COM & LPT)

### Permission Denied (Linux/macOS)

**Problem**: Permission errors when accessing the serial port.

**Solutions**:
```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER

# Or change port permissions temporarily
sudo chmod 666 /dev/ttyUSB0
```

### Out of Memory or Flash Errors

**Problem**: Flash operation fails with memory errors.

**Solutions**:
- Erase the flash completely first:
  ```bash
  esptool.py --chip esp32 --port <PORT> erase_flash
  ```
- Then try uploading again

## Verifying the Upload

After uploading, verify the firmware is running:

1. **Check Serial Output**
   - Open a serial monitor at 115200 baud
   - You should see output from the focuser
   
   Using PlatformIO:
   ```bash
   platformio device monitor -b 115200
   ```
   
   Using screen (Linux/macOS):
   ```bash
   screen /dev/ttyUSB0 115200
   ```

2. **Test Focuser Connection**
   - Follow the installation instructions in the main README.md
   - Connect using MoonLite software or ASCOM driver
   - Test basic focuser movements

## Additional Resources

- **PlatformIO Documentation**: [https://docs.platformio.org/](https://docs.platformio.org/)
- **ESP32 Documentation**: [https://docs.espressif.com/projects/esp-idf/en/latest/esp32/](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- **esptool Documentation**: [https://github.com/espressif/esptool](https://github.com/espressif/esptool)
- **Hardware Repository**: [https://github.com/simonachmueller/ESP32Focuser-hardware](https://github.com/simonachmueller/ESP32Focuser-hardware)

## Getting Help

If you encounter issues not covered in this guide:
- Check the [Issues](https://github.com/simonachmueller/ESP32Focuser/issues) page
- Open a new issue with details about your problem
- Include your operating system, board type, and error messages
