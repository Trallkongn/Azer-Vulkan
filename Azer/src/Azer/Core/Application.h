#pragma once

#include "azpch.h"
#include "Azer/Events/Event.h"
#include "Azer/Core/Window.h"

namespace Azer {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		inline static Application& Get() { return *s_Instance; }
	private:
		static Application* s_Instance;
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}