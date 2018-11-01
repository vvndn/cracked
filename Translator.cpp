#include "provided.h"
#include <string>
#include <list>
#include <vector>
#include <cctype>
using namespace std;

class TranslatorImpl
{
public:
	TranslatorImpl();
    bool pushMapping(string ciphertext, string plaintext);
    bool popMapping();
    string getTranslation(const string& ciphertext) const;
private:
	char m_currMap[26];					// current Translator map
	list<vector<char>> m_mapStack;		// "stack" of maps that have been pushed
	int m_numPush;						// number of times pushMapping() has been called
	int m_numPop;						// number of times popMapping() has been called
};

TranslatorImpl::TranslatorImpl()
	: m_numPush(0), m_numPop(0)
{
	for (unsigned int i = 0; i < 26; i++)
	{
		m_currMap[i] = '?';						// initialize current map to be all '?'
	}
}

bool TranslatorImpl::pushMapping(string ciphertext, string plaintext)
{
	if (ciphertext.size() != plaintext.size())		// if the length of the ciphertext and plaintext are not the same, return false
		return false;

	for (unsigned int i = 0; i < ciphertext.size(); i++)
	{
		if (!isalpha(ciphertext[i]) || !isalpha(plaintext[i]))		// if any of the characters are non alphabetic, return false
			return false;

		ciphertext[i] = tolower(ciphertext[i]);
		plaintext[i] = tolower(plaintext[i]);
	}
	
	vector<char> newMap;			// will be the newest mapping
	vector<char> oldMap;			// holds a copy of the old map
	for (unsigned int i = 0; i < 26; i++)
	{
		newMap.push_back(m_currMap[i]);			// copy over from current 
		oldMap.push_back(m_currMap[i]);
	}

	for (unsigned int j = 0; j < ciphertext.size(); j++)
	{
		if (newMap[ciphertext[j] - 'a'] == '?')			// if the current letter does not already map to something, update 
		{
			for (unsigned int a = 0; a < 26; a++)
			{
				if (newMap[a] == plaintext[j] && (a != (ciphertext[j] - 'a')))			// checks to see if current plaintext char has already been mapped to another ciphertext char
				{
					return false;
				}
				else
					continue;
			}
			newMap[ciphertext[j] - 'a'] = plaintext[j];			// add the letter to the map
		}
		else if (newMap[ciphertext[j] - 'a'] == plaintext[j])		// if the same letter is already mapped, move on with no changes
			continue;
		else
		{
			return false;										// if it already has a value, return false
		}
	}

	for (unsigned int k = 0; k < 26; k++)					// update current map with newest mappings
	{
		m_currMap[k] = newMap[k];
	}

	m_mapStack.push_front(oldMap);							// push the old map (before updates)
	m_numPush++;											// increase the number of pushes

	return true;

}

bool TranslatorImpl::popMapping()
{
	if (m_numPop == m_numPush)								// if the number of poppings is equal to the number of pushes
		return false;										// return false since you cannot pop more than you push
	else
	{
		for (unsigned int i = 0; i < 26; i++)				// update the current map with the values from the top map
			m_currMap[i] = (m_mapStack.front())[i];

		m_mapStack.pop_front();								// pop off the map
		m_numPop++;	
		return true;
	}
}

string TranslatorImpl::getTranslation(const string& ciphertext) const
{
	string translation;										// string to hold translation
	for (unsigned int i = 0; i < ciphertext.size(); i++)
	{
		char curr = tolower(ciphertext[i]);					// change each character to lowercase in a temp variable for processing
		if (!isalpha(curr))									// if not alphabetic, add the character and go to the next iteration
		{
			translation += curr;
			continue;
		}
		translation += m_currMap[curr - 'a'];				// concatenate the new character to the translation
		if (isupper(ciphertext[i]))							// if the current character in ciphertext is uppercase, change the translation char to reflect this
			translation[translation.size() - 1] = toupper(translation[translation.size() - 1]);
	}
	return translation;
}

//******************** Translator functions ************************************

// These functions simply delegate to TranslatorImpl's functions.
// You probably don't want to change any of this code.

Translator::Translator()
{
    m_impl = new TranslatorImpl;
}

Translator::~Translator()
{
    delete m_impl;
}

bool Translator::pushMapping(string ciphertext, string plaintext)
{
    return m_impl->pushMapping(ciphertext, plaintext);
}

bool Translator::popMapping()
{
    return m_impl->popMapping();
}

string Translator::getTranslation(const string& ciphertext) const
{
    return m_impl->getTranslation(ciphertext);
}
