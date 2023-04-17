#include "stdafx.h"
#include "mu_environment_objects.h"
#include "mu_environment.h"
#include "mu_entity.h"
#include "mu_camera.h"
#include "mu_modelrenderer.h"
#include "mu_bboxrenderer.h"
#include "mu_state.h"
#include "mu_renderstate.h"
#include "mu_threadsmanager.h"
#include "res_renders.h"

const mu_boolean NObjects::Initialize()
{
	return true;
}

void NObjects::Destroy()
{

}

void NObjects::Update()
{
	const auto updateTime = MUState::GetUpdateTime();
	const auto frustum = MURenderState::GetCamera()->GetFrustum();
	const auto environment = MURenderState::GetEnvironment();

	const auto view = Registry.view<
		NEntity::NRenderable,
		NEntity::NAttachment,
		NEntity::NLight,
		NEntity::NRenderState,
		NEntity::NSkeleton,
		NEntity::NPosition,
		NEntity::NAnimation,
		NEntity::NBoundingBoxes
	>();

	MUThreadsManager::Run(
		std::unique_ptr<NThreadExecutorBase>(
			new (std::nothrow) NThreadExecutorIterator(
				view.begin(), view.end(),
				[&view, environment, frustum, updateTime](const entt::entity entity) -> void {
					auto [attachment, light, renderState, skeleton, position, animation, boundingBox] = view.get<
						NEntity::NAttachment,
						NEntity::NLight,
						NEntity::NRenderState,
						NEntity::NSkeleton,
						NEntity::NPosition,
						NEntity::NAnimation,
						NEntity::NBoundingBoxes
					>(entity);

					skeleton.Instance.SetParent(
						position.Angle,
						position.Position,
						position.Scale
					);

					NCompressedMatrix viewModel;
					viewModel.Set(
						position.Angle,
						position.Position,
						position.Scale
					);

					const auto model = attachment.Base;
					model->PlayAnimation(animation.CurrentAction, animation.PriorAction, animation.CurrentFrame, animation.PriorFrame, model->GetPlaySpeed() * updateTime);

					auto &bbox = boundingBox.Calculated;
					if (model->HasMeshes() && model->HasGlobalBBox())
					{
						const auto &globalBBox = model->GetGlobalBBox();
						bbox.Min = Transform(globalBBox.Min, viewModel);
						bbox.Max = Transform(globalBBox.Max, viewModel);
					}
					else
					{
						bbox.Min = Transform(boundingBox.Configured.Min, viewModel);
						bbox.Max = Transform(boundingBox.Configured.Max, viewModel);
					}
					bbox.Order();
					
					/* If we have parts to be processed then we animate the skeleton before checking if the object is visible */
					if (attachment.Parts.size() > 0)
					{
						skeleton.Instance.Animate(
							model,
							{
								.Action = animation.CurrentAction,
								.Frame = animation.CurrentFrame,
							},
							{
								.Action = animation.PriorAction,
								.Frame = animation.PriorFrame,
							},
							glm::vec3(0.0f, 0.0f, 0.0f)
						);
					}

					for (auto &[type, part] : attachment.Parts)
					{
						const auto model = part.Model;
						const auto bone = part.IsLinked ? part.Link.Bone : 0;

						if (part.IsLinked == true)
						{
							auto &link = part.Link;
							auto &animation = link.Animation;
							model->PlayAnimation(animation.CurrentAction, animation.PriorAction, animation.CurrentFrame, animation.PriorFrame, model->GetPlaySpeed() * updateTime);
						}

						if (model->HasMeshes() && model->HasGlobalBBox())
						{
							const auto viewModel = skeleton.Instance.GetBone(bone);

							NBoundingBox tmpBBox;
							const auto &globalBBox = model->GetGlobalBBox();
							tmpBBox.Min = Transform(globalBBox.Min, viewModel);
							tmpBBox.Max = Transform(globalBBox.Max, viewModel);
							tmpBBox.Order();

							bbox.Min = glm::min(bbox.Min, tmpBBox.Min);
							bbox.Max = glm::max(bbox.Max, tmpBBox.Max);
						}
					}

					renderState.Flags.Visible = frustum->IsBoxVisible(bbox.Min, bbox.Max);

					if (!renderState.Flags.Visible) return;

					environment->CalculateLight(position, light, renderState);

					/* If we have parts to be processed then we animate the skeleton before checking if the object is visible */
					if (attachment.Parts.size() == 0)
					{
						skeleton.Instance.Animate(
							model,
							{
								.Action = animation.CurrentAction,
								.Frame = animation.CurrentFrame,
							},
							{
								.Action = animation.PriorAction,
								.Frame = animation.PriorFrame,
							},
							glm::vec3(0.0f, 0.0f, 0.0f)
						);
					}
					skeleton.SkeletonOffset = skeleton.Instance.Upload();

					for (auto &[type, part] : attachment.Parts)
					{
						if (part.IsLinked == false) continue;
						const auto model = part.Model;
						auto &link = part.Link;
						auto &animation = link.Animation;
						auto &partSkeleton = link.Skeleton;
						const auto &renderAnimation = link.RenderAnimation;

						const auto boneMatrix = skeleton.Instance.GetBone(link.Bone);
						NCompressedMatrix transformMatrix{
							.Rotation = glm::quat(glm::radians(renderAnimation.Angle)),
							.Position = renderAnimation.Position,
							.Scale = renderAnimation.Scale
						};
						MixBones(boneMatrix, transformMatrix);

						partSkeleton.SetParent(transformMatrix);
						partSkeleton.Animate(
							model,
							{
								.Action = animation.CurrentAction,
								.Frame = animation.CurrentFrame,
							},
							{
								.Action = animation.PriorAction,
								.Frame = animation.PriorFrame,
							},
							glm::vec3(0.0f, 0.0f, 0.0f)
						);
						link.SkeletonOffset = partSkeleton.Upload();
					}
				}
			)
		)
	);
}

