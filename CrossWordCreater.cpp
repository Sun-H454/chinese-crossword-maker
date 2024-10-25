#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <unordered_map>

using namespace std;

// Constants for grid dimensions
constexpr auto MAP_X = 30; // Grid width
constexpr auto MAP_Y = 55; // Grid height

// Constants for game settings
#define TYPE_OF_DIRECTION 4
#define TIME_OF_ATTEMPT 100
#define VECTOR_SIZE 10000

// direction
#define DIR_L2R 0
#define DIR_DOWN 1
#define DIR_R2L 2
#define DIR_UP 3

unsigned int seed; // seed for random number generator

// struct to store Chinese character
struct hz {
	char p1, p2;
	// hz() : p1(0), p2(0) {}
	// hz(char p1, char p2) : p1(p1), p2(p2) {}

	bool operator== (const hz& other) const {
		return (p1 == other.p1 && p2 == other.p2);
	}

	bool operator!=(const hz& other) const {
		return !(*this == other);
	}

	hz& operator=(const hz& other) {
		p1 = other.p1;
		p2 = other.p2;
		return *this;
	}

	void Print() const {
		cout << p1 << p2;
	}
	bool Empty() const {
		return (p1 == 0 && p2 == 0);
	}

};
// Main grid for the crossword map
hz grid[MAP_X + 2][MAP_Y + 2];

// Hash function for ChineseCharacter
struct HashHz {
	size_t operator()(const hz& h) const {
		return hash<char>()(h.p1) ^ hash<char>()(h.p2);
	}
};

// Dictionary mapping characters to corresponding sentence positions
unordered_map<hz, vector<pair<int, int> >, HashHz> wordToSentences;

// use index torepresent a way to fill the grid
struct state {
	int p1, p2;     // Position in the sentence
	int m1, m2;     // Position in the grid
	int direction;
};

// struct to prevent illegally word placement in this direction
struct disabled {
	bool hor, ver;
}d[MAP_X][MAP_Y];

typedef vector<hz> sentence;
vector<sentence> se;      // all sentences that can be used

// labels
vector<int> labelMap[MAP_X + 2][MAP_Y + 2];
vector<int> sorts(VECTOR_SIZE);
vector<int> used(VECTOR_SIZE);
vector<int> isTitleSentence(VECTOR_SIZE);

// clues storage for each direction
vector< pair<int, int > > clueList[TYPE_OF_DIRECTION];

// read characters from file
void readData(string& s)
{
	ifstream in("data.txt");
	char ch;
	while (in.get(ch))
		s.push_back(ch);
}

// initialize sentences and others
void init(string& s)
{
	ifstream config("init.ini");
	config >> seed;
	srand(seed);

	string fs;
	config >> fs;
	for (int i = 0; i < fs.size() / 2; i++)// fill with the first sentence
	{
		grid[i][MAP_Y / 2] = { fs[2 * i],fs[2 * i + 1] };
		d[i][MAP_Y / 2 - 1].ver = d[i][MAP_Y / 2].ver = d[i][MAP_Y / 2 + 1].ver = 1;
		if (i == 0)labelMap[i][MAP_Y / 2].push_back(0);
	}

	// divide data into sentences
	vector<hz> fse;
	bool istitle;
	for (int i = 0; i < s.size();)
	{
		if (s[i] == '%')// title flag
		{
			istitle = true;
			i++;
		}
		else if (s[i] == '\n' || s[i] == '\r' || s[i] == ' ') {
			if (fse.size() > 0)
			{
				se.push_back(fse);
				fse.clear();

				if (istitle)
				{
					istitle = 0;
					isTitleSentence[se.size() - 1] = 1;
				}
			}
			i++;
		}
		else if (s[i] == -95 && s[i + 1] == -94
			|| s[i] == -95 && s[i + 1] == -74
			|| s[i] == -95 && s[i + 1] == -73)// 将顿号，书名号吞掉
			i += 2;
		else if (s[i] == -93 && s[i + 1] == -84
			|| s[i] == -95 && s[i + 1] == -93
			|| s[i] == -93 && s[i + 1] == -70
			|| s[i] == -95 && s[i + 1] == -80
			|| s[i] == -95 && s[i + 1] == -79
			|| s[i] == -93 && s[i + 1] == -69
			|| s[i] == -93 && s[i + 1] == -65
			|| s[i] == -93 && s[i + 1] == -95)// 标点符号分割
		{
			if (fse.size() > 0)
			{
				se.push_back(fse);
				fse.clear();
			}
			i += 2;
		}
		else {
			fse.push_back({ s[i],s[i + 1] });
			i += 2;
		}
	}
	// Populate wordToSentences
	for (int k = 0; k < se.size(); k++) {
		sentence s = se[k];
		for (int l = 0; l < s.size(); l++) {
			wordToSentences[s[l]].emplace_back(k, l); // map character to its sentences position
		}
	}
}

