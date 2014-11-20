# RTIMULib - a versatile 9-dof IMU library for embedded Linux systems

RTIMULib is the simplest way to connect a 9-dof IMU to an embedded Linux system and obtain Kalman-filtered quaternion or Euler angle pose data. Basically, two simple funtion calls (IMUInit() and IMURead()) are pretty much all that's need to integrate RTIMULib.

Two demo apps are included - RTIMULibDemo is a GUI-based program that shows all the data being produced and also support compass calibration. RTIMULibDrive is just about the most basic program possible and can be used for performance testing filters and drivers. It can also be used as the basis of a real application quite easily.

RTIMULibCal is a stand-alone, command line program that can be used to generate calibration data in non-GUI environments. See the RTIMULibCal readme for more details.

In addition, there are two apps (RTHostIMU and RTHostIMUGL) that allow the sensor fusion to be separated from the sensor interfacing and collection. An Arduino (running the RTArduLinkIMU sketch from the RTIMULib-Arduino repo) fitted with an IMU chip collects the sensor data and sends it to the host. RTHostIMU and RTHostIMUGL (this one has an OpenGL visualization of the data) communicate with the Arduino via a USB connection.

Its prerequisites are very simple - just I2C support on the target system along with the standard build-essential (included in the Raspberry Pi Raspbian distribution by default).

RTIMULib provides a flexible framework for interfacing 9-dof IMUs to embedded Linux systems. RTIMULib currently supports the InvenSense MPU9150 single chip IMU, STM LSM9DS0 single chip IMU, Pololu Altimu and Adafruit 9-dof IMU. RTIMULib also supports multiple sensor integration fusion filters such as Kalman filters.

The instructions here are for the Raspberry Pi but RTIMULib can be use easily with other embedded systems with minor (or no changes). An abstraction layer allows RTIMULib to be used with non-Linux systems also.

Check out www.richards-tech.com for more details, updates and news.

The MPU-9250 and SPI driver code is based on code generously supplied by staslock@gmail.com (www.clickdrive.io). I am sure that any bugs that may exist are due to my integration efforts and not the quality of the supplied code!

RTIMULib is licensed under the MIT license.

## Note about magnetometer (compass) calibration

It is essential to calibrate the magnetometer or else very poor fusion results will be obtained. For more about this, see http://wp.me/p4qcHg-b4. RTIMULibDemo (GUI) and RTIMULibCal (command line) can be used to do this. They both support magnetometer min/max, magnetometer ellipsoid fit and accelerometer min/max calibration.

## Release history

### November 18 2014 - 4.2.0

Add the IMU axis rotation capability to better handle IMUs in non-standard orientations. See http://wp.me/p4qcHg-cO for more details of how to use this capability.

### November 14 2014 - 4.1.0

Corrected some problems with the continuous gyro bias update system. There is a new function in RTIMU called setGyroContinuousLearningAlpha() that allows the continuous learning rate to be set. RTIMULIB uses a rapid learning rate to collect the initial gyro bias data but then uses a much slower rate for continuous tracking of bias. This function allows the rate to be set if necessary - values can be between 0.0 and 1.0. Setting it to 0.0 turns off continuous learning completely so that gyro bias calculation only occurs during the rapid learning period.

loadSettings() and saveSettings() in RTSettings.cpp are now virtual to give more flexibility in how settings are stored.

### November 8 2014 - 4.0.1

Fixed some missing MPU-9250 related defs in python interface.

### November 7 2014 - 4.0.0

Restructured library to add support for the MPU-9250 and SPI bus. This is a little experimental right now - use V3.1.1 if problems are encountered with existing supported IMUs. The MPU-9250 has been seen to hang when used on the SPI bus at sample rates above 300 samples per second. However, sample rates up to 1000 seem to work fine using I2C.

The RTIMULib apps are now able to auto-detect on the I2C and SPI bus so, if only one supported IMU is present on either bus, the code should find it. Note that only the MPU-9250 is supported by the SPI driver at the moment. There are some new settings in the RTIMULib.ini file related to the SPI bus that may need editing in some systems. The default SPI bus is set 0 which works nicely for the Raspberry Pi. Connect the MPU-9250 to SPI0 and CS0 and it should work without needing to change anythin in RTIMULib.ini.

### November 4 2014 - 3.1.1

Can now supply the .ini name as a command line argument. For example:

    RTIMULibCal Palm
    
