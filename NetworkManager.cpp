#include "stdafx.h"
#include "NetworkManager.h"

void CNetworkManager::Initialize()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

void CNetworkManager::ShutDown()
{
    Disconnect();
    WSACleanup();
}

void CNetworkManager::SetNonBlocking(SOCKET InSocket)
{
    u_long mode = 1;
    ioctlsocket(InSocket, FIONBIO, &mode);
}

bool CNetworkManager::StartHost(unsigned short InPort)
{
    m_bIsHost = true;

    m_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SetNonBlocking(m_ListenSocket);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(InPort);

    if (bind(m_ListenSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        m_eState = ENetworkState::Failed;
        return false;
    }

    if (listen(m_ListenSocket, 1) == SOCKET_ERROR)
    {
        m_eState = ENetworkState::Failed;
        return false;
    }

    m_eState = ENetworkState::WaitingForClient;
    return true;
}

bool CNetworkManager::ConnectToHost(const std::string& InIP, unsigned short InPort)
{
    m_bIsHost = false;

    m_ConnectedSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SetNonBlocking(m_ConnectedSocket);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, InIP.c_str(), &addr.sin_addr);
    addr.sin_port = htons(InPort);

    connect(m_ConnectedSocket, (sockaddr*)&addr, sizeof(addr));

    m_eState = ENetworkState::Connecting;
    return true;
}

void CNetworkManager::Update()
{
    switch (m_eState)
    {
    case ENetworkState::WaitingForClient: PollAccept();  break;
    case ENetworkState::Connecting:       PollConnect(); break;
    case ENetworkState::Connected:        PollRecv();    break;
    default: break;
    }
}

void CNetworkManager::PollAccept()
{
    SOCKET client = accept(m_ListenSocket, nullptr, nullptr);
    if (client == INVALID_SOCKET)
        return;

    m_ConnectedSocket = client;
    SetNonBlocking(m_ConnectedSocket);
    m_eState = ENetworkState::Connected;
}

void CNetworkManager::PollConnect()
{
    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(m_ConnectedSocket, &writeSet);

    timeval timeout{ 0, 0 };
    int result = select(0, nullptr, &writeSet, nullptr, &timeout);

    if (result > 0 && FD_ISSET(m_ConnectedSocket, &writeSet))
        m_eState = ENetworkState::Connected;
}

void CNetworkManager::PollRecv()
{
    char buf[512];
    int nReceived = recv(m_ConnectedSocket, buf, sizeof(buf), 0);

    if (nReceived > 0)
    {
        m_RecvBuffer.insert(m_RecvBuffer.end(), buf, buf + nReceived);
    }
    else if (nReceived == 0)
    {
        m_eState = ENetworkState::Failed;
        return;
    }
    else
    {
        int nErr = WSAGetLastError();
        if (nErr != WSAEWOULDBLOCK)
        {
            m_eState = ENetworkState::Failed;
            return;
        }
    }

    while (m_RecvBuffer.size() >= sizeof(SPonpokoPacket))
    {
        SPonpokoPacket packet;
        memcpy(&packet, m_RecvBuffer.data(), sizeof(SPonpokoPacket));
        m_RecvBuffer.erase(m_RecvBuffer.begin(), m_RecvBuffer.begin() + sizeof(SPonpokoPacket));
        m_RemoteInputQueue.push(packet);
    }
}

void CNetworkManager::SendInput(const SPonpokoPacket& InPacket)
{
    if (m_eState != ENetworkState::Connected)
        return;

    send(m_ConnectedSocket, (const char*)&InPacket, sizeof(InPacket), 0);
}

bool CNetworkManager::TryPopRemoteInput(SPonpokoPacket& OutPacket)
{
    if (m_RemoteInputQueue.empty())
        return false;

    OutPacket = m_RemoteInputQueue.front();
    m_RemoteInputQueue.pop();
    return true;
}

void CNetworkManager::Disconnect()
{
    if (m_ConnectedSocket != INVALID_SOCKET)
    {
        closesocket(m_ConnectedSocket);
        m_ConnectedSocket = INVALID_SOCKET;
    }

    if (m_ListenSocket != INVALID_SOCKET)
    {
        closesocket(m_ListenSocket);
        m_ListenSocket = INVALID_SOCKET;
    }

    m_eState = ENetworkState::Idle;
    m_RecvBuffer.clear();
    std::queue<SPonpokoPacket>().swap(m_RemoteInputQueue);
}