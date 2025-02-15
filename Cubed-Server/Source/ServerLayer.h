#pragma once
#include <glm/vec2.hpp>

#include "HeadlessConsole.h"
#include "Walnut/Layer.h"
#include "Walnut/Networking/Server.h"

namespace Cubed
{
    class ServerLayer : public Walnut::Layer
    {
    public:
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float ts) override;
        void OnUIRender() override;   
    private:
        void OnConsoleMessage(std::string_view message);

        void OnClientConnected(const Walnut::ClientInfo& clientInfo);
        void OnClientDisconnected(const Walnut::ClientInfo& clientInfo);
        void OnDataRecieved(const Walnut::ClientInfo& clientInfo, const Walnut::Buffer buffer);
        
    private:
        HeadlessConsole m_Console;
        Walnut::Server m_Server { 8192 };

        struct PlayerData
        {
            glm::vec2 Position;
            glm::vec2 Velocity;
        };

        // Gonna swap this out to a concurrent, lock-free queue in the future
        std::mutex m_PlayerDataMutex;        
        std::map<uint32_t, PlayerData> m_PlayerData;
    };
}
