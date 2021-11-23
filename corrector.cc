#include <fstream>
#include <iostream>
#include <sstream>

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>

using namespace std;

#define INF ((unsigned)~0)
#define MAX_EDIT_DISTANCE 2

struct SourceCandidate
{
    string candidate;
    unsigned int distance;
};

struct CandidateCompare
{
    bool operator()(const SourceCandidate &a, const SourceCandidate &b)
    {
        return a.distance >= b.distance;
    }
};

typedef unordered_set<string> Dictionary;
typedef priority_queue<SourceCandidate, vector<SourceCandidate>, CandidateCompare> CandidatesByDistance;
typedef unordered_map<string, CandidatesByDistance> SymSpellDictionary;
typedef string Text;

inline pair<string, string> parseArgs(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Invalid arguments, run the program with <TXT_FILE_PATH> <DICTIONARY_PATH> as the first and second arguments" << endl;
        exit(1);
    }
    else
        return {argv[1], argv[2]};
}

inline void generateEdits(SymSpellDictionary &symSpell, const string &sourceWord, string word, unsigned d)
{
    if (d > MAX_EDIT_DISTANCE or word.size() <= 1)
        return;

    for (unsigned int i = 0; i < word.size(); ++i)
    {
        string reducedWord = word;
        reducedWord.erase(i, 1);

        SymSpellDictionary::iterator it = symSpell.find(reducedWord);
        if (it == symSpell.end())
        {
            CandidatesByDistance candidatesByDistance;
            candidatesByDistance.push({sourceWord, d});
            symSpell.insert({reducedWord, candidatesByDistance});
        }
        else
        {
            it->second.push({sourceWord, d});
        }

        generateEdits(symSpell, sourceWord, reducedWord, d + 1);
    }
}

inline string generateAndGetBestCandidate(const string &word, const SymSpellDictionary &symSpell)
{
    string nearestWord = word;
    unsigned int nearestDistance = INF;

    queue<string> wQueue;
    wQueue.push(word);

    while (not wQueue.empty())
    {
        string partialWord = wQueue.front();
        wQueue.pop();

        SymSpellDictionary::const_iterator it = symSpell.find(partialWord);
        if (it != symSpell.end())
        {
            SourceCandidate c = it->second.top();
            if (c.distance < nearestDistance)
            {
                nearestDistance = c.distance;
                nearestWord = c.candidate;

                if (nearestDistance <= 1)
                    return nearestWord;
            }
        }

        if (partialWord.size() > 1)
        {
            for (unsigned int i = 0; i < partialWord.size(); ++i)
            {
                string reducedWord = partialWord;
                reducedWord.erase(i, 1);
                wQueue.push(reducedWord);
            }
        }
    }

    return nearestWord;
}

inline pair<Dictionary, SymSpellDictionary> loadDictionary(const string &dictionaryPath)
{
    ifstream dictionaryFile(dictionaryPath);
    if (not dictionaryFile.is_open())
    {
        cerr << "File " << dictionaryPath << " not found..." << endl;
        exit(1);
    }

    Dictionary dictionary;
    SymSpellDictionary symSpell;

    string word;
    while (getline(dictionaryFile, word))
    {
        word.pop_back();
        dictionary.insert(word);
        generateEdits(symSpell, word, word, 1);

        SymSpellDictionary::iterator it = symSpell.find(word);
        if (it == symSpell.end())
        {
            CandidatesByDistance candidatesByDistance;
            candidatesByDistance.push({word, 0});
            symSpell.insert({word, candidatesByDistance});
        }
        else
        {
            it->second.push({word, 0});
        }
    }

    dictionaryFile.close();
    return {dictionary, symSpell};
}

inline Text loadText(const string &textPath)
{
    Text text;

    ifstream textFile(textPath);
    stringstream buffer;

    if (textFile.is_open())
    {
        buffer << textFile.rdbuf();
        text = buffer.str();
        textFile.close();
    }
    else
    {
        cerr << "File " << textPath << " not found..." << endl;
        exit(1);
    }

    return text;
}

inline void correctText(const Text &text, const Dictionary &dictionary, const SymSpellDictionary &symSpell)
{
    Text result;

    size_t wordStart = 0;
    size_t wordEnd = 0;

    while (wordEnd < text.size())
    {
        if (text[wordEnd] != ' ')
        {
            if (text[wordEnd] == '\n')
            {
                result.append("\n");
                ++wordEnd;
                wordStart = wordEnd;
            }

            ++wordEnd;
            continue;
        }

        unsigned int wordLength = wordEnd - wordStart;
        if (wordLength > 0)
        {
            string word = text.substr(wordStart, wordEnd - wordStart);

            char punct = 'P';
            char lastChar = word[word.size() - 1];
            if (ispunct(lastChar))
            {
                punct = lastChar;
                word.pop_back();
            }

            bool isCorrect = dictionary.find(word) != dictionary.end();
            if (not isCorrect)
                word = generateAndGetBestCandidate(word, symSpell);

            if (punct != 'P')
                word.push_back(punct);
            word.push_back(' ');

            result.append(word);
        }

        ++wordEnd;
        wordStart = wordEnd;
    }

    cout << result << endl;
}

int main(int argc, char *argv[])
{
    pair<string, string> dataFiles = parseArgs(argc, argv);

    Text originalText = loadText(dataFiles.first);
    pair<Dictionary, SymSpellDictionary> dictionary = loadDictionary(dataFiles.second);

    correctText(originalText, dictionary.first, dictionary.second);
}