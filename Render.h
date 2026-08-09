#pragma once

#include "DXManager.h"
#include <memory>
#include <type_traits>
#include <concepts>

class IRenderable abstract
{
public:
	IRenderable() {};
	virtual ~IRenderable() {}

public:
	virtual void OnRender() = 0;
};

namespace Render
{
	template<typename T>
	concept RenderableClass = std::is_base_of_v<IRenderable, T>;

	template<RenderableClass T, typename... Args>
	std::shared_ptr<T> CreateRenderable(Args&&... args)
	{
		auto RenderObj = std::make_shared<T>(std::forward<Args>(args)...);
		GET_DX_MGR()->AddRenderTargets(RenderObj);
		return RenderObj;
	};

	void EraseRenderable(std::shared_ptr<IRenderable> InRenderable);
	
};