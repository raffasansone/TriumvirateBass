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
			"Test_pre", 
			R"V0G0N(<?xml version="1.0" encoding="UTF - 8"?>
<PARAMETERS presetName="Test_pre" default="1">
	<PARAM id="highPreampGain" value="48.04999923706055"/>
	<PARAM id="highVolume" value="-17.85000610351562"/>
	<PARAM id="inputGain" value="0.0"/>
	<PARAM id="lowPreampGain" value="63.10000228881836"/>
	<PARAM id="lowVolume" value="0.0"/>
	<PARAM id="midPreampGain" value="67.90000152587891"/>
	<PARAM id="midVolume" value="0.0"/>
	<PARAM id="outputGain" value="0.0"/>
	<PARAM id="bypass"/>
</PARAMETERS>
			")V0G0N"
		},

		{
			"Test_pre2",
			R"V0G0N(<?xml version="1.0" encoding="UTF - 8"?>
<?xml version="1.0" encoding="UTF - 8"?>
<PARAMETERS presetName="Test_pre2" default="1">
	<PARAM id="highPreampGain" value="48.04999923706055"/>
	<PARAM id="highVolume" value="-10.85000610351562"/>
	<PARAM id="inputGain" value="0.0"/>
	<PARAM id="lowPreampGain" value="60.10000228881836"/>
	<PARAM id="lowVolume" value="0.0"/>
	<PARAM id="midPreampGain" value="57.90000152587891"/>
	<PARAM id="midVolume" value="0.0"/>
	<PARAM id="outputGain" value="0.0"/>
	<PARAM id="bypass"/>
</PARAMETERS>
			")V0G0N"
		}
	};
}