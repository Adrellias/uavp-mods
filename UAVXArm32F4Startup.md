## Introduction ##

This is the evolving setup page for the Arm version of UAVX. We will try to keep explanations concise. If you have flown UAVX then most of this is familiar as we have deliberately scaled the parameters and kept the look and feel of UAVPSet and UAVXGS to those you are familiar with so you can concentrate on flying.

UAVXStartup contains a wealth of information which you could take the time to read. You should pay particular attention to what the different patterns of LED behaviour mean (Lights and Sirens).

UAVXArm32F4 supports many different airframe types from octocopters, through helis out to conventional aircraft. UAVX has done this for several years. Most of the multicopter configurations are shown in UAVXStartup. If you are flying a fixed wing aircraft you should read the UAVXArm32F4FixedWingStartup Wiki as well.

If you are using UAVXArm32F4 board without sensors as an adapter for the old UAVP board you should read the associated Wiki. If you are doing this then you will be quite familiar with UAVX/UAVP but it is still worth reading the notes below.

#### UAVXArm32F4 Board ####

![http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVXArm32F4Connections.png](http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVXArm32F4Connections.png)

### Step 1 ###

Mount the board on your favourite frame oriented in the direction of forward flight. So if you are flying +Mode point the board along the K1 motor arm. If you are flying XMode point the board between the K1 and K3 motor arms. Select the desired configuration in UAVPSet using the airframe pulldown.

Take all the the shorting links (if any) off the board. Connect up an arming switch between the Arming pin and the adjacent Ground pin.

The motors should not run even if you have the arming switch on until you get through all of the following steps but think about a Kw of motors all running at around 7000RPM flying around your workshop - take the props OFF.

The board can be connected directly to your main battery (3S LiPo).

The Red light should be on and the Yellow LED should be flashing once a second indicating that you have not yet calibrated your accelerometers.

### Step 2 ###

Download the software package from the downloads area and install UAVPset and UAVXGS.

Connect the adapter lead to the board and then to a USB port on your computer.

