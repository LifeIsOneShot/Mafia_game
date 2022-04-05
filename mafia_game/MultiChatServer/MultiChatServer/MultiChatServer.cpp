#include <iostream>
#include <winsock2.h>
#include <thread>
#include <ctime>
#include <string>
#include <algorithm>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)

#define PACKET_SIZE 1024

WSADATA wsa;
SOCKET soc, * client_sock;
SOCKADDR_IN* client;
int* client_size, MAX;

enum job
{
	LogOut = 0,
	Citizen,
	Mafia,
	Doctor,
	Police
};

int voted[10] = { 0 }; // 투표횟수
int vote_member[10] = { 0 }; // 투표대상

job mf_table[10] = { LogOut }; // 유저 정보
int user = 0;
int gameStart = 0;
int vote = -1;
int die = -1;
int day = 0;

void receive_data(SOCKET& s, int client_num) {
	char* buf = new char[PACKET_SIZE];

	while (1) {
		ZeroMemory(buf, PACKET_SIZE);
		if (recv(s, buf, PACKET_SIZE, 0) == -1) break;
		if (gameStart == 1 && (strcmp(buf, "I'm Dead") == 0))
			break;

		cout << buf << endl;
		if ((user < MAX) && (strcmp(buf, "ready") == 0))
			user += 1;

		cout << "\nClient #" << client_num << " << " << buf << "\n보낼 데이터를 입력 >> ";

		char a = (char)(client_num + 48);
		char* address = (buf - 1);
		*address = a;

		if (vote < 0) {
			for (int i = 0; i < MAX; i++) {
				if (mf_table[i] != LogOut)
					send(client_sock[i], address, 1 + strlen(buf), 0);
			}
		}
		else
		{
			if (voted[client_num] >= 1)
				continue;
			voted[client_num] += 1;
			vote += 1;
			vote_member[buf[0] - '0'] += 1;

			for (int i = 0; i < MAX; i++)
				cout << voted[i] << ", ";
			cout << endl;
			for (int i = 0; i < MAX; i++)
				cout << vote_member[i] << ", ";
			cout << endl;
		}

	}
	while (1);
	delete[] buf;
	return;
}

void accpetclients() {
	char client_num[10];
	for (int n = 0; n < MAX; n++) {
		client_size[n] = sizeof(client[n]);
		client_sock[n] = accept(soc, (SOCKADDR*)&client[n], &client_size[n]);

		if (client_sock[n] == INVALID_SOCKET) {
			cout << "accept error";
			closesocket(client_sock[n]);
			closesocket(soc);
			WSACleanup();
			return;
		}

		cout << "Client #" << n << " Joined!" << "\n보낼 데이터를 입력 >> ";
		ZeroMemory(client_num, sizeof(client_num));
		itoa(n, client_num, 10);
		send(client_sock[n], client_num, strlen(client_num), 0);
		thread(receive_data, ref(client_sock[n]), n).detach();
	}
	return;
}

void Start() {

	gameStart = 1;
	char game_msg[] = "game start!!\n";

	int mafia_num = 0;
	int doctor_num = 0;
	int police_num = 0;
	while (mafia_num == doctor_num || mafia_num == police_num || police_num == doctor_num)
	{
		mafia_num = rand() % MAX;
		doctor_num = rand() % MAX;
		police_num = rand() % MAX;
	}
	for (int i = 0; i < MAX; i++)
	{
		send(client_sock[i], game_msg, strlen(game_msg), 0);

		if (i == mafia_num)
		{
			send(client_sock[i], "mafia", strlen("mafia"), 0);  mf_table[i] = Mafia;
		}
		else if (i == doctor_num)
		{
			send(client_sock[i], "doctor", strlen("doctor"), 0); mf_table[i] = Doctor;
		}
		else if (i == police_num)
		{
			send(client_sock[i], "police", strlen("police"), 0); mf_table[i] = Police;
		}
		else
		{
			send(client_sock[i], "citizen", strlen("citizen"), 0); mf_table[i] = Citizen;
		}
	}
}

