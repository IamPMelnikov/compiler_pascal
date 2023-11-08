#pragma once
#include <string>
#include "CErrorManager.h"
#include <regex>
#include <string>
#include <iostream>
#include "CToken.h"
#include "CTokenFactory.h"

using namespace std;


// Класс, отвечающий за лексический анализ
class CLexic {
private:
	const string *text; //текст проги 
	int pos, lineNum = 1; //абсолютная позиция, номер строки
	int lastLexemStartPos = 0; //место начала последней (рассматриваемой) лексемы
	int lastNewLinePos = 0; //прзиция начала последней (рассматриваемой) строки 

	CTokenFactory factory; //объект класса для определения типа токена
	CErrorManager* errorManager; //объект менеджера ошибок (для записи их)
	string getLexem();	// получить лексему из текста, начиная с позиции pos
	bool isLetter(char ch);	// проверка является ли передаваемый символ буквой
	bool isDigit(char ch);	// проверка является ли передаваемый символ цифрой
	void passWhitespaces();//пропуск пробельных символов
	void skipComments();// пропуск многострочного комментария
	void setOldPos(int _pos, int _lineNum, int _lastNewLinePos, int _lastLexemStartPos); // запись переменных позиции  (для заполнения старыми данными когда смотрели вперед)

public:
	CLexic(CErrorManager* errorManager, const string *text);//инициализация объекта с закидкой текста и менеджера ошибок
	unique_ptr<CToken> getNext(bool get);// получить или посмотреть следующий токен
	int getStartPosition();	// Возвращает индекс начала последней лексемы в строке
	int getCurLine();// возврат текущей строки
	void passToNewLine();// пропуск символов до начала новой строки или до конца файла
	int getCurPos();// возвращает текущую позицию в файле
	int getCurPosInLine();	// возвращает текущую позицию в строке
	unique_ptr<CToken> skip(set<string> lexemes); // пропуск до заданных лексем
};
