#pragma once
#include "CError.h"
#include <list>

class CErrorPrinter {
public:
	string path = "";// ���� �� �������� ������� ��������� ������
	bool print(list<CError> errors) ;// ����� ��� ������ ������ �� ���� path
	CErrorPrinter(string path);
	CErrorPrinter();
	~CErrorPrinter();
};
