#include <vector>
#include <iostream>
#include "TwichSocket.h"

namespace TwitchBot {

		int TwitchSocket::send(const string& information) {
			return ::send(connection, information.c_str(), information.length(), NULL);
		}
		void TwitchSocket::SendToMessageChannel(const string& channel, const string& user , const string& message) {
				send("PRIVMSG #" + channel + " :" + message+ "\r\n");
		}
		void TwitchSocket::SendToMessageChannel(const string& channel,  const string& message) {
			send("PRIVMSG #" + channel + " :" + message + "\r\n");
		}
		function<int(const string&)> TwitchSocket::SendToChannel(const string& channel) {
			return [=](const string& message) {
				return this->send("PRIVMSG #" + channel + " :" + message + "\r\n");
			};
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
			receive();

		}
	};