would calibrate a settings file called Palm.ini.

### November 1 2014 - 3.1.0

Added the RTIMULibCal application. This implements IMU calibration in no GUI (command line) environments.

### October 13 2014 - 3.0.3

Increased time allowed for ellipse fitting to complete before declaring it finished.

### October 13 2014 - 3.0.2

Added license information to Calibrate.pdf.

### October 13 2014 - 3.0.1

Fixed missing license header in RTEllipsoidFit.

### October 13 2014 - 3.0.0

RTIMULib now support accelerometer calibration and enhanced magnetometer calibration using ellipsoid fitting. Please check the Calibration.pdf document for instructions on how to create the calibration data.

### October 8 2014 - 2.1.2

Fixed some missed license header changes.

### October 8 2014 - 2.1.1

Fixed bug where the first com put was missed on the GUI dropdown in RTHostIMU and RTHostIMUGL.

### October 8 2014 - 2.1.0

Changed license to MIT.

Added RTHostIMU and RTHostIMUGL. These apps use RTArduLink to connect the host system to an IMU connected to an Arduino. This allows processing to be split between the Arduino and the host system. Sensor data collection is performed on the Arduino, sensor fusion and display is performed on the host. This means that the apps will run on hosts without I2C ports (such as PCs). See below for more details.

### October 2 2014 - 2.0.0

Changed the gyro bias calculation to run automatically when the IMU is detected as being stable. This means
that the IMU no longer needs to be kept still for 5 seconds after restart and gyro bias is continually tracked. IMUGyroBiasValid can be called to check if enough stable samples have been obtained for a reasonable bias calculation to be made. If the IMU is stable, this will normally occur within 5 seconds. If not stable at the start, it may take longer but it will occur eventually once enough stable samples have been obtained. If RTIMULibDemo never indicates a valid bias, the #defines RTIMU_FUZZY_GYRO_ZERO and/or RTIMU_FUZZY_ACCEL_ZERO may need to be increased if the gyro bias or accelerometer noise is unusually high. These should be set to be greater than the readings observed using RTIMULibDemo when the IMU is completely stable. In the case of the gyros, this should be the absolute values when the IMU isn't being moved. In the case of the accels, this should be the maximum change in values when the IMU isn't being moved.

Stable gyro bias values are saved to the RTIMULib.ini file in order to speed up restarts. The values will once again be 
updated after enough stable samples have been obtained in order to track long term changes in gyro bias.

If problems are encountered, try version 1.0.4 which is available under the GitHub repo releases tab. Please also report any issues via the GitHub issue system to help improve RTIMULib!

### September 3 2014 - 1.0.4

Fixed message error in RTIMUSettings.

### September 2 2014 - 1.0.3

CompassCalMax was returning m_compassCalMin in PyRTIMU_settings.cpp - changed to max instead. Thanks to Stefan Grufman for finding that.

### August 6 2014 - 1.0.2

Added missing compass sample rate defines for LSM303DLHC and updated settings comments. Thanks to Bill Robertson (broberts4) for spotting that!

### July 29 2014 - 1.0.1

Fixed the python getIMUData function.

### July 7 2014 - 1.0.0

#### Added python bindings

Thanks to avishorp for the python code and Luke Heidelberger for a bug fix.

### April 13 2014 - 0.9.4

#### Added new RTQF fusion filter

RTQF is a very highly stripped down Kalman filter that avoids matrix inversion and lot of other matrix operations. It makes a small performance difference on the Raspberry Pi but would have more impact on lower powered processors.

### April 10 2014 - 0.9.3

#### STM LSM9DS0 IMU Implementation now working

The single chip IMU LSM9DS0 is now working with RTIMULib. An example breakout is available from Sparkfun - https://www.sparkfun.com/products/12636.

### April 9 2014 - 0.9.2

#### STM L3GD20H + LSM303D IMU Implementation now working

The combination of the L3GD20H gyro and LSM303D accel/mag chip is now working. The physical configuration supported is as used on the Pololu Altimu V3 - http://www.pololu.com/product/2469. The pressure chip on the 10-dof version will be supported shortly but 9-dof is working now.

#### STM L3GD20 + LSM303DLHC IMU Implementation now working

The combination of the L3GD20 and LSM303DLHC accel/mag chip is now working. The physical configuration supported is as used on the Adafruit 9-dof IMU - http://www.adafruit.com/products/1714.

