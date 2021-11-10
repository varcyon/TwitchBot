#pragma once
#include <string>
#include <map>

using namespace std;
using UserAttributes = map<string, string>;

struct  TwitchMessage
{
	UserAttributes attributes;
	string userName;
	string msgType;
	string channel;
	string body;
};
UserAttributes AttributesFor(const string& attrList);
TwitchMessage ParseTwitchMessage(const string& message);
