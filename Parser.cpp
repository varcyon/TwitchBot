#include "PCH.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>

#include "Parser.h"


	using namespace std;
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;
	namespace fusion = boost::fusion;
	string vecToString(const vector<char>& vec) {
		return string(vec.begin(), vec.end());
	}

	TwitchMessage ParseTwitchMessage(const string& message) {
		TwitchMessage msg;

		vector<string> results(3);
		vector<string> temp;
		boost:algorithm:split(results, message, boost::algorithm::is_any_of(":"));
		boost::algorithm::split(temp, results[1], boost::algorithm::is_any_of("#"));
		results[1] = temp[1];
		msg.attributes = AttributesFor(results[0]);
		msg.userName = msg.attributes["display-name"];
		msg.channel = results[1];

		msg.body = results[2];
		msg.body.erase(remove(msg.body.begin(), msg.body.end(), '\r'), msg.body.end());
		msg.body.erase(remove(msg.body.begin(), msg.body.end(), '\n'), msg.body.end());
		return msg;
	}

	UserAttributes AttributesFor(const string& attrList)
	{
		UserAttributes results;
		vector<string> keyValuePairs;
		boost::algorithm::split(keyValuePairs, attrList, boost::algorithm::is_any_of(";"));

		for (const auto& kvp : keyValuePairs) {
			vector<string> elements;
			boost::algorithm::split(elements, kvp, boost::algorithm::is_any_of("="));
			results[elements.front()] = elements.back();
		}
		return results;
	}

