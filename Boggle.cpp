/*
* File: Boggle.cpp
* ----------------
* Name: [TODO: enter name here]
* Section: [TODO: enter section leader here]
* This file is the main starter file for Assignment #4, Boggle.
* [TODO: extend the documentation]
*/

#include <iostream>
#include "gboggle.h"
#include "grid.h"
#include "gwindow.h"
#include "lexicon.h"
#include "random.h"
#include "simpio.h"
using namespace std;

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;
#define CUBE_SIDES 6
#define WORD_MIN_LENGTH 4



const string STANDARD_CUBES[16]  = {
	"AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
	"AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
	"DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
	"EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

const string BIG_BOGGLE_CUBES[25]  = {
	"AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
	"AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
	"CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
	"DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
	"FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

/* Function prototypes */

void welcome();
int setRules();
bool takeInput();
void giveInstructions();
void setBoard(Grid<char> & board);
void randomiseBoard(Grid<char> & board);
void getChars(Vector<char> & toUseChars, int boardDim);
void getGrid(Grid<char> & board, Vector<char> & toUseChars);
void playerRound(Set<string> & usedWords, Grid<char> & board);
void computerRound(Set<string> & usedWords, Grid<char> & board, int boardDim);
bool canBuildString(string input, Grid<char> & board);
void drawGrid(Grid<bool> answer, int boardDim);
bool recCanBuildString(string input, Grid<char> & board, Grid<bool> & answer, int x, int y);
bool pointIsValid(int i, int j, int x, int y, int dim, Grid<bool> & answer);
void repaintGrid(int boardDim);
void computerRound(Set<string> & usedWords, Grid<char> & board, int boardDim);
void recFindWord(Set<string> & usedWords, Grid<char> & board, Grid<bool> & usedChars, int boardDim, int x, int y, string currString, Lexicon english);

/* Main program */

int main() {
	GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
	initGBoggle(gw);
	welcome();

	int boardDim = setRules();
	
	drawBoard(boardDim, boardDim);

	while (true){
		
		Grid<char> grid(boardDim, boardDim);
		Set<string> usedWords;

		cout << "I'll give you a chance to set up the board to your specification, which makes it easier to confirm your boggle program is working. "<< endl;
		cout << "Do you want to force the board configuration? " << endl;
		if (takeInput()) setBoard(grid);
		else randomiseBoard(grid);
		
		playerRound(usedWords, grid);
		computerRound(usedWords, grid, boardDim);

		cout << "Wanna play again? " << endl;
		if (!takeInput()) break;

		
		drawBoard(boardDim, boardDim);
	}


	return 0;
}

/*
* Function: welcome
* Usage: welcome();
* -----------------
* Print out a cheery welcome message.
*/

void welcome() {
	cout << "Welcome!  You're about to play an intense game ";
	cout << "of mind-numbing Boggle.  The good news is that ";
	cout << "you might improve your vocabulary a bit.  The ";
	cout << "bad news is that you're probably going to lose ";
	cout << "miserably to this little dictionary-toting hunk ";
	cout << "of silicon.  If only YOU had a Gig of RAM..." << endl << endl;
}
/*
*	Gives user instructions and determines either it will be 4 by 4 boogle board or 5 by 5.
*/
int setRules(){
	cout << "Do you need instructions? " << endl;
	if (takeInput()) giveInstructions();

	cout << "You can choose standard Boggle (4x4 grid) "<< endl;
	cout << "or Big Boggle (5x5). " << endl;
	cout << "Would you like Big Boggle?" << endl;

	if(takeInput()) return 5;
	return 4;
}

/* This method teturns true if user answered positive on question or false if he does not. */
bool takeInput(){
	string s = "";
	while (true){
		s = toLowerCase(getLine(""));

		if (s == "ye" || s == "yep" || s == "yes" || s == "yeah") return true;

		if (s == "no" || s == "nope" || s == "nah") return false;

		cout << "Please answer yes or no." << endl;

	}
}

/*
* Function: giveInstructions
* Usage: giveInstructions();
* --------------------------
* Print out the instructions for the user.
*/

void giveInstructions() {
	cout << endl;
	cout << "The boggle board is a grid onto which I ";
	cout << "I will randomly distribute cubes. These ";
	cout << "6-sided cubes have letters rather than ";
	cout << "numbers on the faces, creating a grid of ";
	cout << "letters on which you try to form words. ";
	cout << "You go first, entering all the words you can ";
	cout << "find that are formed by tracing adjoining ";
	cout << "letters. Two letters adjoin if they are next ";
	cout << "to each other horizontally, vertically, or ";
	cout << "diagonally. A letter can only be used once ";
	cout << "in each word. Words must be at least four ";
	cout << "letters long and can be counted only once. ";
	cout << "You score points based on word length: a ";
	cout << "4-letter word is worth 1 point, 5-letters ";
	cout << "earn 2 points, and so on. After your puny ";
	cout << "brain is exhausted, I, the supercomputer, ";
	cout << "will find all the remaining words and double ";
	cout << "or triple your paltry score." << endl << endl;
	cout << "Hit return when you're ready...";
	getLine();
}


/* This method helps to test my program if its working correctly by letting user to set his own boogle board. */
void setBoard(Grid<char> & board){

	int boardDim = board.numCols();

	cout << "Enter a " << boardDim * boardDim <<"-character string to identify which letters you want on the cubes." << endl;
	cout << "The first " << boardDim << " letters are the cubes on the top row from left to right, "<< endl;
	cout << "the next " << boardDim << " letters are the second row, and so on. " << endl;

	bool bl= true;
	string chars;
	while (bl){
		chars = toUpperCase(getLine("Enter the string: "));

		if (chars.size() < boardDim * boardDim || chars.size() > boardDim * boardDim) 
			cout << "String must include " << boardDim*boardDim << " characters! Try again: " << endl;
		else{
			bl = false;
			for (int i = 0; i < boardDim * boardDim; i++){
				if (chars[i] < 'A' || chars[i] > 'Z') {
					bl = true;
					cout << "Please enter VALID english letters: " << endl;
					break;
				}
			}
		}
	}

	for (int row = 0; row < boardDim; row++){
		for (int col = 0; col < boardDim; col++){
			
			board[row][col] = chars[row * boardDim + col];
			labelCube(row, col, chars[row * boardDim + col]);
		}
	}

}
/* As method name says, it randomises boogle board by 1) - randomising sides of cubes and 2) randomising their places. */
void randomiseBoard(Grid<char> & board){
	int boardDim = board.numCols();

	Vector<char> toUseChars;
	getChars(toUseChars, boardDim);

	getGrid(board, toUseChars);
}

void getChars(Vector<char> & toUseChars, int boardDim){
	for (int i = 0; i < boardDim*boardDim; i++){
		if (boardDim == 4) toUseChars.add(STANDARD_CUBES[i][randomInteger(0, CUBE_SIDES - 1)]);
		else toUseChars.add(BIG_BOGGLE_CUBES[i][randomInteger(0, CUBE_SIDES - 1)]);
	}
}

void getGrid(Grid<char> & board, Vector<char> & toUseChars){
	for(int i = 0; i < board.numCols(); i++){
		for (int j = 0; j < board.numRows(); j++){
			int randomNum = randomInteger(0, toUseChars.size()-1);
			char randomChar = toUseChars[randomNum];

			board[i][j] = randomChar;
			labelCube(i, j, randomChar);

			toUseChars.remove(randomNum);
		}
	}
}
/* While input is not equal to ""  this method asks user for next guess and checks if that input is valid. */
void playerRound(Set<string> & usedWords, Grid<char> & board){
	
	cout << "Ok, take all the time you want and find all the words you can!" << endl;
	cout << "Signal that you're finished by entering an empty line. " << endl;

	string input = " ";
	Lexicon english("EnglishWords.dat");

	while (input != ""){
		repaintGrid(board.numCols());
		input = toUpperCase(getLine("Enter a word: "));
		if(input.size() < WORD_MIN_LENGTH) cout << "That word doesn't meet the minimum word length." << endl;
		else if (!english.contains(input)) cout << "That's not a word!" << endl;
		else if (usedWords.contains(input)) cout << "You've already guessed that!" << endl;
		else if (canBuildString(input, board)) {
			usedWords.add(input);
			recordWordForPlayer(input, HUMAN);
		}
		else cout << "You can't make that word!" << endl;
		pause(400);
	}

}
/* This method checks if given string could be "built" on boogle board. */
bool canBuildString(string input, Grid<char> & board){
	int boardDim = board.numCols();
	Grid<bool> answer(boardDim, boardDim);

	char ch = input[0];
	input = input.substr(1);

	for (int i = 0; i < boardDim; i++){
		for (int j = 0; j < boardDim; j++){
			if (board[i][j] == ch){
				answer[i][j] = true;
				if(recCanBuildString(input, board, answer, i, j)) {
					drawGrid(answer, boardDim);
					return true;
				}
				answer[i][j] = false;
			}
		}
	}

	return false;
}
/* Method colors word (if correctly found)*/
void drawGrid(Grid<bool> answer, int boardDim){
	for (int i = 0; i < boardDim; i++){
		for (int j = 0; j < boardDim; j++){
			if (answer[i][j]) highlightCube(i, j, true);
		}
	}
}
/* Method repaints everything back (as it was)*/
void repaintGrid(int boardDim){
	for (int i = 0; i < boardDim; i++){
		for (int j = 0; j < boardDim; j++){
			highlightCube(i, j, false);
		}
	}
}
/* Recursive method that helps to find if input string can be found on board. */
bool recCanBuildString(string input, Grid<char> & board, Grid<bool> & answer, int x, int y){
	if (input.size() == 0) return true;

	char ch = input[0];
	input = input.substr(1);

	for (int i = -1; i <= 1; i++){
		for (int j = -1; j <= 1; j++){
			if (pointIsValid(i, j, x, y, board.numCols(), answer)){

				if (board[x+i][y+j] == ch){
					answer[x+i][y+j] = true;
					if (recCanBuildString(input, board, answer, x+i, y+j))  return true;
					answer[x+i][y+j] = false;
				}
			}
		}
	}
	return false;
}
/* Checks if given coordinates exists and if letter on thos coordinates was not used yet. */
bool pointIsValid(int i, int j, int x, int y, int dim, Grid<bool> & answer){
	if (i + x < 0 || i + x >= dim || j + y < 0 || j + y >= dim || answer[x+i][y+j]) return false;
	return true;
}

/* Founds every possible word (that was not used yet) on board and prints them. */
void computerRound(Set<string> & usedWords, Grid<char> & board, int boardDim){
	string currString = "";
	Lexicon english("EnglishWords.dat");
	Grid<bool> usedChars(boardDim, boardDim);

	for (int i = 0; i < boardDim; i++){
		for (int j = 0; j < boardDim; j++){
			currString = board[i][j];
			recFindWord(usedWords, board, usedChars, boardDim, i, j, currString, english);
		}
	}
}

void recFindWord(Set<string> & usedWords, Grid<char> & board, Grid<bool> & usedChars, int boardDim, int x, int y, string currString, Lexicon english){
	usedChars[x][y] = true;
	if(currString.size() >= WORD_MIN_LENGTH && english.contains(currString) && 
					!usedWords.contains(currString)){
		recordWordForPlayer(currString, COMPUTER);
		usedWords.add(currString);	
	}
	for (int i = -1; i <= 1; i++){
		for (int j = -1; j <= 1; j++){
			if (pointIsValid(i, j, x, y, boardDim, usedChars)){
				string temp = currString + board[i+x][j+y];
				if(english.containsPrefix(temp)) recFindWord(usedWords, board, usedChars, boardDim, x+i, y+j, temp, english);
			}
		}
	}
	usedChars[x][y] = false;
}