#pragma once
#include<string>

using namespace std;

// Класс базовой ошибки. Может наследоваться другими специфичными классами ошибок
// содержит код ошибки, описание, позицию, доп агрументы для описания
class CError
{
public:
	int pos;		// позиция в строке
	int line;		// номер строки
	int code;		// код ошибки
	string description;	// описание ошибки
	CError(int pos, int line, int code, string description);
	virtual ~CError();
};
