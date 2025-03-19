# RPi Pico and CLion

Minimal CMake project for Raspberry Pi PicoW.

## Cloning the repository

To clone the repository using HTTPS, use the following command:

```
git clone https://gitlab.metropolia.fi/lansk/rpi-pico-minimal.git
```

This command pulls the example project from git in to directory rpi-pico-minimal. 
The directory can then be opened in CLion for editing/compiling the project. 

Follow the instructions for your operating system to setup CLion to build RPi Pico projects. 
Below you find instructions for Windows, Linux, and OSX (Mac).


## IDE Configuration - CLion - Windows

**Note: CLion has a long standing issue with OpenOCD and file paths that have spaces in them.  
To avoid any issues, clone the SDK and the requisite tools using a file path with no spaces in it.  
All embedded projects should also be created using file paths that have no spaces in them.  
This guide uses the file path "C:\pico" for this reason.**  
Should you want to use a different path (while still maintaining the no spaces policy), replace **"C:\pico"** with your own location.

Note: This guide assumes that you have installed [CLion](https://www.jetbrains.com/clion/download/#section=windows) and [Git for Windows](https://git-scm.com/downloads/win).

---

### Step 1: Installing the ARM GNU Toolchain

Download the ARM GNU Toolchain [here](https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-mingw-w64-i686-arm-none-eabi.exe).  
Run the installer and **make sure to tick the "Add path to environment variable" checkbox.** The default installation path is fine.

![Screenshot of "Add path to environment variable" checkbox](docs/images/toolchain-path-checkbox.png?raw=true)

CMake, the build generator of RPi Pico projects, need to know where the C/C++ compilef for RPi Pico is located.
The easiest way to pass this information to CMake is through environment variable "PICO_TOOLCHAIN_PATH".
On Windows the environment variables are set through "Edit environment variables for your account"

![Edit environment variables for your account](docs/images/Windows_edit_env.png?raw=true)

![Screenshot of environment variable editor with PICO_TOOLCHAIN_PATH defined](docs/images/windows_environment_1.png?raw=true)

---

### Step 2: Cloning the SDK

Clone the Pico SDK using the following terminal commands.  
All commands were tested using Windows Powershell.

```
mkdir c:/pico
cd c:/pico
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init
```

CMake, the build generator of RPi Pico projects, need to know where the SDK is located.
The easiest way to pass this information to CMake is through environment variable "PICO_SDK_PATH".
On Windows the environment variables are set through "Edit environment variables for your account"

![Screenshot of environment variable editor with PICO_SDK_PATH defined](docs/images/windows_environment_2.png?raw=true)

---

### Step 3: Installing OpenOCD

Download the OpenoOCD [here](https://github.com/raspberrypi/pico-sdk-tools/releases/download/v2.1.0-0/openocd-0.12.0+dev-x64-win.zip).  
Extract the files into **"C:\pico\openocd"**.

Using the terminal run the following command to edit one of the included script files:

```
notepad c:/pico/openocd/scripts/board/pico-debug.cfg
```

Edit the line `source [find target/rp2040-core0.cfg]` to match the following: `source [find target/rp2040.cfg]`.  
Save the file and close Notepad.

In order to get CLion to recognize OpenOCD correctly, the executable and it's associated .dll files have to be moved.  
Using the File Explorer, navigate to the OpenOCD installation directory (C:\pico\openocd).

**"C:\pico\openocd"** contents:

![Screenshot of openocd folder before changes](docs/images/openocd-folder-before.png?raw=true)

Create a new folder called **"bin"** and move openocd.exe and the two .dll files into this folder.

**"C:\pico\openocd"** and **"C:\pico\openocd\bin"** contents:

![Screenshot of openocd folder after changes](docs/images/openocd-folder-after.png?raw=true)
![Screenshot of openocd/bin folder](docs/images/openocd-bin-folder.png?raw=true)

---

### Step 4: Setting up CLion for building

Clone the rpi-pico-minimal repository to a file path without spaces in it using the cloning instructions at the top of the page.

Finally, launch CLion, and open the folder containing the cloned repository.  
On the **Open Project Wizard** window, check the box labeled **"Reload CMake project on editing..."** and in the box labeled **"Environment"** add the environment variable `PICO_SDK_PATH=C:\pico\pico-sdk`.

![Screenshot of CLion Open Project Wizard window](docs/images/new-project-settings.jpg?raw=true)

CMake should now configure the project (if there is an warning about picotool, it can be ignored safely).

After successful configuration, the output should look like this:

![Screenshot of CMake output after successful configuration](docs/images/cmake-configure-done.png?raw=true)

It should now be possible to build the project using the **"Build"** button at the top left corner of the window.

![Screenshot of CLion "Build" button](docs/images/clion-build.png?raw=true)

After the build finishes, the output should look something like this:

![Screenshot of the output from a successful build](docs/images/cmake-build-success.png?raw=true)

---

### Step 5: Setting up CLion for programming/debugging

First, wire up the Raspberry Pi Debug Probe according to the instructions from [here](https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html).  
Note that on the Pico W, the JST header is in a slightly different location.  
**Take extra care when connecting the small JST headers, using excessive force can cause damage to the headers.**

At the top right corner of the CLion interace, click the menu button and find and click the **"Settings..."** option.

![Screenshot of CLion "Menu" button](docs/images/clion-menu.png?raw=true)
![Screenshot of CLion "File" menu with "Settings" option marked](docs/images/clion-menu-settings.png?raw=true)

Under **"Build, Execution, Deployment"** select the **"Embedded Development"** option.  
Set the OpenOCD executable location here and click on the **"Test"** button.  
The output popup should contain some version information about OpenOCD and the background should be green.

![Screenshot of a correctly set OpenOCD location](docs/images/clion-openocd-location.png?raw=true)

Under **"Embedded Development"**, select the **"RTOS Integration"** option.  
Check the **"Enable RTOS Integration"** and select **"FreeRTOS"** from the dropdown.

![Screenshot of "RTOS Integration" window](docs/images/clion-rtos-integration.png?raw=true)

Click **"OK"** to save the settings and close the window.

At the top left corner of the CLion interface, click on the project name, and then the **"Edit Configurations..."** option.

![Screenshot of CLion project "Edit configurations" option](docs/images/clion-config-edit.png?raw=true)

At the top right corner of the **"Run/Debug Configurations"** window, click the **"+"**-sign and then scroll down to find and click the **"OpenOCD Download & Run"** option.

![Screenshot of CLion "Run/Debug Configurations" with the **"OpenOCD Download & Run"** option](docs/images/clion-openocd-config.png?raw=true)

On the new window, click the **"Assist..."** button for the **"Board config file"** setting.

![Screenshot of CLion "OpenOCD Download & Run" settings page](docs/images/clion-openocd-config-debug-board.png?raw=true)

Start typing to search for **"pico-debug"**, find and select the **"pico-debug.cfg"** and click **"Use"**.

![Screenshot of CLion "Board config](docs/images/clion-openocd-config-debug-board-search.png?raw=true)

Configure **"Executable binary"** and the **"Download"** options to match the figure below.

![Screenshot of CLion "Board config](docs/images/clion-openocd-config-complete.png?raw=true)

Click **"OK"** to save and close the window.

Connect the USB cables from **both** the Pico and the Debug Probe to the computer.

Clicking the **"Run"** button at the top left corner of the CLion interface should now build and program the device.

![Screenshot of CLion "Run" button](docs/images/clion-run.png?raw=true)

Output from a successful programming (look for the line that reads **"Programming Finished"**):

![Screenshot of the output from a successful programming](docs/images/openocd-program-success.png?raw=true)

The program should now blink the LED on the Pico.

---

### Step 6: Setting up CLion Serial Monitor

The Pico Debug Probe sends console prints (printf statements etc.) through a serial connection. To allow printing commands through CLion, the Serial Monitor plugin should be installed.

Start by opening the settings window like at the start of step 5 of this guide.
Under the settings, find and click the **"Plugins"** option.  
Search for "serial" and install the **"Serial Port Monitor"** plugin.

![Screenshot of CLion "Settings" window with the "Plugins" option selected and "Serial Port Monitor" selected.](docs/images/clion-serial-monitor-install.png?raw=true)

After installation click **"OK"** to save and close the settings.

Find the **"Serial Port Monitor"** button at the bottom left corner of the interface.

![Screenshot of CLion "Serial Port Monitor" button.](docs/images/clion-serial-monitor-button.png?raw=true)

Select the COM port and make sure to match the connection settings before clicking **"Connect"**.  
**Note: The correct COM port is system specific and as such, is likely to be different that the screenshots. If there are multiple COM ports available, try each one until you get a connection and start seeing messages.**

![Screenshot of CLion "Serial Port Monitor" connection settings.](docs/images/clion-serial-monitor-connect-settings.png?raw=true)

With **both** the Pico and the Debug Probe connected, the program should now blink the LED on the Pico and print "Blink" on the console.

![Screenshot of CLion "Serial Port Monitor" showing terminal output](docs/images/clion-serial-monitor-output.png?raw=true)

If the device outputs HEX numbers, disable **"HEX view"** by clicking the following button:

![Screenshot of CLion "Serial Port Monitor" with the HEX view button highlighted](docs/images/clion-serial-monitor-hex-view.png?raw=true)

If there is no output, double check the wiring of the Debug Probe and if there are multiple COM ports available, try each one.

## IDE Configuration - CLion - Linux

On Ubuntu 16.04 or later, you can install CLion as a snap package using the following command:

```
sudo snap install clion --classic
```

On other distros, you can download CLion [here](https://www.jetbrains.com/clion/download/#section=linux).

**Note: These instructions have been tested on Ubuntu 22.04 LTS and as such, commands and requisite package names might be different on other distros.**

**Note: CLion has a long standing issue with OpenOCD and file paths that have spaces in them.  
To avoid any issues, clone the SDK and the requisite tools using a file path with no spaces in it.  
All embedded projects should also be created using file paths that have no spaces in them.  
This guide uses the file path "/home/\<user>/pico (or ~/pico)" for this reason.**  
Should you want to use a different path (while still maintaining the no spaces policy), replace **"~/pico"** with your own location.

### Step 1: Installing prerequisites

Before starting the installation, it is recommended to update all the package repositories and upgrade all installed packages.  
This can be done using the following commands:

```
sudo apt update
sudo apt upgrade
```

To install all the prerequisite software, run the following command:

```
sudo apt install git python3 build-essential gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib libtool pkg-config libusb-1.0-0-dev
```

---

### Step 2: Cloning the SDK

Clone the Pico SDK using the following terminal commands.  

```
mkdir ~/pico
cd ~/pico
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init
```

---

### Step 3: Installing OpenOCD

On Linux, the simplest way to install OpenOCD is to build it from sources and install it.  
This can be done using the following commands:

```
cd ~/pico
git clone https://github.com/raspberrypi/openocd.git
cd openocd
./bootstrap
./configure --disable-werror
make
sudo make install
```

To allow OpenOCD to be called without sudo, run the following commands to update the udev rules:

```
sudo cp ~/pico/openocd/contrib/60-openocd.rules /etc/udev/rules.d/
sudo udevadm control --reload
sudo udevadm trigger
```

After the installation, run the following command to fix a small error in one of the config files:

```
sudo sed -i "s|rp2040-core0|rp2040|g" "/usr/local/share/openocd/scripts/board/pico-debug.cfg"
```

---

### Step 4: Setting up CLion for building

Clone the repository to a file path without spaces in it using the cloning instructions at the top of the page.

Finally, launch CLion, and open the folder containing the cloned repository.  
On the **Open Project Wizard** window, check the box labeled **"Reload CMake project on editing..."** and in the box labeled **"Environment"** add the environment variable `PICO_SDK_PATH=~\pico\pico-sdk`.

**Note: Screenshot is from the Windows version of this guide, make sure to add the correct path from above.**
![Screenshot of CLion Open Project Wizard window](docs/images/new-project-settings.jpg?raw=true)

CMake should now configure the project (if there is an warning about picotool, it can be ignored safely).

After successful configuration, the output should look like this:

**Note: Screenshot is from the Windows version of this guide, but the output should be the same apart from the file path.**
![Screenshot of CMake output after successful configuration](docs/images/cmake-configure-done.png?raw=true)

It should now be possible to build the project using the **"Build"** button at the top left corner of the window.

![Screenshot of CLion "Build" button](docs/images/clion-build.png?raw=true)

After the build finishes, the output should look something like this:

![Screenshot of the output from a successful build](docs/images/cmake-build-success.png?raw=true)

---

### Step 5: Setting up CLion for programming/debugging

First, wire up the Raspberry Pi Debug Probe according to the instructions from [here](https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html).  
Note that on the Pico W, the JST header is in a slightly different location.  
**Take extra care when connecting the small JST headers, using excessive force can cause damage to the headers.**

At the top right corner of the CLion interace, click the menu button and find and click the **"Settings..."** option.

![Screenshot of CLion "Menu" button](docs/images/clion-menu.png?raw=true)
![Screenshot of CLion "File" menu with "Settings" option marked](docs/images/clion-menu-settings.png?raw=true)

Under **"Embedded Development"**, select the **"RTOS Integration"** option.  
Check the **"Enable RTOS Integration"** and select **"FreeRTOS"** from the dropdown.

![Screenshot of "RTOS Integration" window](docs/images/clion-rtos-integration.png?raw=true)

Click **"OK"** to save the settings and close the window.

At the top left corner of the CLion interface, click on the project name, and then the **"Edit Configurations..."** option.

![Screenshot of CLion project "Edit configurations" option](docs/images/clion-config-edit.png?raw=true)

At the top right corner of the **"Run/Debug Configurations"** window, click the **"+"**-sign and then scroll down to find and click the **"OpenOCD Download & Run"** option.

![Screenshot of CLion "Run/Debug Configurations" with the **"OpenOCD Download & Run"** option](docs/images/clion-openocd-config.png?raw=true)

On the new window, click the **"Assist..."** button for the **"Board config file"** setting.

![Screenshot of CLion "OpenOCD Download & Run" settings page](docs/images/clion-openocd-config-debug-board.png?raw=true)

Start typing to search for **"pico-debug"**, find and select the **"pico-debug.cfg"** and click **"Use"**.

![Screenshot of CLion "Board config](docs/images/clion-openocd-config-debug-board-search.png?raw=true)

Configure **"Executable binary"** and the **"Download"** options to match the figure below.

![Screenshot of CLion "Board config](docs/images/clion-openocd-config-complete.png?raw=true)

Click **"OK"** to save and close the window.

Connect the USB cables from **both** the Pico and the Debug Probe to the computer.

Clicking the **"Run"** button at the top left corner of the CLion interface should now build and program the device.

![Screenshot of CLion "Run" button](docs/images/clion-run.png?raw=true)

Output from a successful programming (look for the line that reads **"Programming Finished"**):

![Screenshot of the output from a successful programming](docs/images/openocd-program-success.png?raw=true)

The program should now blink the LED on the Pico.

---

### Step 6: Setting up CLion Serial Monitor

The Pico Debug Probe sends console prints (printf statements etc.) through a serial connection. To allow printing commands through CLion, the Serial Monitor plugin should be installed.

**Note: On Ubuntu, access to serial devices is limited to users in the **"dialout"** group. To allow CLion to access the serial port, use the following command to add the user to the group:**

```
sudo usermod -aG dialout $USER
```

**To make sure the group is applied correctly, reboot the system.**

Start by opening the settings window like at the start of step 5 of this guide.
Under the settings, find and click the **"Plugins"** option.  
Search for "serial" and install the **"Serial Port Monitor"** plugin.

![Screenshot of CLion "Settings" window with the "Plugins" option selected and "Serial Port Monitor" selected.](docs/images/clion-serial-monitor-install.png?raw=true)

After installation click **"OK"** to save and close the settings.

Find the **"Serial Port Monitor"** button at the bottom left corner of the interface.

![Screenshot of CLion "Serial Port Monitor" button.](docs/images/clion-serial-monitor-button.png?raw=true)

Select the serial device and make sure to match the connection settings before clicking **"Connect"**.  
**Note: The screenshots below are from the Windows version of CLion, on Linux the serial devices are named /dev/ttyACM0 (or /dev/ttyACM1 etc.). Everything else should work the same.**

![Screenshot of CLion "Serial Port Monitor" connection settings.](docs/images/clion-serial-monitor-connect-settings.png?raw=true)

With **both** the Pico and the Debug Probe connected, the program should now blink the LED on the Pico and print "Blink" on the console.

![Screenshot of CLion "Serial Port Monitor" showing terminal output](docs/images/clion-serial-monitor-output.png?raw=true)

If the device outputs HEX numbers, disable **"HEX view"** by clicking the following button:

![Screenshot of CLion "Serial Port Monitor" with the HEX view button highlighted](docs/images/clion-serial-monitor-hex-view.png?raw=true)

**Note: The debug probe also launches it's own serial device, so there should be two devices available (/dev/ttyACM0 and /dev/ttyACM1 for instance), try each to see which one is correct.**

If there is no output, double check the wiring of the Debug Probe.

## IDE Configuration - CLion - Mac

### 1) Install Homebrew(If not installed already) and if you have an M1 or M2 mac install Rosetta

``` 
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)" 
/usr/sbin/softwareupdate --install-rosetta --agree-to-license
```

### 2) Install CMake

```
brew install cmake
```

### 3) Install cross compilation toolchain

```
brew install --cask gcc-arm-embedded
```

Now the tools will be installed in "PICO_TOOLCHAIN_PATH=/Applications/ArmGNUToolchain/13.2.rel1/arm-none-eabi/bin".
Note that the path depends on the version of the toolchain that gets installed. Check the version after
installling the toolchain to get the correct path.

### 4)  Install openocd

```
brew install openocd
```

Getting the SDK and Examples

```
cd ~/
mkdir pico
cd pico

git clone https://github.com/raspberrypi/pico-sdk.git --branch master
cd pico-sdk
git submodule update --init
```
Now your SDK is located where you have cloned it. You can get the path by running:

```
pwd
```

Then clone the examples.

```
cd ..
git clone https://github.com/raspberrypi/pico-examples.git --branch master
```
place the following lines into ~/.bash_profile with the correct paths.
If you use zshell place the lines in ~/.zprofile

Note that the paths below must be changed to the paths used on your computer.

export PICO_SDK_PATH="\<Path to where you cloned the pico-sdk>"

export PICO_TOOLCHAIN_PATH="/Applications/ArmGNUToolchain/13.2.rel1/arm-none-eabi/bin"


If you have issues with arm-gcc you may have to resinstall arm-gcc
Fixing the arm-none-eabi-gcc issue

```
brew uninstall arm-none-eabi-gcc
brew autoremove
```

Go to step 3)


## IDE Configuration - VSCode - Windows/Mac/Linux

To be added if necessary.