void Morning() {
	cout << "아침이 밝았습니다! day : " << day + 1 << endl;
	day++;
	if (day > 1) {
		die = *max_element(vote_member, vote_member + 10);
		cout << " 최다 득표 수 : " << die << endl;
		if (die > 1)
		{
			for (int i = 0; i < 10; i++)
			{
				cout << vote_member[i] << ",  ";
				if (mf_table[i] != LogOut && vote_member[i] == die)
				{
					cout << "User No." << i << " Died" << endl;
					send(client_sock[i], "Your Died!", strlen("Your Died!"), 0);

					char buf[PACKET_SIZE];

					if (mf_table[i] == Citizen)
						strcpy(buf, "사망자는 시민 입니다.");
					else if (mf_table[i] == Mafia)
						strcpy(buf, "사망자는 마피아 입니다.");
					else if (mf_table[i] == Doctor)
						strcpy(buf, "사망자는 의사 입니다.");
					else if (mf_table[i] == Police)
						strcpy(buf, "사망자는 경찰 입니다.");

					mf_table[i] = LogOut;

					int Mafia_count = 0;

					for (int j = 0; j < MAX; j++)
					{
						if (mf_table[j] == Mafia)
							Mafia_count++;
					}
					if (Mafia_count == 0) {
						gameStart = 2; return;
					}
					else if (Mafia_count == user) {
						gameStart = 3; return;
					}

					for (int j = 0; j < MAX; j++) {
						if (mf_table[j] != LogOut && i != j)
							send(client_sock[j], buf, strlen(buf), 0);
					}
					
					closesocket(client_sock[i]);
					user -= 1;
				}
			}
			cout << endl;
		}
	}
	Sleep(5000);
} 

void Night() {
	if(gameStart > 1)
		return;
	cout << "밤이 되었습니다!" << endl;
	//Sleep(30000);
	int count = 0;
	vote = 0;
	for (int i = 0; i < 10; i++)
	{
		voted[i] = 0; vote_member[i] = 0;
	}
	cout << "투표해주세요" << endl;
	while (vote != user)
	{
		Sleep(1000);
		cout << "Vote : " << vote << endl; 
		count++;
		cout << count << endl;
		if (count > 10) break;
	}
	vote = -1;

}


void openSocket(int PORT) {
	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
		cout << "WSA Error" << endl;
		return;
	}

	soc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (soc == INVALID_SOCKET) {
		cout << "socket Error" << endl;
		closesocket(soc);
		WSACleanup();
		return;
	}

	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(soc, (SOCKADDR*)&addr, sizeof(addr))) {
		cout << "bind error";
		closesocket(soc);
		WSACleanup();
		return;
	}

	if (listen(soc, SOMAXCONN)) {
		cout << "listen error";
		closesocket(soc);
		WSACleanup();
		return;
	}

	thread(accpetclients).detach();

	char msg[PACKET_SIZE], sendnum[PACKET_SIZE];

	

	srand((unsigned int)time(NULL));

	while (gameStart < 2) {
		Sleep(1000);
		if (user == MAX && gameStart == 0)
		{
			cout << "start" << endl;
			Start();
			cout << "Game Start!!\n" << endl;
		}
		else if (gameStart == 1)
		{
			Morning();
			Night();
		}
	}

	if (gameStart == 2)
	{
		for (int i = 0; i < MAX; i++) {
			if (mf_table[i] != LogOut)
				send(client_sock[i], "시민 승리!!", strlen("시민 승리!!"), 0);
		}
	}
	else if (gameStart == 3)
	{
		for (int i = 0; i < MAX; i++) {
			if (mf_table[i] != LogOut)
				send(client_sock[i], "마피아 승리!!", strlen("마피아 승리!!"), 0);
		}
	}

	for (int i = 0; i < MAX; i++) closesocket(client_sock[i]);
	closesocket(soc);
	WSACleanup();

	return;
}


int main() {
	int PORT;
	cout << "포트설정 >> ";
	//cin >> PORT;
	PORT = 80;
	cout << "클라이언트 Max Num >> ";
	//cin >> MAX;
	MAX = 4;

	client_sock = new SOCKET[MAX];
	client = new SOCKADDR_IN[MAX];
	client_size = new int[MAX];

	ZeroMemory(client_sock, sizeof(client_sock));
	ZeroMemory(client, sizeof(client));
	ZeroMemory(client_size, sizeof(client_size));

	openSocket(PORT);

	delete[] client_sock, client, client_size;
	return 0;
}