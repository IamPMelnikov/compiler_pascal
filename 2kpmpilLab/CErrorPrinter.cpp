#include "CErrorPrinter.h"
#include <fstream>
#include <iostream>
#include "ToLowerStr.h"

//������ ������
bool CErrorPrinter::print(list<CError> errors)
{
	if (path == "") return false;
	ofstream myfile(path);

	if (!myfile.is_open())
		return false;

	for (CError er : errors) {
		myfile << "code:" << er.code << "  l/p ( " << er.line << ":" << er.pos << " )  - " << er.description<<endl;
	}
	myfile.close();
	return true;
}

//�������� ������� + ������� � ���� ��������� ����� ��� ������
CErrorPrinter::CErrorPrinter(string path)
{
	this->path = path;
}

CErrorPrinter::CErrorPrinter() {}

CErrorPrinter::~CErrorPrinter() {}
