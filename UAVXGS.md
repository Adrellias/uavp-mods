# UAVX Ground-station (UAVXGS) #

UAVXGS is a simple PC based ground-station GUI which is a intended as a base for other ground-station development. It is written in Visual C#.

# Instructions #

**Note Important Safety Precautions!!**

There is a simulator firmware that you can install and practice with the UAVXGS that is in the UAVX firmware download.  When you use the simulator firmware, the motors are disabled for safety.  I suggest you use this initially to check out your equipment.  If all is well then load the flight firmware and be very cautious and consider safety by tyeing down your model as the throttle will start the motors.

1.  Download and install the latest UAVXGS that is companion to your UAVX firmware from our Google Code download area.  Make sure that you have Microsoft.Net Framework 3.0 installed.  For Windows 7 users, make sure that you right-click on the UAVXGS setup.exe and select "Run as Administrator".

2.  Make sure that the Telemetry is set to UAVX in the UAVPset and written to the UAVX.

3. Use the same PC cable as for the UAVPset.

4. Set the same com port in the UAVXGS as you use for the UAVPset.  The UAVXGS should be defaulted and set to 9600bps to work with the UAVX while it is armed.

5. Close all other programs on you PC especially the UAVPset so the com port is free for the UAVXGS to use.

6. Power up your Tx, UAVX, and UAVXGS.

7. Click on the connect button on the UAVXGS.  The data should then start populating the fields.


This older version video is similar to what is now in use as an example.



wiki:video: cannot find YouTube video id within parameter "url".



# Rf data Link #
It is possible to also use the UAVXGS via a Rf data link.  There is a variety of Tx/Rx's that can be use, such as ZigBee or XBee, long range Bluetooth and FrSky Telemetry.  Set the baud rates for 9600bps and follow that data link equipment's setup instructions.

The UAVX can record the flight info for playback on your PC.  Choose the play button and load or save the file.

## Source Code ##

Source code is in the Downloads area.