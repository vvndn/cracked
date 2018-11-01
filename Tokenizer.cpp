#include "provided.h"
#include <string>
#include <vector>
using namespace std;

class TokenizerImpl
{
public:
    TokenizerImpl(string separators);
    vector<string> tokenize(const std::string& s) const;
private:
	vector<char> m_separatorList;
};

TokenizerImpl::TokenizerImpl(string separators)
{
	for (unsigned int i = 0; i < separators.size(); i++)
	{
		m_separatorList.push_back(separators[i]);				// push back all separators
	}
}

vector<string> TokenizerImpl::tokenize(const std::string& s) const
{
	vector<string> tokenizedWords;		// vector to hold the token strings

	string currToken = "";				// string to hold current token
	bool isSeparator = false;
	for (int i = 0; i < s.size(); i++)			// for all characters in string s
	{
		for (int j = 0; j < m_separatorList.size(); j++)		// for all separators in the separator list
		{
			if (s[i] == m_separatorList[j])		// if the character is found in the separator list, set isSeparator to true and break
			{
				isSeparator = true;
				break;
			}
		}
		if (isSeparator)			// check if a separator was found
		{
			if (currToken != "")	// if the current token being processed is not empty, push it onto vector and reset bo
			{
				tokenizedWords.push_back(currToken);
				currToken = "";
			}
			isSeparator = false;
		}
		else
		{
			currToken += s[i];		// otherwise, concatenate the character onto the current token
		}
	}
	if (currToken != "")
		tokenizedWords.push_back(currToken);

	return tokenizedWords;		// return vector of tokenized words
}

//******************** Tokenizer functions ************************************

// These functions simply delegate to TokenizerImpl's functions.
// You probably don't want to change any of this code.

Tokenizer::Tokenizer(string separators)
{
    m_impl = new TokenizerImpl(separators);
}

Tokenizer::~Tokenizer()
{
    delete m_impl;
}

vector<string> Tokenizer::tokenize(const std::string& s) const
{
    return m_impl->tokenize(s);
}
