#include "provided.h"
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

class DecrypterImpl
{
public:
	DecrypterImpl();
    bool load(string filename);
    vector<string> crack(const string& ciphertext);
private:
	// Recursive helper function
	void crack(const string& ciphertext, vector<string>& output);

	WordList wl;						// WordList object
	Translator t;						// Translator object
	Tokenizer token;					// Tokenizer object
	vector<string> tokenizedWords;		// vector of tokenized words
};

DecrypterImpl::DecrypterImpl()
	: token(" ,;:.!()[]{}-\"#$%^&0123456789")
{
}

bool DecrypterImpl::load(string filename)
{
	if (!wl.loadWordList(filename))
		return false;
	return true;
}

vector<string> DecrypterImpl::crack(const string& ciphertext)
{

	vector<string> validSolutions;		// vector of valid solution strings

	// Step 2:
	// Tokenize the given ciphertext string
	tokenizedWords = token.tokenize(ciphertext);			

	crack(ciphertext, validSolutions);		// calling helper function

	if (!validSolutions.empty())			// sort the vector of valid solutions in alphabetical order
	{
		sort(validSolutions.begin(), validSolutions.end());
	}
	
    return validSolutions;  
}

void DecrypterImpl::crack(const string& ciphertext, vector<string>& output)
{
	// Step 2 (pick word with most untranslated letters)
	int maxNum = 0;
	int indexOfMaxNum = 0;
	for (unsigned int i = 0; i < tokenizedWords.size(); i++)
	{
		string word = tokenizedWords[i];
		string translated = t.getTranslation(word);
		int count = 0;
		for (unsigned int j = 0; j < word.size(); j++)
		{
			if (translated[j] == '?')
				count++;
		}
		if (count > maxNum)
		{
			maxNum = count;
			indexOfMaxNum = i;
		}
	}

	// Step 3
	// get the translation of the picked word
	string pickedWord = tokenizedWords[indexOfMaxNum];
	string translatedWord = t.getTranslation(pickedWord);

	// Step 4
	// Find all candidates that fit with the given picked word and translated word
	vector<string> foundCandidates = wl.findCandidates(pickedWord, translatedWord);

	// Step 5
	// if no candidates found, pop and return
	if (foundCandidates.empty())
	{
		t.popMapping();
		return;				// this should exit the recursive call
	}

	// Step 6
	// iterate through the foundCandidates vector
	int count = 0;
	for (unsigned int i = 0; i < foundCandidates.size(); i++)
	{
		if (t.pushMapping(pickedWord, foundCandidates[i]))					// create temp mapping table by pushing a copy of the mapping table
		{
			count = 0;														// count variable holds the number of fully translated words that
																			// that are found in the wordlist
			string translatedCipher = t.getTranslation(ciphertext);			// get translation of entire ciphertext msg
			vector<string> translatedCipherTokenized = token.tokenize(translatedCipher);	// tokenize the translated cipher
			bool wordNotFound = false;												// bool to keep track if a word was not found in wordlist
			for (unsigned int j = 0; j < translatedCipherTokenized.size(); j++)
			{
				bool fullyTranslated = true;							// bool to keep track of whether or not a word is fully translated
				wordNotFound = false;									// reinitialize
				string curr = translatedCipherTokenized[j];				// current string we are looking at
				for (unsigned int k = 0; k < curr.size(); k++)
				{
					if (curr[k] == '?')									// if a character in the current string is a '?',
					{													// it has not been fully translated, so break out of the loop and check other words
						fullyTranslated = false;
						break;
					}
				}
				// 6ci)
				if (fullyTranslated && !wl.contains(curr))	// if word is fully translated but NOT in word list
				{
					t.popMapping();			// pop the map
					wordNotFound = true;
					break;
				}
				else if (fullyTranslated && wl.contains(curr))
				{
					count++;			// increment count of fully translated words that are in word list
				}
			}
			if (wordNotFound)
				continue;				// return to step 6

			if (count < translatedCipherTokenized.size())			// if message has not been completely translated
			{	
				crack(ciphertext, output);							// recursive call with current mapping
			}
			if (count == translatedCipherTokenized.size())		// if message has been fully translated and all words in wordlist
			{
				// record as valid solution for user
				output.push_back(translatedCipher);
				t.popMapping();									// pop the map
			}
		}
	}
	t.popMapping();				// if out of possible candidates to check, pop the map and return
	return;
}

//******************** Decrypter functions ************************************

// These functions simply delegate to DecrypterImpl's functions.
// You probably don't want to change any of this code.

Decrypter::Decrypter()
{
    m_impl = new DecrypterImpl;
}

Decrypter::~Decrypter()
{
    delete m_impl;
}

bool Decrypter::load(string filename)
{
    return m_impl->load(filename);
}

vector<string> Decrypter::crack(const string& ciphertext)
{
   return m_impl->crack(ciphertext);
}
