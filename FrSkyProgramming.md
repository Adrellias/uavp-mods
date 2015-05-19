# Introduction #

[FrSky](http://www.frsky-rc.com/) is a relatively new company and their developmental cycle is quite fast with the documentation often lagging.

What follows is not a recommendation of the [FrSky](http://www.frsky-rc.com/) products in general but is intended to put in one place information that is currently difficult to find.

## Composite Output ##
While the D8R Rx is not directly compatible with UAVX a recent firmaware release allows it to directly generate composite PPM signals where "All Channels" come out on Ch 8. The function of channels 1 to 7 are preserved.

The firmware update process is quite easy. You will need:

  * The programming cable which is available from [FrSky](http://www.frsky-rc.com/) and should come with the combination kit (http://www.frsky-rc.com/ShowProducts.asp?id=42).

  * The updater frsky-upgrade-lite(V1) or later (http://www.frsky-rc.com/download.asp?id=22 ).

## Process ##

![http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/FrSky.jpg](http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/FrSky.jpg)

  * Make up a cable as shown in the photo due to "RENATOA". Note that the Tx pin is mislabelled on the D8R V2 as +5V. This is to be corrected by [FrSky](http://www.frsky-rc.com/) in later releases.

  * Connect the Rx signal pins on Ch7 and 8 together using a jumper plug/wire. **IMPORTANT** - This is how the Rx detects it is in programming mode.

  * Connect your adapter cable to the serial adapter cable you received with the Tx module.

  * Power up the Rx. The LEDs should be out.

  * Start up the updater from the frsky-update-lite files (currently frsky\_update\_rev11).

  * Select the appropriate COM port for the adapter.

  * Select the firmware **file** which is currently http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/fdd_rx_cppm_build110120.frk ). Further release versions should appear on the FrSky site in due course. You should see information on the current Rx version at the bottom of the window.

  * Select **Download** and when finished select **End**

  * Power down the Rx and remove all cables.

# That's it! #

![http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/FrSkyD8RComposite.png](http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/FrSkyD8RComposite.png)


Reference: http://www.rcgroups.com/forums/showpost.php?p=17169119&postcount=2827