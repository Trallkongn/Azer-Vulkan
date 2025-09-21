#include "NewLayer.h"

NewLayer::NewLayer(const std::string& name)
	: Layer(name)
{

}

NewLayer::~NewLayer()
{

}

void NewLayer::OnAttach()
{

}

void NewLayer::OnDetach()
{

}

void NewLayer::OnUpdate(TimeStep delta)
{
	//AZ_INFO("{0}", m_DebugName);
}

void NewLayer::OnRender()
{

}

void NewLayer::OnImGuiRender()
{

}

void NewLayer::OnEvent(Event& event)
{

}
