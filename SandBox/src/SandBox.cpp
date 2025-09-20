#include "Azer.h"
#include "Azer/Core/EntryPoint.h"

class SandBox : public Azer::Application
{
public:
	SandBox()
	{

	}

	~SandBox()
	{

	}
};

Azer::Application* Azer::CreateApplication()
{
	return new SandBox();
}