﻿
#include "Internal.hpp"
#include "CAGainNode.hpp"

namespace ln {
namespace detail {

//==============================================================================
// CAGainNode

CAGainNode::CAGainNode(AudioDevice* context, AudioNode* frontNode)
	: AudioNodeCore(context, frontNode)
    , m_gain(1.0f)
{
}

void CAGainNode::init()
{
    AudioNodeCore::init();

    int numChannels = 2;
    addOutputPin(numChannels);
    addInputPin(numChannels);
}

void CAGainNode::process()
{
    AudioBus* destination = outputPin(0)->bus();
    AudioBus* source = inputPin(0)->bus();
    destination->copyWithGainFrom(*source, Math::clamp01(m_gain));
}

} // namespace detail
} // namespace ln

