#pragma once
#include "Azer/Core/Core.h"

namespace Azer {

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static Scope<GraphicsContext> Create(void* window);
	};
}