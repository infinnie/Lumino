﻿#include "Common.hpp"
#include <LuminoEngine/Effect/ParticleEffectModel2.hpp>
#include "../src/Effect/ParticleEffectInstance.hpp"

class Test_Effect_Particle : public ::testing::Test {};

#if 0
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

//------------------------------------------------------------------------------
TEST_F(Test_Effect_Particle, SpawnRate)
{
	auto particleModel = makeObject<ParticleModel2>();
	auto emitterModel = particleModel->emitters()[0];
	emitterModel->setLifeTime(5);
	emitterModel->setMaxParticles(50);
	emitterModel->setSpawnRate(10);			// 1s 10個

	auto particleInstance = makeObject<detail::ParticleInstance2>(particleModel);
	auto emitterInstance = particleInstance->emitters()[0];

	{
		// 1s 進めたら、10 個作られている
		particleInstance->updateFrame(1.0f);
		ASSERT_EQ(10, emitterInstance->activeParticles());

		ASSERT_FLOAT_EQ(0.0, emitterInstance->particleData(0).time);
		ASSERT_FLOAT_EQ(0.1, emitterInstance->particleData(1).time);
		ASSERT_FLOAT_EQ(0.8, emitterInstance->particleData(8).time);
		ASSERT_FLOAT_EQ(0.9, emitterInstance->particleData(9).time);
	}
}

//------------------------------------------------------------------------------
TEST_F(Test_Effect_Particle, SphereShape)
{
	auto particleModel = makeObject<ParticleModel2>();
	auto emitterModel = particleModel->emitters()[0];
	emitterModel->setLifeTime(10);
	emitterModel->setMaxParticles(10);
	emitterModel->setSpawnRate(10);

	// 半径 1.0 の表面上に一様に配置する
	emitterModel->m_shapeType = ParticleEmitterShapeType::Sphere;
	emitterModel->m_forwardPosition.set(1.0f);
	emitterModel->m_forwardVelocity.set(0);

	auto particleInstance = makeObject<detail::ParticleInstance2>(particleModel);
	auto emitterInstance = particleInstance->emitters()[0];

	{
		particleInstance->updateFrame(1.0f);
		ASSERT_EQ(10, emitterInstance->activeParticles());
		for (int i = 0; i < 10; i++) {
			// 全部中心から 1.0 の距離にあるはず
			ASSERT_FLOAT_EQ(1.0, emitterInstance->particleData(i).position.length());
		}
	}
}

//------------------------------------------------------------------------------
TEST_F(Test_Effect_Particle, ConeShape)
{
	auto particleModel = makeObject<ParticleModel2>();
	auto emitterModel = particleModel->emitters()[0];
	emitterModel->setLifeTime(10);
	emitterModel->setMaxParticles(10);
	emitterModel->setSpawnRate(10);

	emitterModel->m_shapeType = ParticleEmitterShapeType::Cone;
	emitterModel->m_shapeParam.x = Math::PI / 3;
	emitterModel->m_forwardPosition.set(1.0f);
	emitterModel->m_forwardVelocity.set(0);

	auto particleInstance = makeObject<detail::ParticleInstance2>(particleModel);
	auto emitterInstance = particleInstance->emitters()[0];

	{
		particleInstance->updateFrame(1.0f);
		ASSERT_EQ(10, emitterInstance->activeParticles());
		for (int i = 0; i < 10; i++) {
			// 全部中心から 1.0 の距離にあるはず
			ASSERT_FLOAT_EQ(1.0, emitterInstance->particleData(i).position.length());
			// 有効角度が  (Math::PI / 3) なので、正面方向である Y+ の後ろにはないはず
			ASSERT_EQ(true, emitterInstance->particleData(i).position.y > 0.0f);
		}
	}
}

//------------------------------------------------------------------------------
TEST_F(Test_Effect_Particle, BoxShape)
{
	auto particleModel = makeObject<ParticleModel2>();
	auto emitterModel = particleModel->emitters()[0];
	emitterModel->setLifeTime(10);
	emitterModel->setMaxParticles(10);
	emitterModel->setSpawnRate(10);

	emitterModel->m_shapeType = ParticleEmitterShapeType::Box;
	emitterModel->m_shapeParam.set(10, 10, 10);
	emitterModel->m_forwardVelocity.set(0);

	auto particleInstance = makeObject<detail::ParticleInstance2>(particleModel);
	auto emitterInstance = particleInstance->emitters()[0];

	{
		particleInstance->updateFrame(1.0f);
		ASSERT_EQ(10, emitterInstance->activeParticles());
		for (int i = 0; i < 10; i++) {
			auto pos = emitterInstance->particleData(i).position;
			ASSERT_FLOAT_EQ(true, -5.0f <= pos.x && pos.x <= 5.0f);
			ASSERT_FLOAT_EQ(true, -5.0f <= pos.y && pos.y <= 5.0f);
			ASSERT_FLOAT_EQ(true, -5.0f <= pos.z && pos.z <= 5.0f);
		}
	}
}

////------------------------------------------------------------------------------
//TEST_F(Test_Effect_Particle, ZSort)
//{
//	auto particleModel = makeObject<ParticleModel2>();
//	auto emitterModel = particleModel->emitters()[0];
//	emitterModel->setLifeTime(10);
//	emitterModel->setMaxParticles(2);
//	emitterModel->setSpawnRate(2);
//	emitterModel->m_forwardVelocity.set(0);
//	emitterModel->m_velocity.set(Vector3(0, 0, -1));
//
//	auto particleInstance = makeObject<detail::ParticleInstance2>(particleModel);
//	auto emitterInstance = particleInstance->emitters()[0];
//
//	{
//		particleInstance->updateFrame(1.0f);
//		ASSERT_EQ(10, emitterInstance->activeParticles());
//
//
//		auto pos0 = emitterInstance->particleData(0).position;
//		auto pos1 = emitterInstance->particleData(1).position;
//
//		ASSERT_FLOAT_EQ(true, pos0.z < pos1.z);
//	}
//}

#endif