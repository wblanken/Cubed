﻿#include "ClientLayer.h"

#include "Walnut/Input/Input.h"
#include "Walnut/ImGui/ImGuiTheme.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "ServerPacket.h"
#include "misc/cpp/imgui_stdlib.h"
#include "Walnut/Serialization/BufferStream.h"

namespace Cubed
{
    static Walnut::Buffer s_ScratchBuffer;
    
    static void DrawRect(glm::vec2 position, glm::vec2 size, uint32_t color)
    {
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        ImVec2 min = ImGui::GetWindowPos() + ImVec2(position.x, position.y);
        ImVec2 max = min + ImVec2(size.x, size.y);

        drawList->AddRectFilled(min, max, 0xffff00ff);
    }

    void ClientLayer::OnAttach()
    {
        s_ScratchBuffer.Allocate(10 * 1024 * 1024); // 10MB
        
        m_Client.SetDataReceivedCallback([this](const Walnut::Buffer buffer) { OnDataRecieved(buffer); });
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

        Walnut::BufferStreamWriter stream(s_ScratchBuffer);
        stream.WriteRaw(PacketType::ClientUpdate);
        stream.WriteRaw(m_PlayerPosition);
        stream.WriteRaw(m_PlayerVelocity);
        m_Client.SendBuffer(stream.GetBuffer());        
    }

    void ClientLayer::OnUIRender()
    {
        Walnut::Client::ConnectionStatus connectionStatus = m_Client.GetConnectionStatus();
        if (connectionStatus == Walnut::Client::ConnectionStatus::Connected)
        {
            DrawRect(m_PlayerPosition, {50.0f, 50.0f}, 0xffff00ff);

            m_PlayerDataMutex.lock();
            std::map<uint32_t, PlayerData> playerData = m_PlayerData;
            m_PlayerDataMutex.unlock();

            for (auto& [id, data] : playerData)
            {
                if (id == m_PlayerID) continue;            

                DrawRect(data.Position, {50.0f, 50.0f}, 0xff0000ff);
            }
        }
        else
        {
            bool readOnly = connectionStatus != Walnut::Client::ConnectionStatus::Disconnected;
            ImGui::Begin("Connect to Server");

            ImGui::InputText("Server Address", &m_ServerAddress);
            if (connectionStatus == Walnut::Client::ConnectionStatus::FailedToConnect)
                ImGui::TextColored(ImColor(Walnut::UI::Colors::Theme::error), "Failed to connect to server");
            if (connectionStatus == Walnut::Client::ConnectionStatus::Connecting)
                ImGui::TextColored(ImColor(Walnut::UI::Colors::Theme::textDarker), "Connecting...");

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
        Walnut::BufferStreamReader stream(buffer);
        PacketType type;
        stream.ReadRaw(type);
        
        switch (type)
        {
        case PacketType::ClientConnect:
            uint32_t idFromServer;
            stream.ReadRaw<uint32_t>(idFromServer);
            // WL_INFO("We have connected! Server says our ID is {}", idFromServer);
            // WL_INFO("We say our ID is {}", m_Client.GetID());
            m_PlayerID = idFromServer; // Not thread safe, but currently doesn't matter isn't used in other threads for now.
            break;
            
        case PacketType::ClientUpdate:
            m_PlayerDataMutex.lock();
            stream.ReadMap(m_PlayerData);
            m_PlayerDataMutex.unlock();
            break;
        }
    }
}
