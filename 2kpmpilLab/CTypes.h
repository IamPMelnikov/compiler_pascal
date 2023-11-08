#pragma once
#include <string>
#include <list>

using namespace std;

//типы переменных (для семантики)
enum EType {
	eINT, eREAL, eSTRING, eCHAR, eBOOLEAN, eNONE
};

//класс отвечающий за тип переменных
class CType {
protected:
	EType type;//тип переменной
	CType(EType type); //конструктор

public:
	const EType getType() { return type; } //возвращает тип переменной
	~CType();//деструктор
};
class CNoneType : public CType {
	using CType::CType;
public:
	CNoneType();//конструктор наследует eNONE (в реализациии)
	~CNoneType();//деструктор
};

class CEnumType;

class CEnumElType : public CType {
	using CType::CType;
private:
	CEnumType* parent;
public:
	CEnumElType();
	CEnumElType(CEnumType* parent);
	~CEnumElType();
	void setParent(CEnumType* parent);
	CEnumType* getParent() { return parent; };
};

/// <summary>
/// Класс соответствующий простым (базовым) типам
/// </summary>
class CBaseType : public CType {
	using CType::CType;
public:
	CBaseType(EType type);
	~CBaseType();
};
class CEnumType : public CType {
	using CType::CType;
private:
	list<CEnumElType*> els;
public:
	CEnumType();
	~CEnumType();
	void addEl(CEnumElType* el);
};
