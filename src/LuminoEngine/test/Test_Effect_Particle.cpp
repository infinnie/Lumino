﻿#include "Common.hpp"
#include <LuminoEngine/Effect/ParticleEffectModel2.hpp>
#include "../src/Effect/ParticleEffectInstance.hpp"

class Test_Effect_Particle : public ::testing::Test {};

//------------------------------------------------------------------------------
TEST_F(Test_Effect_Particle, Lifetime)
{
	auto particleModel = makeObject<ParticleModel2>();
	auto emitterModel = particleModel->emitters()[0];
	emitterModel->m_lifeTime = { 1, 1 };	// 1s で消えるようにする
	emitterModel->m_maxParticles = 1;

	// Loop off
	{
		particleModel->m_loop = false;

		auto particleInstance = makeObject<detail::ParticleInstance2>(particleModel);
		auto emitterInstance = particleInstance->emitters()[0];

		// 1フレーム分進める。1つ Particle ができている。
		{
			particleInstance->updateFrame(0.016);
			ASSERT_EQ(true, emitterInstance->particleData(0).isActive());
		}
		// さらに1秒進める。Particle は消えている。
		{
			particleInstance->updateFrame(1.0);
			ASSERT_EQ(false, emitterInstance->particleData(0).isActive());
		}
	}

	// Loop on
	{
		particleModel->m_loop = true;

		auto particleInstance = makeObject<detail::ParticleInstance2>(particleModel);
		auto emitterInstance = particleInstance->emitters()[0];

		// 1フレーム分進める。1つ Particle ができている。
		{
			particleInstance->updateFrame(0.016);
			ASSERT_EQ(true, emitterInstance->particleData(0).isActive());
		}
		// さらに0.9秒進める。Particle 新しいものが生成されている。1秒経ってないのでまだ存在している
		{
			particleInstance->updateFrame(0.9);
			ASSERT_EQ(true, emitterInstance->particleData(0).isActive());
		}
	}
}
