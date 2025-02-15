#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Core/Log.h"


class ExampleLayer : public Walnut::Layer
{
public:
	void OnAttach() override
	{
		
	}
	
	void OnDetach() override
	{
		
	}
	
	void OnUpdate(float ts) override
	{
		WL_INFO("Server Timestep: {}", ts);
		
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(5ms);
	}

private:
	bool m_AboutModalOpen = false;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Cubed Server";
	
	Walnut::Application* app = new Walnut::Application(spec);
	std::shared_ptr<ExampleLayer> exampleLayer = std::make_shared<ExampleLayer>();
	app->PushLayer(exampleLayer);
	return app;
}