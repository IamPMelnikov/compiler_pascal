#pragma once
#include<list>
#include "CError.h"
#include <unordered_map>
#include "CErrorPrinter.h"

using namespace std;

//Менеджер ошибок. Содержит словарь кодов и описания ошибок
//хранит список ошибок
class CErrorManager {
public:
	CErrorPrinter* ErrPrint;
	list<CError> errors;		// список ошибок
	unordered_map<int, string> codeDict;		// код ошибки и ее описание
	CErrorManager(string path);
	~CErrorManager();
	bool setExceptionList(string path);		// считывает коды ошибок и их описания из файла. Возвращает false при неудаче
	void printErrors(CErrorPrinter* printer);	// вызывает соответствующую функцию у класса, печатающего ошибки
	void addError(int pos, int line, int code);		// создание ошибки и добавление ее в список errors

};