### April 7 2014 - 0.9.1

#### Improved performance with MPU-9150

A new caching strategy for the MPU-9150 seems to be achieving 1000 samples per second without fifo overflows using a 900MHz Raspberry Pi and 400kHz I2C bus. This is as reported by RTIMULibDrive with a CPU utilization of 28%. RTIMULibDemo manages 890 samples per second with the MPU-9150 set to 1000 samples per second. The driver gracefully handles this situation although there is increased delay when the application cannot handle the full sample rate.

#### Auto detection of IMU

RTIMULib can now scan for supported IMUs and configure automatically. This is the default behavior now. It handles IMUs at alternate address automatically as well (for example, it will detect an MPU-9150 at 0x68 or 0x69).

#### Partial support for STM L3GD20H/LSM303D IMUs

This is in a very early state and only supports the gyro sensor at the moment.

### April 4 2014 - 0.9.0

Initial release with support for MPU9150.

## RTIMULib on the Raspberry Pi/Raspbian

### Obtaining RTIMULib

From a command prompt, enter:

	git clone https://github.com/richards-tech/RTIMULib.git

### Setting up the Raspberry Pi

#### Connecting the IMU

The easiest way to connect the IMU to the Raspberry Pi is to use something like the Adafruit Pi Plate (http://www.adafruit.com/products/801) as it makes it obvious where the I2C bus 1 pins are and where to pick up 3.3V. Basically, you need to connect the I2C SDA, I2C SCL, 3.3V and GND to the IMU breakout board you are using. Take care with these connections or else disaster may follow!

#### Enabling and Configuring the I2C Bus

Add the following two lines to /etc/modules:

	i2c-bcm2708
	i2c-dev

Then, comment out the following line in /etc/modprobe.d/raspi-blacklist.conf:

	# blacklist i2c-bcm2708

Restart the Raspberry Pi and /dev/i2c-0 and /dev/i2c-1 should appear. It’s also useful to install the I2C tools:

	sudo apt-get install i2c-tools

Then:

	sudo i2cdetect -y 1

will detect any devices on /dev/i2c-1. If you have the MPU9150 wired up, you should see it at address 0x68. This is the default address expected by the demo programs. If it is at 0x69, the address expected by the demo programs will need to be changed (there's a settings file for doing things like that so it's easy to do).

By default, the I2C devices are owned by root. To fix this, reate a file /etc/udev/rules.d/90-i2c.rules and add the line:

	KERNEL=="i2c-[0-7]",MODE="0666"

The Raspberry Pi will need to be rebooted to implement this change.

Another thing worth doing is to change the I2C bus speed to 400KHz. At the standard 100KHz, the sample rate tops out at around 230 samples per second. With the change, it should be possible to get over 400 samples per second.

To enable 400kHz operation, create a file /etc/modprobe.d/i2c.conf and add the line:

	options i2c_bcm2708 baudrate=400000

Simplest thing is then to reboot to make this change.

The I2C bus should now be ready for operation.

### Compile and Run the RTIMULibDrive Demo Program

RTIMULibDrive is a simple command line program that shows how simple it is to use RTIMULib. Assuming the I2C bus has been enabled, just go to the RTIMULibDrive directory and enter:

	make
	sudo make install

You should be able to run the program just by entering RTIMULibDrive. It will try to auto detect the connected IMU If all is well, you should see a line showing the sample rate and the current Euler angles. This is updated 10 times per second, regardless of the sensor sample rate. By default, the driver runs at 50 samples per second in most cases. So, you should see the sample rate indicating around 50 samples per second. The sample rate can be changed by editing the .ini file entry for the appropriate IMU type.

The displayed pose shows the roll, pitch and yaw seen by the IMU. Using an aircraft analogy, the roll axis points from the pilot towards the nose, the pitch axis points from the pilot along the right wing and the yaw axis points from the pilot down towards the ground. Right wing down is a positive roll, nose up is a positive pitch and clockwise rotation is a positive yaw.

Various parameters can be changed by editing the RTIMULib.ini file. These are described later.

Take a look at RTIMULibDrive.cpp. Quite a few of the code lines are just to calculate rates and display outputs!

One thing you may notice is that the yaw isn't too accurate, especially at non zero pitch and roll. This is because the compass has not been calibrated. RTIMULibDemo can be used to do that.

###Compile and Run the RTIMULibDemo Program

RTIMULibDemo is a Qt-based program (Qt is used to supply the GUI). So, do the following:

	sudo apt-get install libqt4-dev

This will install the libraries and utilities that Qt needs. If compass ellipsoid fitting is desired, also install Octave:

	sudo apt-get install liboctave-dev

Then, go to the RTIMULibDemo directory and enter:

	qmake
	make
	sudo make install

To run the program, the Raspberry Pi needs to be running the desktop. To do this (if it isn't already), enter:

	startx

Then open a command window and enter:

	RTIMULibDemo

You should see the GUI pop up and, if everything is ok, it will start displaying data from the IMU and the output of the Kalman filter. If the MPU9150 is at the alternate address, you'll need to edit the RTIMULib.ini file that RTIMULibDemo generated and restart the program.

To calibrate the compass, click on the "Calibrate compass" tab. A new dialog will pop up showing the maximum and minimum readings seen from the magnetometers. You need to waggle the IMU around, ensuring that each axis (roll, pitch and yaw) point straight down and also straight up at some point. You need to do this in an area clear of magnetic fields otherwise the results will be distorted. Eventually, the readings will stop changing meaning that the real max and min values have been obtained. Click on "Ok" to save the values to the RTIMULib.ini file. Provided this .ini file is used in future (it just has to be in the current directory when RTIMULibDemo is run), the calibration will not have to be repeated. Now that RTIMULibDemo is using calibrated magnetometers, the yaw should be much more reliable.

The .ini file created by RTIMULibDemo can also be used by RTIMULibDrive - just run RTIMULibDrive in the same directory and it will pick up the compass calibration data.

## Gyro bias compensation

Prior to Version 2, gyro bias is calculated during the first 5 seconds of operation. If the IMU chip is moved during this period, the bias may be calculated incorrectly and the app will need to be restarted. The effectiveness of the gyro bias compensation can be monitored using RTIMULibDemo. If the IMU chip is not in motion, the gyro rates should be close to 0 (usually around 0.001 radians per second).

For version 2 and above, the gyro bias is continuously updated when the IMU is dteected as being stable. See the V2.0.0 release
note (above) for more information.

## .ini File Settings

By default, RTIMULib.ini is created in the working directory. This can be easily edited and the file contains extensive comments for the settings of every supported IMU type.

## The RTIMULib Hardware Abstraction Layer

All of the platform-specific code is in the following files:

	RTIMULib/RTIMUHal.h
	RTIMULib/RTIMUHal.cpp
	RTIMULib/RTIMUSettings.h
	RTIMULib/RTIMUSettings.cpp

Changes to I2C code on Linux systems should only involve changes to RTIMUHal. RTIMUSettings uses the Linux file system so, if this is not present, this code would also have to modified to load and store configuration data from somewhere else.

RTIMUSettings would also need to be modified if new IMU drivers and filters are added to the library.

## RTHostIMU and RTHostIMUGL

These apps are built in the same way as RTIMULibDemo. The assumption is that an Arduino running RTArduLinkIMU is connected to one of the host's USB ports. When the apps are started, no com port is configured. The com port dropdown should be used to select the correct port for the connected Arduino. Once this is done, data should start being received and the displays activated.

*** Important note: RTHostIMUGL will not work on the Raspberry Pi as Qt does not support OpenGL on that platform at the moment. Use RTHostIMU instead. RTHostIMUGL has been tested on Ubuntu 14.04 PCs and Windows 7 PCs.

## Next Steps

SyntroPiNav (an app for the Raspberry Pi) and SyntroNavView can be used as a convenient system to experiment with IMU chips, drivers and filters. SyntroPiNav runs on the Pi and transmits IMU data along with filter outputs over a LAN to SyntroNavView running on an Ubuntu PC. SyntroNavView also displays the data and provides a 3D graphics view of the calculated pose.

Since all IMU data is sent to SyntroNavView, SyntroNavView can run its own local filter. This makes it a very convenient testbed for new filter development as the speed of the desktop can be used to accelerate implementation and testing. When ready, the updated RTIMULib can be compiled into SyntroPiNav and should work exactly the same as on the desktop.

SyntroPiNav is available as part of the richards-tech SyntroPiApps repo (https://github.com/richards-tech/SyntroPiApps) while SyntroNavView is available as part of the richards-tech SyntroApps repo (https://github.com/richards-tech/SyntroApps).




