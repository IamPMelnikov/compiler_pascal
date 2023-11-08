#include "CErrorManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

//создание объекта + вызов чтения файла с ошибками
CErrorManager::CErrorManager(string path) {
	setExceptionList(path);
}

CErrorManager::~CErrorManager() {}

//чтение 
bool CErrorManager::setExceptionList(string path) {  //получаем Path файла с списком кодов ошибок и описанием ошбок 
	string line, descr("");
	ifstream input(path);
	int code;

	if (!input.is_open())//поверка 
		return false;

	while (getline(input, line)) {
		descr = "";
		istringstream iss(line);//создали поток ввода
		iss >> code;//читаем первый символ в строке - это код ошибки
		descr = line.substr(iss.tellg(), line.length()); //читаем остальную строку - описание

		codeDict.insert({ code, descr });//кидаем в мапу по типу код-описание
	}
	input.close();

	return true;
}

//печать ошибок
void CErrorManager::printErrors(CErrorPrinter* printer) {
	printer->print(errors);
}

//добавление новых ошибок
void CErrorManager::addError(int pos, int line, int code) {
	errors.push_back(CError(pos, line, code, codeDict[code]));//простая вставка  всписок нового объекта класса CError
}


