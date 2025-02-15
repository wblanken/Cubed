#pragma once

#include "Walnut/Application.h"
#include "Walnut/Layer.h"

#include "Walnut/Networking/Client.h"

#include <glm/glm.hpp>

namespace Cubed
{
    class ClientLayer : public Walnut::Layer
    {
    public:
        void OnAttach() override;
        void OnDetach() override;
        
        void OnUpdate(float ts) override;        
        void OnUIRender() override;
    private:
        void OnDataRecieved(const Walnut::Buffer buffer);
        
    private:
        glm::vec2 m_PlayerPosition {50,50 };
        glm::vec2 m_PlayerVelocity {0,0 };

        Walnut::Client m_Client;
        std::string m_ServerAddress;
    };
}