#include <Winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include "TwichSocket.h"
#include "Main.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Winmm.lib")
using namespace TwitchBot;

	
	int main() {
		fstream authFile("TwitchAuth.txt");
		string OAuthToken;
		WSADATA wsaData;
		SOCKET ConnectionSocket = INVALID_SOCKET;
		addrinfo hints{};
		addrinfo* result;


		if (authFile.is_open()) {
			getline(authFile, OAuthToken, '\n');
			authFile.close();
		}
		//Creates the web socket
		CreateWebSocket(wsaData, hints, result, ConnectionSocket);

		//Connects to the bot to Twitch
		TwitchBot::TwitchSocket twitch(OAuthToken, "varcyonsariougames", ConnectionSocket);

		//Joins the channel
		twitch.send("JOIN #varcy0n\r\n");
		(void)twitch.receive();

		//Plays this sound once its connected
		PlaySound("Sounds/borg_computer_beep.wav", NULL, SND_FILENAME);
		cout << "Bot connected!" << endl;

		regex re("!(.+)@.+PRIVMSG #([^\\s]+) :(.*)");
		smatch match;
		
		chrono::system_clock clock;
		chrono::time_point<chrono::system_clock> lastCommand;
		lastCommand = clock.now() - chrono::seconds(30); /// 30 delay

		if (clock.now() - lastCommand >= chrono::seconds(30)) {
			//do something
			lastCommand = clock.now();
		}

		bool running = true;
		while (running)
		{
			//if (GetAsyncKeyState('Q') & 0x8000) {
			//	running = false;
			//}
			string received = twitch.receive();
			//Parse Message
			//:varcy0n!varcy0n@varcy0n.tmi.twitch.tv PRIVMSG #varcy0n :Testing
			regex_search(received, match, re);
			string user = match[1];
			string channel = match[2];
			string message = match[3];
			if (user != "" || channel != "" || message != "") {
				cout << "Channel: " << channel << endl << "User: " << user << endl << "Message: " << message << endl;
			}
		}
		return 0;
	}

	int CreateWebSocket(WSADATA& wsaData, addrinfo& hints, addrinfo*& result, SOCKET& ConnectionSocket)
	{
		//socket stuff
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			cout << "Startup Failed" << endl;
			return 1;
		}

		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		if (getaddrinfo("irc.chat.twitch.tv", "6667", &hints, &result) != 0) {
			cout << "getaddrinfo failed: <RETURN CODE PASSED>" << endl;
		}

		ConnectionSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ConnectionSocket == INVALID_SOCKET) {
			cout << "ERROR at socket(): " << WSAGetLastError() << endl;
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		if (connect(ConnectionSocket, result->ai_addr, result->ai_addrlen) == SOCKET_ERROR) {
			closesocket(ConnectionSocket);
			ConnectionSocket = INVALID_SOCKET;
		}

		freeaddrinfo(result);

		if (ConnectionSocket == INVALID_SOCKET) {
			cout << "Cannot connect to server" << endl;
			WSACleanup();
			return 1;
		}
		// End socket
	}



