# pill-dispenser-project
This project implements a simple pill dispensing system using a Raspberry Pi Pico. 

# Goal
Our goal was to make a simple pill dispenser, Our pill dispenser can help elderly people to take their medicine on time

# Features
Pill detection via sensor input. LED indicators for system status (e.g. waiting for action, ready, error)

# Tools used
Raspberry Pi Pico, CMake, ARM GCC toolchain (arm-none-eabi-gcc), CLion IDE, Vscode IDE and GitHub

# Workflow
#Startup
-On power-up the system initializes GPIO pins and serial communication, LED indicators show system status: blinking while waiting for calibration, ON during work, 5 times blink in case of error

#Pill detection
A sensor monitors the pill compartment

#Dispense logic
Upon receiving a dispense command timed, the system activates a motor or actuator to release a pill, LED blinks during dispensing to indicate activity

#Errors
If a pill fails to dispense or sensor input is invalid, the system indicates an error, LED blinks 5 times to indicate the issue

#Flowchart
<img width="796" height="1714" alt="image" src="https://github.com/user-attachments/assets/9daa5a32-686d-49a2-854a-e92206239966" />

#Work divided
Lauri made core of code and validation, also gave input for documentation, Valtteri made documentation and tweaks in the code.

