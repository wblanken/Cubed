#include "ClientLayer.h"

#include "Walnut/Input/Input.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

namespace Cubed
{
    static void DrawRect(glm::vec2 position, glm::vec2 size, uint32_t color)
    {
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        ImVec2 min = ImGui::GetWindowPos() + ImVec2(position.x, position.y);
        ImVec2 max = min + ImVec2(size.x, size.y);
	
        drawList->AddRectFilled(min, max, 0xffff00ff);
    }
    
    void ClientLayer::OnAttach()
    {
        m_Client.SetDataReceivedCallback([this](const Walnut::Buffer buffer){ OnDataRecieved(buffer); });
    }

    void ClientLayer::OnDetach()
    {
    }

    void ClientLayer::OnUpdate(float ts)
    {
        if (m_Client.GetConnectionStatus() != Walnut::Client::ConnectionStatus::Connected) return;
        
        glm::vec2 dir{0.0f, 0.0f};
        if (Walnut::Input::IsKeyDown(Walnut::KeyCode::W))
            dir.y = -1;
        else if (Walnut::Input::IsKeyDown(Walnut::KeyCode::S))
            dir.y = 1;        
        if (Walnut::Input::IsKeyDown(Walnut::KeyCode::A))
            dir.x = -1;
        else if (Walnut::Input::IsKeyDown(Walnut::KeyCode::D))
            dir.x = 1;

        if (glm::length(dir) > 0.0f)
        {
            constexpr float speed = 50.0f;
            dir = glm::normalize(dir);
            m_PlayerVelocity += dir * speed;
        }

        m_PlayerPosition += m_PlayerVelocity * ts;
        m_PlayerVelocity = glm::mix(m_PlayerVelocity, glm::vec2(0.0f), 5.0f * ts);        
    }

    void ClientLayer::OnUIRender()
    {
        Walnut::Client::ConnectionStatus connectionStatus = m_Client.GetConnectionStatus();
        if (connectionStatus == Walnut::Client::ConnectionStatus::Connected)
        {
            DrawRect(m_PlayerPosition, {50.0f, 50.0f}, 0xffff00ff);    
        }
        else
        {
            bool readOnly = connectionStatus != Walnut::Client::ConnectionStatus::Disconnected;
            ImGui::Begin("Connect to Server");

            ImGui::InputText("Server Address", &m_ServerAddress, readOnly ? ImGuiInputTextFlags_ReadOnly : 0);
            
            if (readOnly)
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            
            if (ImGui::Button("Connect"))
            {
                m_Client.ConnectToServer(m_ServerAddress);
            }
            
            if (readOnly)
                ImGui::PopItemFlag();
            
            ImGui::End();
        }
        
        ImGui::ShowDemoWindow();
    }

    void ClientLayer::OnDataRecieved(const Walnut::Buffer buffer)
    {
    }
}