void NObjects::Render()
{
	const auto view = Registry.view<NEntity::NRenderable, NEntity::NAttachment, NEntity::NRenderState, NEntity::NSkeleton>();
	for (auto [entity, attachment, renderState, skeleton] : view.each())
	{
		if (!renderState.Flags.Visible) continue;
		if (skeleton.SkeletonOffset == NInvalidUInt32) continue;

		MURenderState::AttachTexture(TextureAttachment::Skin, attachment.Skin);

		const NRenderConfig config = {
			.BoneOffset = skeleton.SkeletonOffset,
			.BodyOrigin = glm::vec3(0.0f, 0.0f, 0.0f),
			.BodyScale = 1.0f,
			.EnableLight = renderState.Flags.LightEnable,
			.BodyLight = renderState.BodyLight,
		};
		MUModelRenderer::RenderBody(skeleton.Instance, attachment.Base, config);

		for (auto &[type, part] : attachment.Parts)
		{
			const auto model = part.Model;
			auto &skeletonInstance = part.IsLinked ? part.Link.Skeleton : skeleton.Instance;

			const NRenderConfig config = {
				.BoneOffset = part.IsLinked ? part.Link.SkeletonOffset : skeleton.SkeletonOffset,
				.BodyOrigin = glm::vec3(0.0f, 0.0f, 0.0f),
				.BodyScale = 1.0f,
				.EnableLight = renderState.Flags.LightEnable,
				.BodyLight = renderState.BodyLight,
			};
			MUModelRenderer::RenderBody(skeletonInstance, part.Model, config);
		}

		MURenderState::DetachTexture(TextureAttachment::Skin);
	}

#if RENDER_BBOX
	const auto bboxView = Objects.view<NEntity::NRenderable, NEntity::NRenderState, NEntity::NBoundingBox>();
	for (auto [entity, renderState, boundingBox] : bboxView.each())
	{
		if (!renderState.Flags.Visible) continue;
		MUBBoxRenderer::Render(boundingBox.Calculated);
	}
#endif
}

void NObjects::Clear()
{
	Registry.clear();
}

const entt::entity NObjects::Add(
	const TObject::Settings object
)
{
	auto &registry = Registry;
	const auto entity = registry.create();
	registry.emplace<NEntity::NAttachment>(
		entity,
		NEntity::NAttachment{
			.Base = object.Model,
		}
	);

	if (object.Renderable)
	{
		registry.emplace<NEntity::NRenderable>(entity);
	}

	if (object.Interactive)
	{
		registry.emplace<NEntity::NInteractive>(entity);
	}

	NEntity::NLightSettings lightSettings;
	switch (object.Light.Mode)
	{
	case EntityLightMode::Terrain:
		{
			lightSettings.Terrain.Color = object.Light.Color;
			lightSettings.Terrain.Intensity = object.Light.LightIntensity;
			lightSettings.Terrain.PrimaryLight = object.Light.PrimaryLight;
		}
		break;

	case EntityLightMode::Fixed:
		{
			lightSettings.Fixed.Color = object.Light.Color;
		}
		break;

	case EntityLightMode::SinWorldTime:
		{
			lightSettings.WorldTime.TimeMultiplier = object.Light.TimeMultiplier;
			lightSettings.WorldTime.Multiplier = object.Light.LightMultiplier;
			lightSettings.WorldTime.Add = object.Light.LightAdd;
		}
		break;
	}

	registry.emplace<NEntity::NLight>(
		entity,
		object.Light.Mode,
		lightSettings
	);

	registry.emplace<NEntity::NRenderState>(
		entity,
		NEntity::NRenderFlags{
			.Visible = false,
			.LightEnable = object.LightEnable,
		},
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
		);

	NSkeletonInstance instance;
	instance.SetParent(
		object.Angle,
		glm::vec3(0.0f, 0.0f, 0.0f),
		1.0f
	);

	registry.emplace<NEntity::NSkeleton>(
		entity,
		NEntity::NSkeleton{
			.SkeletonOffset = NInvalidUInt32,
			.Instance = instance,
		}
	);

	registry.emplace<NEntity::NBoundingBoxes>(
		entity,
		NEntity::NBoundingBoxes{
			.Configured = NBoundingBox{
				.Min = object.BBoxMin,
				.Max = object.BBoxMax,
			}
		}
	);

	registry.emplace<NEntity::NPosition>(
		entity,
		NEntity::NPosition{
			.Position = object.Position,
			.Angle = object.Angle,
			.Scale = object.Scale,
		}
	);

	registry.emplace<NEntity::NAnimation>(
		entity,
		NEntity::NAnimation{}
	);

	return entity;
}

void NObjects::Remove(const entt::entity entity)
{
	auto &registry = Registry;
	registry.destroy(entity);
}