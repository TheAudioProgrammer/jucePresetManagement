#pragma once

#include <JuceHeader.h>

namespace Utility
{
	class ParameterHelper
	{
	public:
		ParameterHelper() = delete;

		static AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
		{
			return AudioProcessorValueTreeState::ParameterLayout{
				std::make_unique<AudioParameterFloat>("time", "Time", NormalisableRange<float>{30.f, 3000.f, 0.01f}, 200.f),
				std::make_unique<AudioParameterFloat>("feedback", "Feedback", NormalisableRange<float>{0.0f, 0.95f, 0.01f}, 0.3f),
				std::make_unique<AudioParameterFloat>("mix", "Mix", NormalisableRange<float>{0.0f, 1.0f, 0.01f}, 0.5f),
				std::make_unique<AudioParameterChoice>("type", "Delay Type", StringArray{ "Tape", "Digital", "Ping Pong", "Reverse" }, 0),
				std::make_unique<AudioParameterBool>("sync", "Sync To BPM", false)
			};
		}
	};
}
