/*
  ==============================================================================

    DefaultPresets.h
    Created: 14 Sep 2023 9:47:01pm
    Author:  jorda

  ==============================================================================
*/

#pragma once

#include <unordered_map>
#include <JuceHeader.h>

namespace DefaultPresets {
	std::unordered_map<juce::String, juce::String> defaultPresetsMap = {

		{
			"Aslobomb", 
			R"V0G0N(<?xml version="1.0" encoding="UTF - 8"?>
<PARAMETERS presetName="Aslobomb" default="1">
  <PARAM id="bypass" value="0.0"/>
  <PARAM id="highPreampGain" value="89.59999847412109"/>
  <PARAM id="highVolume" value="-8.200004577636719"/>
  <PARAM id="inputGain" value="0.0"/>
  <PARAM id="lowPreampGain" value="35.54999923706055"/>
  <PARAM id="lowVolume" value="-2.1300048828125"/>
  <PARAM id="midPreampGain" value="36.85000228881836"/>
  <PARAM id="midVolume" value="-6.330001831054688"/>
  <PARAM id="outputGain" value="0.0"/>
</PARAMETERS>
			")V0G0N"
		},

		{
			"BuffMeUp",
			R"V0G0N(<?xml version="1.0" encoding="UTF - 8"?>
<PARAMETERS presetName="BuffMeUp" default="1">
  <PARAM id="bypass" value="0.0"/>
  <PARAM id="highPreampGain" value="20.14999961853027"/>
  <PARAM id="highVolume" value="-2.900001525878906"/>
  <PARAM id="inputGain" value="0.0"/>
  <PARAM id="lowPreampGain" value="30.85000038146973"/>
  <PARAM id="lowVolume" value="0.0"/>
  <PARAM id="midPreampGain" value="0.0"/>
  <PARAM id="midVolume" value="0.0"/>
  <PARAM id="outputGain" value="0.0"/>
</PARAMETERS>
			")V0G0N"
		},

		{
			"Every Punk Tone",
			R"V0G0N(<?xml version="1.0" encoding="UTF - 8"?>
<PARAMETERS presetName="Every Punk Tone" default="1">
  <PARAM id="bypass" value="0.0"/>
  <PARAM id="highPreampGain" value="56.04999923706055"/>
  <PARAM id="highVolume" value="-8.600006103515625"/>
  <PARAM id="inputGain" value="0.0"/>
  <PARAM id="lowPreampGain" value="54.85000228881836"/>
  <PARAM id="lowVolume" value="-8.020004272460938"/>
  <PARAM id="midPreampGain" value="17.05000114440918"/>
  <PARAM id="midVolume" value="-23.42000579833984"/>
  <PARAM id="outputGain" value="0.0"/>
</PARAMETERS>

			")V0G0N"
		},

		{
			"Golden Fingers",
			R"V0G0N(<?xml version="1.0" encoding="UTF - 8"?>
<PARAMETERS presetName="Golden Fingers" default="1">
  <PARAM id="bypass" value="0.0"/>
  <PARAM id="highPreampGain" value="9.949999809265137"/>
  <PARAM id="highVolume" value="-7.75"/>
  <PARAM id="inputGain" value="0.0"/>
  <PARAM id="lowPreampGain" value="54.65000152587891"/>
  <PARAM id="lowVolume" value="0.0"/>
  <PARAM id="midPreampGain" value="0.0"/>
  <PARAM id="midVolume" value="-4.850006103515625"/>
  <PARAM id="outputGain" value="0.0"/>
</PARAMETERS>
			")V0G0N"
		},

		{
			"Outrageous Fuzz",
			R"V0G0N(<?xml version="1.0" encoding="UTF - 8"?>
<PARAMETERS presetName="Outrageous Fuzz" default="1">
  <PARAM id="bypass" value="0.0"/>
  <PARAM id="highPreampGain" value="100.0"/>
  <PARAM id="highVolume" value="-2.120002746582031"/>
  <PARAM id="inputGain" value="0.0"/>
  <PARAM id="lowPreampGain" value="100.0"/>
  <PARAM id="lowVolume" value="-2.770004272460938"/>
  <PARAM id="midPreampGain" value="90.90000152587891"/>
  <PARAM id="midVolume" value="-5.950004577636719"/>
  <PARAM id="outputGain" value="-6.03000020980835"/>
</PARAMETERS>
			")V0G0N"
		},

		{
			"Slap Master",
			R"V0G0N(<?xml version="1.0" encoding="UTF - 8"?>
<PARAMETERS presetName="Slap Master" default="1">
  <PARAM id="bypass" value="0.0"/>
  <PARAM id="highPreampGain" value="9.949999809265137"/>
  <PARAM id="highVolume" value="-0.220001220703125"/>
  <PARAM id="inputGain" value="5.510000228881836"/>
  <PARAM id="lowPreampGain" value="28.64999961853027"/>
  <PARAM id="lowVolume" value="-2.110000610351562"/>
  <PARAM id="midPreampGain" value="30.25"/>
  <PARAM id="midVolume" value="-14.66000366210938"/>
  <PARAM id="outputGain" value="0.0"/>
</PARAMETERS>
			")V0G0N"
		},

		{
			"The Pieces Fit",
			R"V0G0N(<?xml version="1.0" encoding="UTF - 8"?>
<PARAMETERS presetName="The Pieces Fit" default="1">
  <PARAM id="bypass" value="0.0"/>
  <PARAM id="highPreampGain" value="37.70000076293945"/>
  <PARAM id="highVolume" value="-9.709999084472656"/>
  <PARAM id="inputGain" value="3.719999313354492"/>
  <PARAM id="lowPreampGain" value="32.45000076293945"/>
  <PARAM id="lowVolume" value="-8.950004577636719"/>
  <PARAM id="midPreampGain" value="34.60000228881836"/>
  <PARAM id="midVolume" value="-54.1300048828125"/>
  <PARAM id="outputGain" value="5.579999923706055"/>
</PARAMETERS>
			")V0G0N"
		}

	};
}