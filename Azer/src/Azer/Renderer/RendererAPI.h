#pragma once

namespace Azer {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL, Vulkan
		};

	public:
		virtual void Init() = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}