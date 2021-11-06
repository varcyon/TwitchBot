#include <Winsock2.h>
#include <Windows.h>
#include <vector>
#include <string>
namespace TwitchBot {
	using namespace std;

	constexpr int BufferLength = 512;

	class TwitchSocket {
	private:
		vector<char> buffer = vector<char>(BufferLength);
		SOCKET connection;
	public:
		int send(const string& information);
		void SendToChannel(const string& channel, const string& user, const string& message);
		void SendToChannel(const string& channel, const string& message);
		string receive();
		TwitchSocket(string oAuthToken, string username, SOCKET connection);
		TwitchSocket(const TwitchSocket&) = delete;
		TwitchSocket& operator=(const TwitchSocket&) = delete;
	};
}