#include "CVariant.h"
#include <iostream>

CVariant::CVariant(EVarType type, string lexem) //������������� ���� � ������ �������
{
	this->identType = type;
	this->lexem = lexem;
}
EVarType CVariant::getType()//������� ���� 
{
	return identType;
}
CVariant::CVariant() { }
CVariant::~CVariant() {}

//���������� ��������� � ����� Integer
CIntVariant::CIntVariant(int val) : CVariant(INT, to_string(val))
{
	this->m_val = val;
}
CIntVariant::~CIntVariant() {};

//���������� ��������� � ����� real
CRealVariant::CRealVariant(float val) : CVariant(REAL, to_string(val))
{
	this->m_val = val;
}
CRealVariant::~CRealVariant() {};

//���������� ��������� � ����� string
CStringVariant::CStringVariant(string lexem) : CVariant(STRING, lexem) {
	this->m_val = lexem;
}
CStringVariant::~CStringVariant() {}

//���������� ��������� � ����� char
CCharVariant::CCharVariant(char ch) : CVariant(CHAR, to_string(ch)) {
	this->m_val = ch;
}
CCharVariant::~CCharVariant() {}