#include "stdafx.h"
#include "Render.h"

namespace Render
{
	void EraseRenderable(std::shared_ptr<IRenderable> InRenderable)
	{
		GET_DX_MGR()->RemoveRenderTargets(InRenderable);
	}
}

