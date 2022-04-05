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
		ZeroMemory(buffer, PACKET_SIZE); // 버퍼 비우기
		recv(s, buffer, PACKET_SIZE, 0); // 메세지 수신

		if (WSAGetLastError()) 
			return;
		if (strcmp(buffer, "Your Died!") == 0)
		{
			cout << buffer << endl;
			send(s, "I'm Dead", strlen("I'm Dead"), 0);
			closesocket(s);	// 소캣 종료
			WSACleanup();
			exit(0);
		}
		cout << "\n * [Server] >> " << buffer << "\n보낼 데이터를 입력 >> ";
	}
}

int main() {
	char IP[30];
	int PORT;
	cout << "아이피주소 설정 >> ";
	// cin >> IP;
	strcpy(IP,"127.0.0.1");
	cout << "포트 설정 >> ";
	//cin >> PORT;
	PORT = 80;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {	// 윈속 초기화 (WS2_32.DLL)
		cout << "WSA Error" << endl;
		WSACleanup();
		return 0;
	}

	SOCKET soc;
	soc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 소캣 생성(IPv4, TCP)
	if (soc == INVALID_SOCKET) {
		cout << "Socket Error" << endl;
		closesocket(soc);
		WSACleanup();
		return 0;
	}

	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET;	// IPv4
	addr.sin_port = htons(PORT);	// 포트번호
	addr.sin_addr.s_addr = inet_addr(IP);	// IP주소

	while (connect(soc, (SOCKADDR*)&addr, sizeof(addr)));	// addr의 주소와 연결
	char buffer[PACKET_SIZE];

	recv(soc, buffer, PACKET_SIZE, 0);	// 버퍼에 응답받은 메세지 저장(연결 시작 응답)
	sprintf(buffer, " * [%d] %s::%d", atoi(buffer), inet_ntoa(addr.sin_addr), ntohs(addr.sin_port)); // 연결된 Server 정보 출력
	thread(receive_data, ref(soc)).detach(); // 메인스레도와 동시에 응답을 대기할수 있는 스레드 생성
	while (!WSAGetLastError()) {
		cout << "보낼 데이터를 입력하세요. >> ";
		cin >> buffer;	// 메세지 입력

		send(soc, buffer, strlen(buffer), 0);	// 메세지 전송
	}

	closesocket(soc);	// 소캣 종료
	WSACleanup();

	return 0;
}