#include "azpch.h"
#include "Application.h"
#include "Azer/Core/Log.h"

namespace Azer {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		AZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create();
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		AZ_CORE_TRACE("{0}",e.ToString());
	}
}