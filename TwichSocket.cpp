#include <vector>
#include "TwichSocket.h"

namespace TwitchBot {

		int TwitchSocket::send(const string& information) {
			return ::send(connection, information.c_str(), information.length(), NULL);
		}

		string TwitchSocket:: receive() {
			int bytesReceived = recv(connection, buffer.data(), BufferLength, NULL);
			if (bytesReceived < 0) { return ""; }
			return string(buffer.begin(), buffer.begin() + bytesReceived);
		}

		TwitchSocket::TwitchSocket(string oAuthToken, string username, SOCKET connection) {
			this->connection = connection;
			this->buffer = vector<char>(BufferLength);
			send("PASS " + oAuthToken + "\r\n");
			send("NICK " + username + "\r\n");
			(void)receive();

		}
	};
