#pragma once
#include<list>
#include "CError.h"
#include <unordered_map>
#include "CErrorPrinter.h"

using namespace std;

//�������� ������. �������� ������� ����� � �������� ������
//������ ������ ������
class CErrorManager {
public:
	CErrorPrinter* ErrPrint;
	list<CError> errors;		// ������ ������
	unordered_map<int, string> codeDict;		// ��� ������ � �� ��������
	CErrorManager(string path);
	~CErrorManager();
	bool setExceptionList(string path);		// ��������� ���� ������ � �� �������� �� �����. ���������� false ��� �������
	void printErrors(CErrorPrinter* printer);	// �������� ��������������� ������� � ������, ����������� ������
	void addError(int pos, int line, int code);		// �������� ������ � ���������� �� � ������ errors

};
