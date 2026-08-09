#pragma once
#pragma once
#include "Singleton.h"
#include "PlayerSession.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <queue>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")

struct SPonpokoPacket
{
    int nTick = 0;
    int nActionCount = 0;
    EInputType Actions[(int)EInputType::Max]{};
};

enum class ENetworkState
{
    Idle,
    WaitingForClient,
    Connecting,
    Connected,
    Failed
};

class CNetworkManager : public ISingleton<CNetworkManager>
{
public:
    void Initialize();
    void ShutDown();

    bool StartHost(unsigned short InPort);
    bool ConnectToHost(const std::string& InIP, unsigned short InPort);

    void Update();

    bool IsConnected() const { return m_eState == ENetworkState::Connected; }
    bool IsFailed() const { return m_eState == ENetworkState::Failed; }

    void SendInput(const SPonpokoPacket& InPacket);
    bool TryPopRemoteInput(SPonpokoPacket& OutPacket);

    void Disconnect();

private:
    void SetNonBlocking(SOCKET InSocket);
    void PollAccept();
    void PollConnect();
    void PollRecv();

private:
    ENetworkState m_eState = ENetworkState::Idle;
    bool m_bIsHost = false;

    SOCKET m_ListenSocket = INVALID_SOCKET;
    SOCKET m_ConnectedSocket = INVALID_SOCKET;

    std::vector<char> m_RecvBuffer;
    std::queue<SPonpokoPacket> m_RemoteInputQueue;
};