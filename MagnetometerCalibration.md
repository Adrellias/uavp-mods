## Magnetometer Calibration ##

The Magnetometer is significantly more complicated to calibrate than the simple compass but it does offer adjustment for flight tilt angles. For the 2 axis compass simply doing a 360 degree rotation is enough. The magnetometer however measures the Earth's magnetic field in 3 axes and so needs to be rotated in "all" directions to determine the maximum and minimum magnetic field strengths. From these we can work out the offset from zero corresponding to no field and thence through some math the direction of of the magnetic north pole which is in fact downwards through the Earth. From there we can determine the direction of Magnetic North.


### Procedure ###

For the PIC version of UAVX you must first load the TEST version of firmware. UAVXArm has all of the tests available in the flight version.

Run the calibration test under the Tools pulldown in UAVPSet. Rotate the aircraft in all the directions you can think of including upsided down and on edge. This captures the initial maximum and minimum values of the X,Y and Z axes. You do not need to re-run the calibration unless it is clearly giving crazy answers for the computed compass heading. The maximum/minimum values are stored in EEPROM/FLASH where they are retained after disconnecting the battery.

The maximum/minimum values are also tracked continously when the aircraft is powered including in flight when the motors are running. Unlike the calibration test they are only written to EEPROM/FLASH when you **disarm**.

The compass heading estimate should gradually improve as the max/min values stabilise.

In UAVXArm the magnetometer values are "fused" with the gyro and accelerometer measurements to yield better heading estimates even if the magnetometer is not fully calibrated.