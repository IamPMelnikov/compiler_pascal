#include "CErrorManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

//�������� ������� + ����� ������ ����� � ��������
CErrorManager::CErrorManager(string path) {
	setExceptionList(path);
}

CErrorManager::~CErrorManager() {}

//������ 
bool CErrorManager::setExceptionList(string path) {  //�������� Path ����� � ������� ����� ������ � ��������� ����� 
	string line, descr("");
	ifstream input(path);
	int code;

	if (!input.is_open())//������� 
		return false;

	while (getline(input, line)) {
		descr = "";
		istringstream iss(line);//������� ����� �����
		iss >> code;//������ ������ ������ � ������ - ��� ��� ������
		descr = line.substr(iss.tellg(), line.length()); //������ ��������� ������ - ��������

		codeDict.insert({ code, descr });//������ � ���� �� ���� ���-��������
	}
	input.close();

	return true;
}

//������ ������
void CErrorManager::printErrors(CErrorPrinter* printer) {
	printer->print(errors);
}

//���������� ����� ������
void CErrorManager::addError(int pos, int line, int code) {
	errors.push_back(CError(pos, line, code, codeDict[code]));//������� �������  ������� ������ ������� ������ CError
}


