#include "ClientLayer.h"
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Cubed";
	spec.CustomTitlebar = false;
	spec.UseDockspace = false;

	Walnut::Application* app = new Walnut::Application(spec);	
	app->PushLayer<Cubed::ClientLayer>();
	return app;
}