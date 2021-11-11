#include "PCH.h"
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include "Parser.h"
#include "TwichSocket.h"
#include <sqlite3.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Winmm.lib")

using namespace std;
using namespace TwitchBot;

//GLOBAL VARIABLES
map<string, string> commands;

//Methods
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
		return 1;
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
		return 1;
	}

	freeaddrinfo(result);

	if (ConnectionSocket == INVALID_SOCKET) {
		cout << "Cannot connect to server" << endl;
		WSACleanup();
		return 1;
	}
	return 0;
	// End socket
}

vector<string> ParseMsgForArgs( string& args) {
	vector<string> results;
	args.erase(remove(args.begin(), args.end(), '\r'), args.end());
	args.erase(remove(args.begin(), args.end(), '\n'), args.end());
	boost:algorithm:split(results, args, boost::algorithm::is_any_of(" "));
	return results;
}

bool isModerator(  UserAttributes& attributes) {
	if (attributes["display-name"] == "Varcy0n") {
		return true;
	}
	auto m = attributes["user-type"];
	m.erase(remove(m.begin(), m.end(), ' '), m.end());
	if (m == "mod") {
		return true;
	}
	return false;
}

bool bot_command(const string& commandName, const string& command,  vector<string>* argsList) {
	size_t split = commandName.length() + 1;
	if (command.substr(0, split) == commandName + " ") {
		string argString = command.substr(split);
		*argsList = ParseMsgForArgs((argString));
		return true;
	}
	return false;
}

void CreateDB(const char* dir) {
	
	sqlite3* DB;
	int exit = 0;
	exit = sqlite3_open(dir, &DB);

	if (exit) {
		std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
	}
	else
		std::cout << "Opened Database Successfully!" << std::endl;
	sqlite3_close(DB);
}

void CreateTable(const char* dir) {
	sqlite3* DB;
	std::string sql = "CREATE TABLE IF NOT EXISTS COMMANDS("
		"ID INT PRIMARY KEY NOT NULL, "
		"commandName TEXT NOT NULL, "
		"commandResponse TEXT NOT NULL); ";

	int exit = 0;
	exit = sqlite3_open(dir, &DB);
	char* messaggeError;
	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

	if (exit != SQLITE_OK) {
		std::cerr << "Error Create Table" << std::endl;
		sqlite3_free(messaggeError);
	}
	else
		std::cout << "Table created Successfully" << std::endl;
	sqlite3_close(DB);
}
static int SelectCommandscallback(void* data, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++)
	{
		cout << azColName[i] << ": " << argv[i] << endl;
	}
	cout << endl;
	commands[argv[1]] = argv[2];
	return 0;
}
void SelectCommands(const char* dir) {
	sqlite3* DB;
	int exit = 0;
	exit = sqlite3_open(dir, &DB);

	string sql("SELECT * FROM COMMANDS;");
	if (exit) {
		std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
	}
	else
		std::cout << "Opened Database Successfully!" << std::endl;

	int rc = sqlite3_exec(DB, sql.c_str(), SelectCommandscallback, NULL, NULL);

	if (rc != SQLITE_OK)
		cerr << "Error SELECT" << endl;
	else {
		cout << "Operation OK!" << endl;
	}

	sqlite3_close(DB);
}
int main() {
	const char* dir = "Assets/TwitchBot.db";
	CreateDB(dir);
	CreateTable(dir);
	SelectCommands(dir);
		fstream authFile("TwitchAuth.txt");
		string OAuthToken;
		WSADATA wsaData;
		SOCKET ConnectionSocket = INVALID_SOCKET;
		addrinfo hints{};
		addrinfo* result;
		string botName = "varcyonsariougames";
		string channel = "varcy0n";
		if (authFile.is_open()) {
			getline(authFile, OAuthToken, '\n');
			authFile.close();
		}
		//Creates the web socket
		CreateWebSocket(wsaData, hints, result, ConnectionSocket);

		//Connects to the bot to Twitch
		TwitchBot::TwitchSocket twitch(OAuthToken, botName, ConnectionSocket);

		//Joins the channel
		twitch.send("JOIN #"+channel+"\r\n");
		twitch.send("CAP REQ :twitch.tv/tags\r\n");

		//Plays this sound once its connected
		PlaySound("Sounds/borg_computer_beep.wav", NULL, SND_FILENAME);
		cout << "Bot connected!" << endl;
		
		chrono::system_clock clock;
		chrono::time_point<chrono::system_clock> lastCommand;
		lastCommand = clock.now() - chrono::seconds(30); /// 30 delay

		auto sendMsg = twitch.SendToChannel(channel);
		sendMsg("Resistance is futile!");

		bool running = true;

		vector<string> commandArgs;

		while (running)
		{
			string receive = twitch.receive();
			TwitchMessage twitchMessage =  ParseTwitchMessage(receive);
				//checks if the message was empty and if not then checks if the first char is a "!" 
				//signaling that its a command
				//runs through the commands map for match
				//sends the message stored in commands if the match was found
				cout << twitchMessage.userName << ": " << twitchMessage.body << endl;
				if (twitchMessage.body == "!Computer" && isModerator(twitchMessage.attributes)) {
					if (clock.now() - lastCommand >= chrono::seconds(30)) {
						PlaySound("Sounds/borg_computer_beep.wav", NULL, SND_FILENAME);
						lastCommand = clock.now();
					}
					else {
						sendMsg( "Command is on cool down!");
					}
				}

				if (twitchMessage.body == "!Hello") {
					sendMsg( "Hello " + twitchMessage.userName + "!");
				}

				if (bot_command("!so", twitchMessage.body, &commandArgs)) {
						sendMsg("Shout out to our boy " + commandArgs[0]);
				}

				if (isModerator(twitchMessage.attributes) && bot_command("!addcom", twitchMessage.body, &commandArgs)) {
					string commandName = commandArgs[0];
					string commandBody = twitchMessage.body.substr(8 + commandName.length() + 1);
					commands[commandName] = commandBody;
					auto command = commands.find(commandName);
					if (command != commands.end()) {
						sendMsg("Command " + commandName + " has been added!");
						//cout << command->first << endl << command->second << endl;
						for (const auto& command : commands) {
							cout << command.first << endl << command.second << endl;
						}
					}
				}
				if (!twitchMessage.body.empty() && twitchMessage.body.front() == '!') {
					string commandNameTemp = twitchMessage.body.substr(1);
					commandNameTemp.erase(remove(commandNameTemp.begin(), commandNameTemp.end(), '\r'), commandNameTemp.end());
					commandNameTemp.erase(remove(commandNameTemp.begin(), commandNameTemp.end(), '\n'), commandNameTemp.end());
					cout << commandNameTemp << endl;
					auto command = commands.find(commandNameTemp);
					if (command != commands.end()) {
						sendMsg(command->second);
					}
					else {
						cout << "Command: " + twitchMessage.body.substr(1) + " not found!" << endl;
					}
				}


		}
		return 0;
	}




