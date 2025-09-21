#pragma once

#include "azpch.h"
#include "Azer/Events/Event.h"
#include "Azer/Events/ApplicationEvent.h"
#include "Azer/Core/Window.h"
#include "Azer/Core/LayerStack.h"

namespace Azer {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline static Application& Get() { return *s_Instance; }
	private:
		bool onWindowClosed(WindowCloseEvent& e);
		bool onWindowResize(WindowResizeEvent& e);
	private:
		static Application* s_Instance;
	private:
		std::unique_ptr<Window> m_Window;
		LayerStack m_LayerStack;
		bool m_Running = true;
		bool m_Minimized = false;
		float m_LastFrameTime = 0.0f;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}