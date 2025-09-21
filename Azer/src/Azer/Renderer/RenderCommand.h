#pragma once
#include "RendererAPI.h"

namespace Azer
{
	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}
	private:
		static RendererAPI* s_RendererAPI;
	};
}