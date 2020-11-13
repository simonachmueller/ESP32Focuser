# ESP32Focuser
Build status ![PlatformIO CI](https://github.com/semenmiroshnichenko/ESP32Focuser/workflows/PlatformIO%20CI/badge.svg)

Focuser for astronomy telescope based on ESP32 controller. Comportable with Indilib (http://indilib.org/) and ASCOM (https://ascom-standards.org/).
The controller uses the protocol Moonlite (as documented in the indilib project).

This is a port of an awesome code from https://github.com/Hansastro/Focuser to ESP32 platform with some additions:
1. It uses TMC2209 stepper motor driver (because it's cool and really silent).
1. It uses an optical rotary encoder connected to pins 2 and 15 as a handcontroller, which helps manually focus your telescope on a not-really-stable mounts. The encoder focusing control feels very direct and highly resolved (because it has about 1:5 reduction). 

# Installation for computer-controlled use
## Windows
1. Install CP210x driver from https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers if not installed automatically by windows itself
2. From here https://focuser.com/downloads.php install MoonLite Single Focuser if you wan to use the focuser in the manual mode or test it, or Universal ASCOM driver if you want to access the focuser from other astronomical tools like SharpCap
