#include "CVariant.h"
#include <iostream>

CVariant::CVariant(EVarType type, string lexem) //инициализаци€ типа и текста лексемы
{
	this->identType = type;
	this->lexem = lexem;
}
EVarType CVariant::getType()//возврат типа 
{
	return identType;
}
CVariant::CVariant() { }
CVariant::~CVariant() {}

//объ€вление пременной с типом Integer
CIntVariant::CIntVariant(int val) : CVariant(INT, to_string(val))
{
	this->m_val = val;
}
CIntVariant::~CIntVariant() {};

//объ€вление пременной с типом real
CRealVariant::CRealVariant(float val) : CVariant(REAL, to_string(val))
{
	this->m_val = val;
}
CRealVariant::~CRealVariant() {};

//объ€вление пременной с типом string
CStringVariant::CStringVariant(string lexem) : CVariant(STRING, lexem) {
	this->m_val = lexem;
}
CStringVariant::~CStringVariant() {}

//объ€вление пременной с типом char
CCharVariant::CCharVariant(char ch) : CVariant(CHAR, to_string(ch)) {
	this->m_val = ch;
}
CCharVariant::~CCharVariant() {}