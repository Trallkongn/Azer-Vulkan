#pragma once

#include "RenderCommand.h"

namespace Azer {

	class Renderer
	{
	public:
		static void Init();

		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};
}