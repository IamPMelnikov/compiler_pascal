#include "Lexic.h"
#include <string>
#include "ToLowerStr.h"
using namespace std;

//если символ является буквой, то возвращает true иначе false
bool CLexic::isLetter(char ch) {
	if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z')
		return true;
	else return false;
}

//если символ является цифрой, то возвращает true иначе false
bool CLexic::isDigit(char ch)
{
	if (ch >= '0' && ch <= '9')
		return true;
	else return false;
}

//пропуск пробельных символов
void CLexic::passWhitespaces() {

	while ((*text)[pos] == ' ' || (*text)[pos] == '\r' || (*text)[pos] == '\t' || (*text)[pos] == '\n') {
		if ((*text)[pos] == '\n') {
			lineNum++;//если попался перевод строки, то увеличиваем счетчик строки на 1
			lastNewLinePos = pos;//запоминаем позицию конца строки
		}
		pos++;//идем с следующей позиции в строке
		if (pos == (*text).length())//если конец тейста
			return;
	}
}

//пропуск комментариев многострочных
void CLexic::skipComments()
{
	while (pos < (*text).length() && (*text)[pos] != '}') {//проходим строку пока не закончится комментарий или строка
		if ((*text)[pos] == '\n') {
			lineNum++;//переходим к след строке
			lastNewLinePos = pos;//запоминаем похицию последнего конца строки
		}
		pos++;
	}
	// пропускаем '}'
	if (pos < (*text).length())
		pos++;
	else
		errorManager->addError(pos - lastNewLinePos, lineNum, 86);//запускаем ошибку, что комментарий не закрыт
}

//получить  лексему
string CLexic::getLexem()
{
	int oldPos = pos;//начальное положение запоминаем
	string res = "";
	switch ((*text)[oldPos])
	{
		//если это строка с ковычками
	case '\'':
		oldPos++;
		res += "\'";
		while (oldPos < (*text).length() && (*text)[oldPos] != '\'' && (*text)[oldPos] != '\n') {
			res += (*text)[oldPos];//собираем пока не конец строки или закрывающая ковычка
			oldPos++;
		}
		if (oldPos == (*text).length()) {//если при этом это конец файла
			errorManager->addError(oldPos - lastNewLinePos, lineNum, 75);//ошибка в символьной константе 
			return res;
		}

		if ((*text)[oldPos] != '\'') {//если при этом закончили ан конце строки
			errorManager->addError(oldPos - lastNewLinePos, lineNum, 75);//ошибка в символьной константе
			return res;
		}
		res += "\'";
		return res;//возвращаем символьную константу строку/символ в кавычках
	case ':':
		if ((*text)[oldPos + 1] == '=')//проверяем не знак ли присваивания это
			return ":=";
		else return ":";
	case '<':
		if ((*text)[oldPos + 1] == '=')//проверки приверженности сложным знакам
			return "<=";
		else if (((*text)[oldPos + 1] == '>'))
			return "<>";
		else return "<";
	case '>':
		if ((*text)[oldPos + 1] == '=')//проверка приверженности  знаку >=
			return ">=";
		else return ">";
	case '.':
		if ((*text)[oldPos + 1] == '.')//проверка не является ли это многотчием
			return "..";
		else return ".";

		// однострочный комментарий 
	case '/': {
		if ((*text)[oldPos + 1] == '/') {//если это в итоге часть комментария, то
			passToNewLine();//переходим на ковую линиют t.k.
			passWhitespaces();//пропускаем пробелы
			return getLexem();//получаем следующий символ
		}
		else return "/";//если не коммент в итоге то просто

	}


	case '+': case '-': case '*': case '^':
	case '(': case ')': case '[': case ']':
	case ';': case ',': 
	case '=':
		return res += (*text)[oldPos];//записываем в результирующую строку

		// многострочный комментарий
	case '{': {
		skipComments();//вызов пропуска многострочного комментария
		if (pos < (*text).length()) {
			passWhitespaces();//пропускаем пробелы
			string tmp = getLexem();//смотрим след лексему
			return tmp;
		}
		else {
			return "";
		}
	}
	default:
		break;
	}

	// проверяем, что это не является неопознанным символом
	// возможно число или ид
	if (isDigit((*text)[oldPos]) || isLetter((*text)[oldPos]) ||    //проверка на число, букву или _  если один из них то 
		(*text)[oldPos] == '_') {
		while (isDigit((*text)[oldPos]) || isLetter((*text)[oldPos]) ||//читаем пока встречаем число, букву или _  или .
			(*text)[oldPos] == '_' || (*text)[oldPos] == '.') {

			// отдельно проверяется не является ли лексема "end" (отделить end и .)
			res += (*text)[oldPos];
			if (toLower(res) == "end") { //проверяем на end 
				if (oldPos < (*text).length() && (*text)[oldPos + 1] == '.') { //проверям наличие точки в след позиции и что эта позиция не пояледняя
					return toLower(res); //возвращаем  end
				}
			}
			oldPos++;
		}
		// если есть 
		auto found = res.find("..");//проверяем нет ли там многоточия в полученной лексеме
		if (found != string::npos) {//если есть .. то 
			res = res.substr(0, found);// в res кладем что что до ..
		}
		return toLower(res);// в нижнем регистре вохвращаем собранное слово/число/имя 
	}
	// если ничего из вышеперечисленного, то неопределенный символ
	res += (*text)[oldPos];
	errorManager->addError(oldPos, lineNum, 6);
	return toLower(res);//возвращаем лексему в нижнем регистре 
}

