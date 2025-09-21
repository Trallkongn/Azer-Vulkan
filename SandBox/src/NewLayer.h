#include <Azer.h>

using namespace Azer;

class NewLayer : public Layer
{
public:
	NewLayer(const std::string& name);
	~NewLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(TimeStep delta) override;
	virtual void OnRender() override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Event& event) override;
private:

};