// To check if the sentence can be placed in this direction,
// or fill the grid with the sentence in this direction.
bool isProper(int q, int p, int x, int y, int direction, bool alter, int number)
{
	sentence s = se[q];
	int len = s.size();

	if (direction == DIR_L2R || direction == DIR_R2L)// Horizontal
	{
		if (y - p < 0 || y - p + len - 1 >= MAP_Y) return 0;// exceeds the grid
		if (alter)
		{
			// prevent to fill before or after one character in the sentence
			if (y - p > 0)d[x][y - p - 1] = { 1,1 };
			if (y - p + len < MAP_Y)d[x][y - p + len] = { 1,1 };
		}
		else
		{
			if (y - p > 0 && !grid[x][y - p - 1].Empty()
				|| y - p + len < MAP_Y && !grid[x][y - p + len].Empty())return 0;
		}
		for (int i = 0, j; i < len; i++)
		{
			if (direction == DIR_L2R)j = y - p + i;
			if (direction == DIR_R2L)j = y - p + len - i - 1;
			if (alter)
			{
				d[x][j].hor = 1;
				if (x > 0)d[x - 1][j].hor = 1;
				if (x + 1 < MAP_X)d[x + 1][j].hor = 1;
				grid[x][j] = s[i];
				if (i == 0)                         // 首字，填入标号。
					labelMap[x][j].push_back(number);
			}
			else {
				if ((!grid[x][j].Empty() && grid[x][j] != s[i]) || d[x][j].hor)// 非空 & 非本字,或者是邻域有字
				{
					return 0;
				}
			}
		}
		return 1;
	}
	if (direction == DIR_UP || direction == DIR_DOWN)// 纵向
	{
		if (x - p < 0 || x - p + len - 1 >= MAP_X)return 0;
		if (alter)
		{
			if (x - p > 0)d[x - p - 1][y] = { 1,1 };
			if (x - p + len < MAP_X)d[x - p + len][y] = { 1,1 };
		}
		else
		{
			if (x - p > 0 && !grid[x - p - 1][y].Empty()
				|| x - p + len < MAP_X && !grid[x - p + len][y].Empty())return 0;
		}
		for (int i = 0, j; i < len; i++)
		{
			if (direction == DIR_DOWN)j = x - p + i;
			if (direction == DIR_UP)j = x - p + len - i - 1;
			if (alter)
			{
				d[j][y].ver = 1;
				if (y > 0)d[j][y - 1].ver = 1;
				if (y + 1 < MAP_Y)d[j][y + 1].ver = 1;
				grid[j][y] = s[i];
				if (i == 0)
					labelMap[j][y].push_back(number);
			}
			else {
				if ((!grid[j][y].Empty() && grid[j][y] != s[i]) || d[j][y].ver)
				{
					return false;
				}
			}
		}
		return 1;
	}

}

// To find all possible states for a character in the grid
void getStates(vector<state>& possibleStates)
{
	for (int i = 0; i < MAP_X; i++)
	{
		for (int j = 0; j < MAP_Y; j++)
		{
			// Iterate through each character in the grid
			hz mz = grid[i][j];
			if (mz.Empty())
				continue;

			// check if possible to place
			if (wordToSentences.find(mz) != wordToSentences.end()) {
				const auto& sentences = wordToSentences[mz];
				for (const auto& pair : sentences) {
					int k = pair.first;
					int l = pair.second;
					if (used[k]) continue;

					// check all directions to place the sentence
					for (int direction = 0; direction < TYPE_OF_DIRECTION; direction++) {
						if (isProper(k, l, i, j, direction, 0, 0)) {
							possibleStates.push_back({ k, l, i, j, direction });
						}
					}
				}
			}

		}
	}
}

bool comp(pair<int, int> x, pair<int, int> y)
{
	return x.first < y.first;
}