![http://uavp-mods.googlecode.com/svn/branches/uavxarm32_graphics/ftdi.jpg](http://uavp-mods.googlecode.com/svn/branches/uavxarm32_graphics/ftdi.jpg)

Start UAVPSet and check that you have the appropriate COM port selected.

Pulldown the Tools menu and select Software Test. You will see a range of tests.

In the tools select Load defaults.  Close the Software Tests window and select the **Read Config** icon. The default parameter set should populate the various boxes. There are a lot of parameters most of which you may never have to adjust. If you highlight any of the parameter boxes explanatory text should be displayed in the text window.

The default parameters were obtained from flights by Ken & Jim. These are a good starting point but you will probably wish to tune the Roll/Pitch and Yaw parameters in particular to suit your own flying style and the size/span and weight of your aircraft.

http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVPSet_Params.JPG

Now select the Setup Test.  Read the results and try to make sense of it. At the bottom you will see various alarms. With luck the only alarms will be that you have not calibrated the accelerometer and the Rx is not working.

http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVPSet_Start.JPG

### Step 3 (Rx) ###

Important - the black or ground pins of your Rx and Motor connections should be connected to the pins closest to the edge of the board. If in doubt double check with a multimeter.

From now on any changes you make to parameters in UAVPSet must be followed by selecting the **Write Config** icon. Some parameter changes involve an electrical reconfiguration of the board so you will occasionally see all LEDs flashing - cycle the power and this should allow the board to complete the reconfiguration.

Connect the front motor control lead to M1. This will power your Rx as centre pins of all of the M and Rx pins are connected together.

If you have a a Rx (e.g. FrSky) that supports Compound PPM (CPPM) connect it to Rx1. For CPPM you must specify the number of channels your Rx is receiving using UAVPSet.

If you have an Rx that uses parallel PPM then connect it to Rx1 up to Rx8 in the order Throttle, Aileron, Elevator, Rudder, Gear, Aux1, Aux2, Aux3. You need a minimum of 7 channels for full functionality.

For parallel PPM using UAVPSet set number of channels you have actually connected.

You can fly with a minimum of 4 channels Throttle, Aileron, Elevator, Rudder. If you do so then you will have altitude hold but no navigation capability.

You may re-assign the channels using the selectors in UAVPSet. So if your throttle is on Channel 3 for example then setup the pulldowns appropriately.

Turn on your Tx and run the Setup Test again. If you got lucky the Rx will be OK. Select the Rx Test and it will show you which channels are failing. The throttle channel should be around 5%, Aileron, Elevator and Rudder about 50% and Gear around 10%.

http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVPSet_RxTest.JPG

If the Rx Test passes then close the test software window and select the Tx/Rx setup icon. Double check the controls are behaving sensibly and the endpoints are under 100%.

http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVPSet_RxGraphic.JPG

As well as adjusting the navigation sensitivity NavS or Channel 7 (normally a potentiometer/knob) is used to enable altitude hold.  Initially you should fly with altitude hold off. Altitude hold is switch OFF below 10% and ON above 10%. The knob does not control altitude hold sensitivity.

In writing this an auto configure for the Rx has moved to the top of the development list but not for the first release!

The Green LED should be on and Yellow LED flashing every second.

### Step 4 (Acc/Gyro Temperature Calibration) ###

This is the most critical step in the setup and you usually only need to do it once or until you next bend the aircraft.

**YOU MUST CALIBRATE ACCELEROMETERS OTHERWISE THE ATTITUDE ESTIMATES (PITCH/ROLL ANGLES) WILL BE INCORRECT LEADING TO UNPREDICTABLE BEHAVIOUR.**

Put the aircraft on a level surface. If it is on the slightest of angles then this is the way it will fly when you centralise the sticks. Bear in mind the floors of your house will not be level, your best table will not be level nor will your average workbench. Eyeballing it is no good at all.

Get a decent spirit level and even then turn it around to make sure it reads level both ways.

Lock the quadrocopter down so it does not move and recheck it is level.

Power up with UAVPSet connected.  Select the Cal Accs/Gyros test. Confirm you are ready to go by selecting Continue.   You will see the blue LED start to flash - it has already taken the first set of readings and is waiting for the temperature to rise to take the second set.  Get a hair dryer and SLOWLY warm up the board - put your hand close to the board to sense how hot the air is. If it is too hot for your hand it is too hot for the board! You only need to change its temperature by 20 Celsius all up so don't melt the board!  There is a delay as the heat gets into the MPU6050 package so slowly/slowly until the blue LED stops flashing and you are done. Obviously it is better to do the calibration so it covers the range of temperatures you are likely to be flying in.

Now select the Acc/Gyro test to check that the Roll/Pitch/Yaw rate are close to zero. Also check the Acc values which should show Front/Back (FB) and LR close to zero and UD close to -1.

http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVPSet_Cal.JPG

Once again you only need to do this once or when loading the defaults or if you change the gyro selection pulldown - it should be on UAVXArm32 unless you are using analog gyros.

The Green LED and Yellow LEDs should be on.

_Note: See Accelerometer Neutral Fine Tuning below._

### Step 5 (Magnetometer Calibration) ###

Run the magnetometer calibration test in UAVPSet. Rotate the aircraft in all the directions you can think of including upside down and on edge. This captures the sensor offsets for the X,Y and Z axes using a little tricky math including fitting points to a sphere. You do not need to re-run the calibration unless it is clearly giving crazy answers for the computed compass heading. The offsets are stored in non-volatile memory where they are retained after disconnecting the battery. The magnetometer test should show the Neg/Pos values as being roughly equal. These values show how well you have rocked the aircraft around and correspond to visits to the octants around the axes.

At this point you should run the setup test and see if all of the alarms are cleared. If they are you are almost ready to fly but don't put the props on yet.

### Step 6 (Motors) ###

Disconnect the centre leads of the rest of your ESC control leads and make sure they are taped back and insulated.

Connect the other motors:

  * Left K2
  * Right K3
  * Back K4

Note: If you plan to run camera gimbals then it is worth considering buying a high current UBEC and connecting that to an unused Rx or M connector. In that case disconnect all of the BEC centre leads and connect the UBEC to an unused M/Rx connector.

Put the aircraft on the ground. Leave the props OFF and switch the arming switch on. UAVPSet communications are terminated once armed - you need to disarm to re-establish communications.

You will see a dancing pattern of Yellow and Blue LEDs while the Black Box memory is cleared; this takes about 8 seconds followed by a single beep. You should hear three starting beeps with the Red LED flashing briefly. At the end you should have Green and Red LEDs on. The Red LED means that no GPS signals are being received.

Advance the throttle and all motors should start. Run the motors more very slowly as you can damage them if you run at high speed without props. Listening carefully move your controls and verify that if you move the aileron right that the left motor increases speed and the right motor slows down. The same for elevator. Rudder left should slow down the front/back motors and speed up the left/right motors by the same amount.

Now lift the quadrocopter up and tilt it left/right, forward/backwards. It should be obvious what the motors will do. Tipping right should cause the right motor to speed up. Again use very little throttle.

If everything seems OK you can power down and make your own decision whether it is SAFE to fit the propellors and fly.

**THIS IS NOT A FLIGHT TUTORIAL SO FROM THIS POINT IT IS UP TO YOU.**

Join us at:

http://www.rcgroups.com/forums/showthread.php?t=1093510

## Next Steps ##

### GPS Connection and Initialisation ###

In what follows ou may wish to set your GPS unit's parameters using the manufacturer's tools. In this case leave the GPS Rx pin disconnected.

If you are using a CPPM capable RC receiver you connect the GPS Tx directly to RC3 and the GPS Rx to RC4. The Arming Switch remains a simple connection between the arming pin and the adjacent ground.

If you are using a receiver other than CPPM then the GPS Tx pin is connected to the Rx pin on the Com port along with the 3V and Ground pins. The GPS Rx pin should be connected to Aux2. Aux2 has some associated code that allows it to act as a serial Tx port for the brief time required to initialise the GPS. We do this as the Tx port on the serial connector is always used for downlink telemetry. A schematic for the combined Arming and GPS connections is in the Appendices but it is recommended that you use RC3 & 4 for GPS if at all possible as UAVXNav requires full bi-directional telemetry.

Bear in mind when selecting GPS options yourself that multicopters behave more like **pedestrians** as far as their movements are concerned so choosing this in the GPS filter options is likely to give better performance when hovering.

If using the NMEA protocol you should choose the $GPGGA and $GPRMC sentences at a minimum update rate of 5Hz. Communications is at 115KBaud.

The DPDT switch is arranged such that the GPS Tx is connected to the Com port Rx only when armed otherwise Com Port Rx is connected to the PC via the FTDI adapter. If the GPS is accidentally connected when UAVX is disarmed then the NMEA or binary transmission from the GPS will be interpreted as commands coming from UAVPSet with quite unpredictable results!

You must have a solid Green LED and a Blue LED flashing at the GPS rate which is normally 5Hz to fly. If you have a Red LED flashing intermittently **do not fly using GPS navigation** as you have poor GPS reception or there is some other fault.

### Accelerometer Neutral Fine Tuning ###

It is inevitable that the accelerometer neutrals will require small adjustments regardless of how well you leveled the aircraft. You can fine tune the neutrals using the Tx sticks.  The procedure is simple and is done when the aircraft is armed:

  * set you Tx aileron and elevator trims to centre (no trim).
  * fly the aircraft pointing away from you and note the directions that it drifts.
  * land and, with the throttle closed but still armed, hold the aileron or elevator stick at maximum in direction opposite to the drift until you hear a beep - there is a delay of 2 seconds to the first beep.
  * Fly again repeating until there is no drift.

You should do this when there is no wind and you only need to do it once. It is always better to adjust the neutrals rather than use the Tx trims if you want the flight controller to work properly.

You should never need yaw trim.

## Appendices ##

### GPS Wiring Harness For Non-CPPM Receivers ###

The schematic shows how the GPS and arming switch are combined into a single DPDT switch. The schematic pin to pin connections may be taken literally to assist in forming wiring bundles.

The Aux2 pin is used to program the GPS unit if you wish UAVX to initialise Ublox/MTK parameters. The connection may be omitted if the manufacturer's programmer is used.

http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVXArm32F4GPSHarness.JPG

### Motor/Servo Connector Assignments ###

The "K" output pin assignments which you should test with care are currently:

http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVXArm32F4_Outputs.JPG

### Pin Allocations ###

These are the Arm processor pin assignments.

http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVXArm32F4Pinout.JPG