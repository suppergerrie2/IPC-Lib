#include "ClientSocket.h"


/// <summary>
/// Constructor of ClientSocket
/// </summary>
/// <param name="ip">Ip address of server</param>
/// <param name="port">Port of server</param>
ClientSocket::ClientSocket(PCWSTR ip, int port)
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
	}

	if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		std::cin.get();
		// environment exit? exit(-1);
	}

	InetPtonW(AF_INET, ip, &m_server.sin_addr.s_addr);
	m_server.sin_family = AF_INET;
	m_server.sin_port = htons(port);

	auto c = connect(m_socket, (struct sockaddr*)&m_server, sizeof(m_server));
	if (c < 0)
	{
		puts("connect error");
		printf("%d", c);
		std::cin.get();
		// environment exit? exit(-1);
	}

	m_disconnected = false;
}

/// <summary>
/// Waits for data on the socket
/// </summary>
/// <param name="data">Expected data format that can be deseralized</param>
void ClientSocket::WaitForData(IDeserializable& data) const
{
	char message[MESSAGEBYTESIZE];
	int size = recv(m_socket, message, MESSAGEBYTESIZE, 0);
	if (size == SOCKET_ERROR)
	{
		printf("Failed to receive message");
		std::cin.get();
		// environment exit? exit(-1);
	}
	data.Deserialize(message, size);
}

/// <summary>
/// Sends data over the socket to the server
/// </summary>
/// <param name="data">The data to be send that can be serialized</param>
void ClientSocket::SendData(ISerializable& data) const
{
	char message[MESSAGEBYTESIZE]; //chars are bytes, this is put on the stack
	int messageSize = 0;
	data.Serialize(message, messageSize); // needs to end with '\0'
	send(m_socket, message, messageSize, 0);
}

/// <summary>
/// Disconnects from server
/// </summary>
void ClientSocket::Disconnect()
{
	closesocket(m_socket);
	WSACleanup();
	m_disconnected = true;
}

/// <summary>
/// Deconstructor, if it is not disconnected it disconnects from the server
/// </summary>
ClientSocket::~ClientSocket()
{
	if (m_disconnected) return;
	Disconnect();
}
