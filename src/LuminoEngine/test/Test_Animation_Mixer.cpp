﻿#include "Common.hpp"
class Test_Animation_Mixer : public ::testing::Test {};

//------------------------------------------------------------------------------
TEST_F(Test_Animation_Mixer, Basic)
{
	auto clip1 = makeObject<AnimationClip>();
	auto curve1 = EasingAnimationCurve::create(0, 10, 1, EasingMode::Linear);
	auto track1 = makeObject<ScalarAnimationTrack>();
	track1->setTargetName(u"Bone.1");
	track1->setCurve(curve1);

	clip1->addTrack(track1);


	class AnimCtrl : public detail::IAnimationMixerCoreHolder
	{
	public:
		Ref<detail::AnimationTargetElementBlendLink> m_bone1Binding;
		float m_bone1Value;

		detail::AnimationTargetElementBlendLink* onRequireBinidng(const String& name) override
		{
			if (name == u"Bone1.1") {
				m_bone1Binding = makeRef<detail::AnimationTargetElementBlendLink>();
				return m_bone1Binding;
			}
			else {
				return nullptr;
			}
		}

		void onUpdateTargetElement(const detail::AnimationTargetElementBlendLink* binding) override
		{
			if (LN_REQUIRE(m_bone1Binding == binding)) return;
			m_bone1Value = binding->rootValue.getFloat();
		}
	} ctrl;

	auto mixer1 = makeObject<AnimationMixerCore>(&ctrl);
	mixer1->addClip(u"Idle", clip1);
	mixer1->play(u"Idle", 0.0f);

	mixer1->advanceTime(0.0f);
	ASSERT_FLOAT_EQ(0.0f, ctrl.m_bone1Value);

	mixer1->advanceTime(1.0f);
	ASSERT_FLOAT_EQ(10.0f, ctrl.m_bone1Value);
}
