
Triumvirate Bass Distortion plugin
==================================

by Raffaele Sansone.
Published under the BSD License (3 clause)


Disclaimer
==========

Although I have permission to use the VFE logo and product name, this is NOT a 
VFE affiliated product, but my own, independent digital recreation.

This plugin is still in the alpha stage of development.
Please use the plugin at your own risk (i.e. don’t load it on important 
sessions, just to be sure).

You can report any issues at info.raffaelesansone@gmail.com.


Build
=====

You will need JUCE 7+ to build this program.
Open the Projucer project file and export to your IDE of choice.

The project depends on the ff_meters module for input and output graphic 
metering. 
Repository: https://github.com/ffAudio/ff_meters.git
API documentation: https://foleysfinest.com/ff_meters/

ff_meters has been added as a submodule for this repository.
Executing the command 'git submodule update --init --recursive' upon cloning
the Triumvirate repository should set everything up for you.

Once you make sure the dependencies are in place, the software should compile 
without issues.


How to use
==========

This is the digital model of a real bass preamp pedal, the VFE Triumvirate.
The pedal is composed of 3 individual bands, for which you can independently 
adjust Low, Mid and High level and distortion.

As such, it is advised to use a bass cab sim after the pedal or it will sound 
harsh with a lot of distortion.

Two things about the pedal that you might find initially counter-intuitive:
Mids and Highs controls are “inverted” if you read the pedal left to right. 
The EQ section is passive: the knobs range from minus infinity to unity gain 
for each band.

********************************************************************************
