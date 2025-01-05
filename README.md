
Triumvirate Bass Distortion plugin
==================================

by Raffaele Sansone.
Published under the BSD License (3 clause)


Disclaimer
==========

Although I have permission to use the VFE logo and product name, this is NOT a \
VFE affiliated product, but my own, independent digital recreation.

This plugin is still in the alpha stage of development. \
Please use the plugin at your own risk (i.e. don’t load it on important \
sessions, just to be sure).

You can report any issues at info.raffaelesansone@gmail.com.


Build on Windows
=====

This plugin was created using JUCE 7 and Visual Studio 2022.

Executing 'setup_pc.bat' upon cloning the Triumvirate repository will set \
everything up and open the .jucer file in Projucer. From the Projucer, choose \
the VS 2022 exporter and open the Solution.

If you wish to use a different VS version you can call the 'setup_pc.bat' \
specifying the year parameter. \
For example: \
```setup_pc.bat 2019 (will build the projucer with VS 2019)```


Build on MacOS
=====

Automatic setup file is not yet available for MacOS. \
After cloning, run the command: \
```git submodule update --init --recursive```

If you don't have the Projucer installed, build it from the .xcodeproj file \
found at: \
```.\JUCE\extras\Projucer\Builds\MacOSX```

Open the TriumvirateBass.jucer file with the Projucer and export to your IDE \
of choice.


Dependencies
============

The project depends on the ff_meters module for input and output graphic \
metering. \
Repository: https://github.com/ffAudio/ff_meters.git \
API documentation: https://foleysfinest.com/ff_meters/


How to use
==========

This is the digital model of a real bass preamp pedal, the VFE Triumvirate. \
The pedal is composed of 3 individual bands, for which you can independently \
adjust Low, Mid and High level and distortion.

As such, it is advised to use a bass cab sim after the pedal or it will sound \
extremely harsh.

Two things about the pedal that you might find counter-intuitive:
- Mids and Highs controls are “inverted” if you read the pedal left to right.
- The EQ section is passive: the knobs range from minus infinity to unity gain \
for each band. In guitar/bass gear fashion, the bands are far from linear \
phase response.

********************************************************************************
