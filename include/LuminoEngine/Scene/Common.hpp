﻿
#pragma once
#include "../Graphics/ColorStructs.hpp"
#include "../Graphics/GeometryStructs.hpp"

namespace ln {
class Level;
class Camera;
class Raycaster;
class RaycastResult;
class AbstractMaterial;

enum class LevelTransitionEffectMode
{
	None,

	/**  */
	FadeInOut,

	/**  */
	CrossFade,
};

} // namespace ln
