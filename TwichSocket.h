#include <Winsock2.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <functional>
namespace TwitchBot {
	using namespace std;

	constexpr int BufferLength = 512;

	class TwitchSocket {
	private:
		vector<char> buffer = vector<char>(BufferLength);
		SOCKET connection;
	public:
		TwitchSocket(const TwitchSocket&) = delete;
		TwitchSocket& operator=(const TwitchSocket&) = delete;
		TwitchSocket(string oAuthToken, string username, SOCKET connection);
		string receive();
		int send(const string& information);
		void SendToMessageChannel(const string& channel, const string& user, const string& message);
		void SendToMessageChannel(const string& channel, const string& message);
		function<int(const string&)> SendToChannel(const string& channel);
	};
}