// Output the Crossword results to files.
void OutputResult()
{
	char seedc[50];
	_itoa(seed, seedc, 10);
	ofstream answers(strcat(seedc, "ans.txt"));
	_itoa(seed, seedc, 10);
	ofstream clues(strcat(seedc, "clue.txt"));
	_itoa(seed, seedc, 10);
	ofstream maps(strcat(seedc, "maps.csv"));

	int tot = 0;
	for (int i = 0; i < MAP_X; i++)
	{
		for (int j = 0; j < MAP_Y; j++)
		{
			// reorder lables by their appearance order in the grid
			for (auto it = labelMap[i][j].begin(); it != labelMap[i][j].end(); it++)
			{
				if (*it != 0)
					sorts[*it] = ++tot;
			}
			// output the answer map
			hz* it = &grid[i][j];
			if (it->Empty())answers << "  ";
			else {
				answers << (it->p1) << (it->p2);
			}
		}
		answers << endl;
	}

	// output the clues
	for (int i = 0; i < TYPE_OF_DIRECTION; i++)
	{
		for (auto it = clueList[i].begin(); it != clueList[i].end(); it++)
		{
			it->first = sorts[it->first];
		}
		sort(clueList[i].begin(), clueList[i].end(), comp);
	}
	clues << "Clues:" << endl;
	for (int i = 0; i < TYPE_OF_DIRECTION; i++)
	{
		switch (i)
		{
		case 0:clues << "Across(left to right)"; break;
		case 1:clues << "Down"; break;
		case 2:clues << "Across(right to left)"; break;
		case 3:clues << "Up"; break;
		}
		clues << endl;
		for (auto it = clueList[i].begin(); it != clueList[i].end(); it++)      /*int j = 0; j < clueList[i].size(); j++*/
		{
			int p = it->second;
			clues << it->first << "(" << se[p].size() << "字)" << ": ";
			if (isTitleSentence[p])
			{
				clues << "\"";
				for (int k = 0; k < se[p + 1].size(); k++)
					clues << se[p + 1][k].p1 << se[p + 1][k].p2;
				clues << "\"所在篇目的标题。" << endl;
			}
			else if (isTitleSentence[p - 1])
			{
				clues << "《";
				for (int k = 0; k < se[p - 1].size(); k++)
					clues << se[p - 1][k].p1 << se[p - 1][k].p2;
				clues << "》的首句。" << endl;
			}
			else {
				clues << "\"";
				for (int k = 0; k < se[p - 1].size(); k++)
					clues << se[p - 1][k].p1 << se[p - 1][k].p2;
				clues << "\"的下一句。" << endl;
			}
		}
	}

	// output CSV file to show the quiz.
	for (int i = 0; i < MAP_X; i++)
	{
		for (int j = 0; j < MAP_Y; j++)
		{
			if ((i != 0 && j != 0 && i != MAP_X - 1 && j != MAP_Y - 1
				&& !grid[i][j - 1].Empty() && !grid[i][j + 1].Empty() && !grid[i + 1][j].Empty() && !grid[i - 1][j].Empty()
				&& grid[i][j].Empty()))
				maps << "x";			// No characters and surrounded
			else if (!grid[i][j].Empty())
				maps << " ";

			for (auto it = labelMap[i][j].begin(); it != labelMap[i][j].end(); it++)
			{
				maps << sorts[*it] << " ";
			}

			if (j == MAP_Y - 1)
				maps << endl;
			else
				maps << ",";
		}
	}
}


int main()
{
	string inputData;
	readData(inputData);
	init(inputData);

	for (int i = 1; i <= TIME_OF_ATTEMPT; i++)   // Attempt several times
	{
		cout << "\rProgress of generating：" << i << " / " << TIME_OF_ATTEMPT;
		vector<state> possibleStates;             // Store all possible states
		possibleStates.clear();
		getStates(possibleStates);

		if (possibleStates.size() == 0) {       // Insufficient data for generation
			cout << "Cannot generate the Crossword in this situaion.\n\
				Check the data is adequate or try another seed.";
			break;
		}

		int choice = rand() % possibleStates.size();// Randomly select a state
		state selectedState = possibleStates[choice];

		clueList[selectedState.direction].push_back(make_pair(i, selectedState.p1));

		// place the sentence in the grid
		used[selectedState.p1] = 1;
		isProper(selectedState.p1, selectedState.p2, selectedState.m1, selectedState.m2, selectedState.direction, 1, i);
	}
	cout << endl << endl;
	OutputResult();
	return 0;
}