//инициализация объекта лексера  с закидкой в него текста  и менеджера ошибок
CLexic::CLexic(CErrorManager* errorManager, const string *text)
{
	this->errorManager = errorManager;
	this->text = text;
	this->pos = 0;
	factory = CTokenFactory();
}


//вызов нового токена 
unique_ptr<CToken> CLexic::getNext(bool get)
{
	int _lineNum = lineNum;
	int _lastNewLinePos = lastNewLinePos;
	int _lastLexemStartPos = lastLexemStartPos;
	int _pos = pos;
	if (pos >= (*text).length()) return nullptr;// возвращаем nullptr, если достигли конца файла
	passWhitespaces();//пропускаем пробеллы
	
	if (pos >= (*text).length()) {//если конец файла
		if (!get)setOldPos(_pos, _lineNum, _lastNewLinePos, _lastLexemStartPos);//записать старого значения
		return nullptr;
	}

	string lexem = getLexem();//получаем лексему по текущей позиции
	if (pos >= (*text).length()) {//
		if (!get)setOldPos(_pos, _lineNum, _lastNewLinePos, _lastLexemStartPos);//записать старого значения
		return nullptr;
	}

	//cout << "Lexemа = " << lexem << endl; //вывод лексера / разбиение на лексеры
	lastLexemStartPos = pos;//запоминаем начало последней лексемы

	if (get) {
		pos += lexem.length();//переходим к концу полученной лексемы
	}
	else setOldPos(_pos, _lineNum, _lastNewLinePos, _lastLexemStartPos);//заисываем старое значение
	return factory.createToken(lexem);//создаем токен по полученной лексеме
}

//заполнение переменных
void CLexic::setOldPos(int _pos, int _lineNum, int _lastNewLinePos, int _lastLexemStartPos) {
	lineNum = _lineNum;
	lastNewLinePos = _lastNewLinePos;
	lastLexemStartPos = _lastLexemStartPos;
	if (_pos > 0) pos = _pos;
}


// Возвращает индекс начала последней лексемы в строке
int CLexic::getStartPosition()
{
	return lastLexemStartPos - lastNewLinePos;
}

//возвращает текущую строку
int CLexic::getCurLine()
{
	return lineNum;
}

//переходим на новую линию в комменте пока только
void CLexic::passToNewLine()
{
	do {//пропускаем пока не наткнемся на конец файла или конец строки
		pos++;
		if (pos >= (*text).length())
			return;
	} while ((*text)[pos] != '\n');
	pos++;	// пропуск \n 
	lineNum++;
	lastNewLinePos = pos;
}

//скип пока не попадем на лексему из списка lexemes 
unique_ptr<CToken> CLexic::skip(set<string> lexemes) { //заходит сет 
	string lexem = "";
	if (pos >= (*text).length()) return nullptr; //проверка на конец файла

	while (pos < (*text).length() && lexemes.find(lexem) == lexemes.end()) { //пока не конец и лексема не из lexemes
		passWhitespaces();
		if (pos >= (*text).length()) return nullptr;

		lexem = toLower(getLexem());//получаем лексему 
		lastLexemStartPos = pos;
		passWhitespaces();
		pos += lexem.length();
	}
	if (pos >= (*text).length()) return nullptr;
	//cout << "LexemaAfterScip = " << lexem << endl;
	return factory.createToken(lexem);//создаем токен
}




//возвращает текущую позицию в файле
int CLexic::getCurPos()
{
	return pos;
}

// возвращает текущую позицию в строке
int CLexic::getCurPosInLine() {
	return pos - lastNewLinePos;
}

