#include "provided.h"
#include "MyHash.h"
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <cctype>
using namespace std;

class WordListImpl
{
public:
	WordListImpl();
    bool loadWordList(string filename);
    bool contains(string word) const;
    vector<string> findCandidates(string cipherWord, string currTranslation) const;
private:
	MyHash<string, vector<string>> m_hash1;			// m_hash1 holds the patterns and its corresponding words with such pattern
	MyHash<string, char>	m_hash2;				// m_hash2 holds the words in the word list as keys, with dummy values for value

	// Helper functions
	string getPattern(string word) const;			// gets the pattern of a string
};

WordListImpl::WordListImpl()
{
}

bool WordListImpl::loadWordList(string filename)
{
	// clear the current hash tables
	m_hash1.reset();
	m_hash2.reset();

	ifstream infile(filename);	// read file input
	if (!infile)
		return false;			// if error in reading filename

	string s;
	while (getline(infile, s))			// checks if has reached the end of the file
	{
		// do check for if line contains a character that is not a letter or apostrophe, ignore
		bool ignoreWord = false;
		for (unsigned int i = 0; i < s.size(); i++)
		{
			s[i] = tolower(s[i]);		// turns all characters to lowercase
			if (!isalpha(s[i]) && s[i] != '\'')
			{
				ignoreWord = true;
				break;
			}
		}
		if (ignoreWord)
			continue;

		m_hash2.associate(s, ' ');		// inserts word into 2nd hashmap

		string pattern = getPattern(s);
		
		vector<string>* v = m_hash1.find(pattern);
		if (v == nullptr)			// if pattern not already in the map
		{
			m_hash1.associate(pattern, vector<string>());
			vector<string>* p = m_hash1.find(pattern);
			p->push_back(s);		// push the current word onto vector
		}
		else
		{
			v->push_back(s);		// if pattern already in map, just add onto the vector that was returned
		}
	}
	return true;
}

bool WordListImpl::contains(string word) const
{
	for (unsigned int i = 0; i < word.size(); i++)
		word[i] = tolower(word[i]);
	if (m_hash2.find(word) != nullptr)
		return true;
	return false;
}

vector<string> WordListImpl::findCandidates(string cipherWord, string currTranslation) const
{
	if (cipherWord.size() != currTranslation.size())
		return vector<string>();

	for (unsigned int i = 0; i < cipherWord.size(); i++)
	{
		cipherWord[i] = tolower(cipherWord[i]);		// turns all characters to lowercase
		// check that cipherword and currTranslation only contain alphabetic or apostrophes, or '?' (currTranslation only)
		if ((!isalpha(cipherWord[i]) && cipherWord[i] != '\'') && (!isalpha(currTranslation[i]) && currTranslation[i] != '\'' && currTranslation[i] != '?'))
			return vector<string>();

	}

	string cipherPattern = getPattern(cipherWord);		// get the pattern for the given cipherWord

	const vector<string>* potentialMatches = m_hash1.find(cipherPattern);	// holds the vector of strings that match the cipherWord pattern
	vector<string> candidates;												// holds the vector of candidates that match the current translation

	if (potentialMatches == nullptr)		// if no potential matches found, return an empty vector
		return vector<string>();

	for (unsigned int i = 0; i < potentialMatches->size(); i++)
	{
		string curr = (*potentialMatches)[i];	// curr holds the current string we are evaluating
		bool isCandidate = true;				// bool to tell us whether or not a string is a candidate
		for (unsigned int j = 0; j < currTranslation.size(); j++)
		{
			// if character in currTranslation is a '?', the corresponding letter of a word in potentialMatches must be alphabetic
			// if it is an apostrophe, there must be a corresponding apostrophe in the current string
			// if it is alphabetic, then the corresponding letter in current string must match exactly
			if ((currTranslation[j] == '?' && !isalpha(curr[j])) || 
				(currTranslation[j] == '\'' && curr[j] != '\'') ||
				(isalpha(currTranslation[j]) && tolower(currTranslation[j]) != curr[j]))
			{
				isCandidate = false;
				break;
			}
		}
		if (isCandidate)
		{
			candidates.push_back(curr);		// if the word is a candidate, push it onto the vector
		}
	}

	return candidates;
}

string WordListImpl::getPattern(string word) const
{
	string temp = word;
	char alpha[26];		// create array of 26 chars, initialize to spaces
	for (int i = 0; i < 26; i++)
		alpha[i] = ' ';

	char patternChar = 'a';			// current character used in pattern
	for (unsigned int i = 0; i < word.size(); i++)
	{
		if (word[i] == '\'')		// if apostrophe, leave as is
			continue;

		// subtracting by 'a' will give the correct index for that letter
		if (alpha[temp[i] - 'a'] == ' ')		// if the element is a space, this character has never been used in the pattern before
		{
			alpha[temp[i] - 'a'] = patternChar;		// add the current patternChar to the array element
			patternChar++;							// increment to the next char
		}
		temp[i] = alpha[temp[i] - 'a'];			// replace the character in the temp string to the pattern char
	}
	
	return temp;		// return the new pattern string
}

//***** hash functions for string, int, and char *****

unsigned int hasher(const std::string& s)
{
    return std::hash<std::string>()(s);
}

unsigned int hasher(const int& i)
{
    return std::hash<int>()(i);
}

unsigned int hasher(const char& c)
{
    return std::hash<char>()(c);
}

//******************** WordList functions ************************************

// These functions simply delegate to WordListImpl's functions.
// You probably don't want to change any of this code.

WordList::WordList()
{
    m_impl = new WordListImpl;
}

WordList::~WordList()
{
    delete m_impl;
}

bool WordList::loadWordList(string filename)
{
    return m_impl->loadWordList(filename);
}

bool WordList::contains(string word) const
{
    return m_impl->contains(word);
}

vector<string> WordList::findCandidates(string cipherWord, string currTranslation) const
{
   return m_impl->findCandidates(cipherWord, currTranslation);
}
