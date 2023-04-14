#ifndef __T_PARTICLE_BASE_H__
#define __T_PARTICLE_BASE_H__

#pragma once

#include "t_particle_config.h"
#include "t_particle_enum.h"
#include "t_particle_entity.h"
#include "t_particle_render.h"
#include "t_particle_create.h"

namespace TParticle
{
	template<typename... Other>
	using EnttViewType = entt::view<entt::get_t<Other...>>;
	typedef entt::registry EnttRegistry;
	typedef EnttViewType<TParticle::Entity::Info> EnttView;
	typedef EnttView::iterator EnttIterator;

	class Template;
	Template *GetTemplate(ParticleType type);

	class Template
	{
	public:
		virtual void Create(EnttRegistry &registry, const NParticleData &data) = 0;
		virtual EnttIterator Move(EnttRegistry &registry, EnttView &view, EnttIterator iter, EnttIterator last) = 0;
		virtual EnttIterator Action(EnttRegistry &registry, EnttView &view, EnttIterator iter, EnttIterator last) = 0;
		virtual EnttIterator Render(EnttRegistry &registry, EnttView &view, EnttIterator iter, EnttIterator last, NRenderBuffer &renderBuffer) = 0;
		virtual void RenderGroup(const NRenderGroup &renderGroup, const NRenderBuffer &renderBuffer) = 0;

	protected:
		friend Template *GetTemplate(ParticleType type);
		static std::map<ParticleType, Template *> Templates;
	};
}

#endif