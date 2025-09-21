#include "Azer.h"
#include "Azer/Core/EntryPoint.h"

#include "NewLayer.h"

class SandBox : public Azer::Application
{
public:
	SandBox()
	{
		PushLayer(new NewLayer("NewLayer"));
	}

	~SandBox()
	{

	}
};

Azer::Application* Azer::CreateApplication()
{
	return new SandBox();
}