http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVXNav3.JPG

# UAVXNav #

UAVXNav is a navigation mission planner with an inbuilt ground-station.

If you have GPS connected to the COM port Rx pin then you will have full downlink telemetry but the mission will remain fixed for the flight. If your Rx uses CPPM you will be using RC3&4 for your GPS connection. In this case you will have full bi-directional telemetry and can change mission parameters in flight.

Selectable voice feedback is available for those who like to look at the aircraft in flight rather than hopefully where it is on a screen.

## Tx setup ##

By default the functions selected by Tx Channel 5 are PIC, Hold assist and RTH. If you check the WP navigation enabled checkbox in UAVPSet the functions become Hold assist, Navigate and RTH.

## Emulation ##

You should use Emulation Mode to help you become familiar with the mission planning and the effect of Channel 5 switch positions.

You select Emulation Mode using the Flight Mode checkbox in UAVPSet. UAVX will then use representative physics to display what the aircraft is likely to do in flight; this includes GPS.  The emulation has a light breeze blowing to the NE and you will see the effect of this.

To get you started Emulation Mode generates a representative mission plan which you may change.

**REMOVE YOUR PROPELlORS WHEN USING EMULATION**. The software should disarm motor function when emulation is selected but I don't trust software so neither should you.

## First Emulated Flight ##

Just as with UAVXGS the aircraft must be **armed** for UAVXNav to work.

When UAVX starts to send telemetry select the Read button or select  Read in the mouse right click menu. This should upload any mission currently stored in UAVX. In Emulation Mode you should see a simple mission displayed.

**WAIT** until the "Home Set" status box is lit signifying GPS lock has occurred and the origin has been acquired.

Increase throttle and "fly" to around 15M. Slowly reduce the throttle to 45% which is hover. You should see the ROC drop to close to zero at which point the altitude hold will engage.

Select Navigate on Channel 5. You should now see (and hear) the aircraft start to traverse the mission.

Try selecting RTH then reselecting Navigate. Note you must go to Hold then back to Navigate otherwise RTH will continue.  Currently the mission is restarted not resumed where it left off.

## More Advanced ##

Most of the usual features you would expect are available:

  * The types of waypoints and some edits are available with right mouse click - the position of the mouse when you click will be the position of the waypoint.
  * The up/down buttons in the mission data box may be used to edit default values, change the waypoint sequence or delete individual waypoints.
  * Conditional waypoint sequences/loops are not currently available.

The waypoint types are currently:

  * VIA - where the aircraft pauses (T) at a waypoint attempting to stay within the proximity radius and altitude limits. If the time is set to zero then the aircraft must go through the waypoint within the proximity radius but not necessarily the altitude.
  * ORBIT - orbits the waypoint at the radius (R) with a velocity (V). UAVX will point an camera at the waypoint - not implemented as at Feb 8 2015 so reverts to VIA.
  * PERCH - lands at the waypoint then takes off and resumes mission.
  * Set POI - this is not a waypoint as such just a point that the aircraft will point to for the rest of the mission or until another Set POI is encountered.
  * Although not displayed RTH is always the default last waypoint. If there is no mission then selecting Navigate results in RTH.

If you do not designate a POI then the normal Turn To WP or hold heading configuration applies.

Any changes to the mission do not become effective until you **select Write**. You should always select Read immediately after to satisfy yourself that the mission appears OK.

Note: UAVXSet also allows you to check whether a current mission is loaded in the **Setup Test**.

## Go To ##

If the Go To checkbox is checked then a right click of the mouse will generate a 1 point mission. This allows you to fly to a designated point dynamically with no control inputs. Again you must select Write to upload the mission change.

## Finally ##

It is important to familiarise yourself with what happens when you change the Channel 5 switch selection. Some of the changes may not be exactly as you expect and it is better to see the effect under emulation ;).

## Video ##

This is a video from 2010 to give you and idea of what you may expect. The screen display has changed a little since that time.

Video shows UAVXNav waypoint navigation and auto-land. Starts with the setting of the last of 4 waypoints; manual takeoff with climb to 35M and 5 Sec loiter at waypoint 2; returning to land after 10 sec hold at the launch point.

Note altitude and position error display along with aircraft location relative to launch. Finally read of in flight stats showing GPS and sensor performance with any Rx failures.

<a href='http://www.youtube.com/watch?feature=player_embedded&v=TdTr5837Rdg' target='_blank'><img src='http://img.youtube.com/vi/TdTr5837Rdg/0.jpg' width='425' height=344 /></a>



## First Release ##

Northern Summer 2010.

### Acknowledgements ###

As of 2015 mission editing is based on that from MultiWiiWinGUI by Andras Schaffer (EOSBandi) (C) 2014. Avionics Instrument Controls were written by Guillaume Chouteau. UAVXNav was originally inspired by Jordi Muñoz from ArduPilotConfigTool.

UAVXNav now uses GMap.NET and allows selection of maps from a variety of sources in addition to the original Google Maps.

_G.K. Egan 2015_