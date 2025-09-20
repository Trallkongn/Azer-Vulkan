#pragma once

#ifdef AZ_PLATFORM_WINDOWS

extern Azer::Application* Azer::CreateApplication();

int main(int argc, char** argv)
{
	Azer::Log::Init();

	auto app = Azer::CreateApplication();
	app->Run();
	delete app;
}

#endif