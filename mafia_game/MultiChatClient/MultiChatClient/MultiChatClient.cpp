#include <iostream>
#include <winsock2.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)
using namespace std;

#define PACKET_SIZE 1024

void receive_data(SOCKET& s) {
	char buffer[PACKET_SIZE];

	while(true) {
		ZeroMemory(buffer, PACKET_SIZE); // ���� ����
		recv(s, buffer, PACKET_SIZE, 0); // �޼��� ����

		if (WSAGetLastError()) 
			return;
		if (strcmp(buffer, "Your Died!") == 0)
		{
			cout << buffer << endl;
			send(s, "I'm Dead", strlen("I'm Dead"), 0);
			closesocket(s);	// ��Ĺ ����
			WSACleanup();
			exit(0);
		}
		cout << "\n * [Server] >> " << buffer << "\n���� �����͸� �Է� >> ";
	}
}

int main() {
	char IP[30];
	int PORT;
	cout << "�������ּ� ���� >> ";
	// cin >> IP;
	strcpy(IP,"127.0.0.1");
	cout << "��Ʈ ���� >> ";
	//cin >> PORT;
	PORT = 80;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {	// ���� �ʱ�ȭ (WS2_32.DLL)
		cout << "WSA Error" << endl;
		WSACleanup();
		return 0;
	}

	SOCKET soc;
	soc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // ��Ĺ ����(IPv4, TCP)
	if (soc == INVALID_SOCKET) {
		cout << "Socket Error" << endl;
		closesocket(soc);
		WSACleanup();
		return 0;
	}

	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET;	// IPv4
	addr.sin_port = htons(PORT);	// ��Ʈ��ȣ
	addr.sin_addr.s_addr = inet_addr(IP);	// IP�ּ�

	while (connect(soc, (SOCKADDR*)&addr, sizeof(addr)));	// addr�� �ּҿ� ����
	char buffer[PACKET_SIZE];

	recv(soc, buffer, PACKET_SIZE, 0);	// ���ۿ� ������� �޼��� ����(���� ���� ����)
	sprintf(buffer, " * [%d] %s::%d", atoi(buffer), inet_ntoa(addr.sin_addr), ntohs(addr.sin_port)); // ����� Server ���� ���
	thread(receive_data, ref(soc)).detach(); // ���ν������� ���ÿ� ������ ����Ҽ� �ִ� ������ ����
	while (!WSAGetLastError()) {
		cout << "���� �����͸� �Է��ϼ���. >> ";
		cin >> buffer;	// �޼��� �Է�

		send(soc, buffer, strlen(buffer), 0);	// �޼��� ����
	}

	closesocket(soc);	// ��Ĺ ����
	WSACleanup();

	return 0;
}