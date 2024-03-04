#ifndef __MU_PRECOMPILED_H__
#define __MU_PRECOMPILED_H__

#pragma once

// This doesn't change only the layout of glm::quat so shouldn't be used
//#define GLM_FORCE_QUAT_DATA_XYZW 1
#define GLM_FORCE_QUAT_DATA_WXYZ 1 // vcpkg glm requires this to work as before
#define GLM_ENABLE_EXPERIMENTAL 1

#include "shared_precompiled.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <FreeImage.h>
#include <entt/entt.hpp>

#if PHYSICS_ENABLED == 1
#include <PxPhysicsAPI.h>
#endif

#include "mu_version.h"

/* Diligent Engine */
#include <RefCntAutoPtr.hpp>
#include <EngineFactory.h>
#include <RenderDevice.h>
#include <DeviceContext.h>
#include <SwapChain.h>
#include <ScreenCapture.hpp>
#include <GraphicsTypesX.hpp>
#include <AdvancedMath.hpp>
#include <ShadowMapManager.hpp>

#define NEXTMU_TITLE "NextMU Project"

#define NEXTMU_UI_DUMMY			(0)
#define NEXTMU_UI_NOESISGUI		(1)
#define NEXTMU_UI_RMLUI			(2)
#define NEXTMU_UI_ULTRALIGHT	(3)

#ifdef NEXTMU_USE_NOESISGUI
#define NEXTMU_UI_LIBRARY NEXTMU_UI_NOESISGUI
#elif NEXTMU_USE_RMLUI
#define NEXTMU_UI_LIBRARY NEXTMU_UI_RMLUI
#elif NEXTMU_USE_ULTRALIGHT
#define NEXTMU_UI_LIBRARY NEXTMU_UI_ULTRALIGHT
#else
#define NEXTMU_UI_LIBRARY NEXTMU_UI_DUMMY
#endif

#define NEXTMU_COMPRESSED_MESHS (0)
#define NEXTMU_COMPRESSED_PARTICLES (0)
#define NEXTMU_COMPRESSED_JOINTS (0)

#define NEXTMU_RENDER_BBOX (0)

#if NEXTMU_UI_LIBRARY == NEXTMU_UI_NOESISGUI
/*
	If this file is missing means you have to create it,
	use ui_noesisgui_license.h.template as base,
	put your license information and you're ready to use NoesisGUI.
*/
#include "ui_noesisgui_license.h"
#include <NoesisPCH.h>
#endif

#include "mu_math.h"
#include "mu_model.h"
#include "mu_terrain.h"
#include "mu_skeletoninstance.h"

#endif