#include "mu_precompiled.h"
#include "mu_renderstate.h"
#include "mu_environment.h"
#include "mu_camera.h"
#include "mu_graphics.h"
#include "mu_resourcesmanager.h"
#include <MapHelper.hpp>

namespace MURenderState
{
	glm::mat4 FrustomProjection, Projection, View, ViewProjection, ViewProjectionTransposed;
	glm::mat4 ShadowView, ShadowProjection;
	NCamera *Camera = nullptr;
	NEnvironment *Environment = nullptr;
	std::vector<NGraphicsTexture *> TextureAttachments;

	Diligent::RefCntAutoPtr<Diligent::IBuffer> CameraUniform;
	Diligent::RefCntAutoPtr<Diligent::IBuffer> LightUniform;

	mu_uint32 RenderWidth = 0u, RenderHeight = 0u;
	NRenderMode RenderMode = NRenderMode::Normal;

	NResourceId ShadowResourceId = NInvalidUInt32;
	NShadowMode ShadowMode = NShadowMode::PCF;
	Diligent::ShadowMapManager *ShadowMap = nullptr;

	const mu_boolean Initialize()
	{
		const auto device = MUGraphics::GetDevice();
		const auto immediateContext = MUGraphics::GetImmediateContext();

		// Camera Uniform
		if (CameraUniform.RawPtr() == nullptr)
		{
			Diligent::BufferDesc bufferDesc;
			bufferDesc.Usage = Diligent::USAGE_DYNAMIC;
			bufferDesc.BindFlags = Diligent::BIND_UNIFORM_BUFFER;
			bufferDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;
			bufferDesc.Size = sizeof(Diligent::CameraAttribs);

			Diligent::RefCntAutoPtr<Diligent::IBuffer> buffer;
			device->CreateBuffer(bufferDesc, nullptr, &buffer);
			if (buffer == nullptr)
			{
				return false;
			}

			CameraUniform = buffer;

			Diligent::MapHelper<Diligent::CameraAttribs> uniform(immediateContext, CameraUniform, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD);
			*uniform = Diligent::CameraAttribs();
		}

		// Light Uniform
		if (LightUniform.RawPtr() == nullptr)
		{
			Diligent::BufferDesc bufferDesc;
			bufferDesc.Usage = Diligent::USAGE_DYNAMIC;
			bufferDesc.BindFlags = Diligent::BIND_UNIFORM_BUFFER;
			bufferDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;
			bufferDesc.Size = sizeof(Diligent::LightAttribs);

			Diligent::RefCntAutoPtr<Diligent::IBuffer> buffer;
			device->CreateBuffer(bufferDesc, nullptr, &buffer);
			if (buffer == nullptr)
			{
				return false;
			}

			LightUniform = buffer;

			Diligent::MapHelper<Diligent::LightAttribs> uniform(immediateContext, LightUniform, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD);
			*uniform = Diligent::LightAttribs();
		}

		TextureAttachments.resize(MUResourcesManager::GetResourcesManager()->GetAttachmentsCount(), nullptr);

		return true;
	}

	void Destroy()
	{
		CameraUniform.Release();
		LightUniform.Release();
	}

	void Reset()
	{
		Environment = nullptr;
		for (mu_uint32 n = 0; n < TextureAttachments.size(); ++n)
		{
			TextureAttachments[n] = nullptr;
		}
	}

	void SetRenderSize(const mu_uint32 width, const mu_uint32 height)
	{
		RenderWidth = width;
		RenderHeight = height;
	}

	mu_uint32 GetRenderWidth()
	{
		return RenderWidth;
	}

	mu_uint32 GetRenderHeight()
	{
		return RenderHeight;
	}

	void SetRenderMode(const NRenderMode mode)
	{
		RenderMode = mode;
	}

	const NRenderMode GetRenderMode()
	{
		return RenderMode;
	}

	void SetShadowResourceId(const NResourceId resourceId)
	{
		ShadowResourceId = resourceId;
	}

	const NResourceId GetShadowResourceId()
	{
		return ShadowResourceId;
	}

	void SetShadowMode(const NShadowMode mode)
	{
		ShadowMode = mode;
	}

	const NShadowMode GetShadowMode()
	{
		return ShadowMode;
	}

	void SetShadowMap(Diligent::ShadowMapManager *shadowMap)
	{
		ShadowMap = shadowMap;
	}

	Diligent::ShadowMapManager *GetShadowMap()
	{
		return ShadowMap;
	}

	Diligent::IBuffer *GetCameraUniform()
	{
		return CameraUniform.RawPtr();
	}

	Diligent::IBuffer *GetLightUniform()
	{
		return LightUniform.RawPtr();
	}

	void SetViewTransform(glm::mat4 view, glm::mat4 projection, glm::mat4 frustumProjection, glm::mat4 shadowView, glm::mat4 shadowProjection)
	{
		FrustomProjection = frustumProjection;
		Projection = projection;
		View = view;
		ViewProjection = Projection * View;
		ViewProjectionTransposed = glm::transpose(ViewProjection);

		ShadowView = shadowView;
		ShadowProjection = shadowProjection;
	}

	void SetViewProjection(glm::mat4 viewProj)
	{
		ViewProjection = viewProj;
		ViewProjectionTransposed = glm::transpose(ViewProjection);
	}

	glm::mat4 &GetViewProjection()
	{
		return ViewProjection;
	}

	glm::mat4 &GetViewProjectionTransposed()
	{
		return ViewProjectionTransposed;
	}

	glm::mat4 &GetFrustumProjection()
	{
		return Projection;
	}

	glm::mat4 &GetProjection()
	{
		return Projection;
	}

	glm::mat4 &GetView()
	{
		return View;
	}

	glm::mat4 &GetShadowProjection()
	{
		return ShadowProjection;
	}

	glm::mat4 &GetShadowView()
	{
		return ShadowView;
	}

	void AttachCamera(NCamera *camera)
	{
		Camera = camera;
	}

	void DetachCamera()
	{
		Camera = nullptr;
	}

	void AttachEnvironment(NEnvironment *environment)
	{
		Environment = environment;
	}

	void DetachEnvironment()
	{
		Environment = nullptr;
	}

	const NCamera *GetCamera()
	{
		return Camera;
	}

	const NEnvironment *GetEnvironment()
	{
		return Environment;
	}

	NTerrain *GetTerrain()
	{
		if (Environment == nullptr) return nullptr;
		return Environment->GetTerrain();
	}

	void AttachTexture(const NTextureAttachmentType type, NGraphicsTexture *texture)
	{
		TextureAttachments[type] = texture;
	}

	void DetachTexture(const NTextureAttachmentType type)
	{
		TextureAttachments[type] = nullptr;
	}

	NGraphicsTexture *GetTexture(const NTextureAttachmentType type)
	{
		if (type == NInvalidAttachment) return nullptr;
		return TextureAttachments[type];
	}
}