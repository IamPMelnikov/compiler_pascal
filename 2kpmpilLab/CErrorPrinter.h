#pragma once
#include "CError.h"
#include <list>

class CErrorPrinter {
public:
	string path = "";// путь по которому следует сохранять ошибки
	bool print(list<CError> errors) ;// метод для печати ошибок по пути path
	CErrorPrinter(string path);
	CErrorPrinter();
	~CErrorPrinter();
};
