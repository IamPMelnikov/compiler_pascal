#include "CToken.h"
CToken::~CToken() {}


CToken::CToken(ETokenType type)//создание с полученным типом
{
	this->m_T = type;
}


CValueToken::CValueToken(CVariant val) : CToken(VALUE) { // создание токена представляющего некое значение
	this->m_val = val;
}
CValueToken::~CValueToken() {}


CIdentToken::CIdentToken(string lexem) : CToken(IDENT) {// создание токена представляющего некий идентификатор
	this->lexem = lexem;
}
CIdentToken::~CIdentToken() {}


COperToken::COperToken(string lexem) : CToken(OPER) { //создание токена представляющего некий оператор
	this->lexem = lexem;
}
COperToken::~COperToken() {}

CUndefToken::CUndefToken(string lexem) : CToken(UNDEF) {//создание неопределенного токена
	this->lexem = lexem;
}
CUndefToken::~CUndefToken() {}

