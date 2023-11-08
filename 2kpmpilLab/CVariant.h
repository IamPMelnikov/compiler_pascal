#pragma once
#include <string>
using namespace std;


// ¬озможные типы переменных и значений
enum EVarType {
	INT,
	REAL,
	STRING,
	BOOLEAN,
	CHAR
};


// јбстрактный родительский класс, который наследуюетс€ классами типов переменных
class CVariant {
public:
	CVariant();
	virtual ~CVariant();
	EVarType getType();//возварт типа
	string getLexem() { return lexem; } //возврат лексемы 
protected:
	EVarType identType;
	string lexem;
	CVariant(EVarType type, string lexem);//инициализаци€ типа и текста лексемы
};


//  ласс дл€ типа Integer
class CIntVariant : public CVariant {
	using CVariant::CVariant;
public:
	CIntVariant(int val);//объ€вление, присваивание значени€
	~CIntVariant();
	int getVal() { return m_val; }//возврат значени€
private:
	int m_val;
};


//  ласс дл€ типа Real
class CRealVariant : public CVariant {
	using CVariant::CVariant;
public:
	CRealVariant(float val); //объ€вление, присваивание значени€
	~CRealVariant();
	float getVal() { return m_val; }//возврат значени€
private:
	float m_val;
};


//  ласс дл€ типа String
class CStringVariant : public CVariant {
	using CVariant::CVariant;
public:
	CStringVariant(string val);//объ€вление, присваивание значени€
	~CStringVariant();
	string getVal() { return m_val; }//возврат значени€
private:
	string m_val;
};


//  ласс дл€ типа Char
class CCharVariant : public CVariant {
	using CVariant::CVariant;
public:
	CCharVariant(char val);//объ€вление, присваивание значени€
	~CCharVariant();
	char getVal() { return m_val; }//возврат значени€
private:
	char m_val;
};
