#pragma once
#include <list>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include "CTypes.h"
#include <set>
#include <unordered_map>
#include "CErrorManager.h"
#include "Lexic.h"

using namespace std;

// переменная может быть не объявлена, однако использоваться, тогда она заносится с меткой BODY и типом NULL (универсальный тип)
enum EBlock { TYPEBL, VARBL, BODYBL };//блок тип, переменная, универсаный (там может быть например необъявленная еще переменная

class CIdetificator {
private:
	string name;//самое имя
	EBlock block;//раздел программы которому принадлежит
	CType* type;//тип идентификатора
public:
	CIdetificator(string name, EBlock block, CType* type);//конструктор с заполнением переданных данных
	CIdetificator();//конструктор
	~CIdetificator();//деструктор
	string getName() const { return name; }//возврат имени
	EBlock getBlock() const { return block; } //возврат раздела программы(блока)
	CType* getType() { return type; }//возврат типа 
};

//клас для областей действия
class CScope {
private:
	CLexic* lexic; // лексер
	CErrorManager* eManager; // менеджер ошибок

	list<CType*> typesBuff;	// буфер создаваемых составных типов
	list<string> namesBuff;	// буфер имен однотипных переменных
	EBlock flagBlock;// флаг, отвечающий за текущий блок

	CScope* outerScope;	// внешняя область действия
	unordered_map<string, CIdetificator> identTbl;// таблица идентификаторов
	list<CType> typeTbl;// таблица типов

	CIdetificator* findType(string name, set<EBlock> block);// находит идентификатор по названию, проходится по всем скоупам
	void writeMistake(int code);
public:
	CScope(CScope* outerScope, CLexic* lexic, CErrorManager* eManager);
	~CScope();
	void createFictive();// создание базовых типов для фиктивного скоупа

	CType makeType(EType type);	// адаптер для синт\сем типов. Возвращает новый объект соотв класса
	void clearTypesBuff();	// очистка буфера типов
	void clearNamesBuff();// добавляет в ТИ идентификаторы 
	void clearBuffs();// очищает оба буфера
	void addToNameBuffer(string name);// добавление в буфер имен
	void addToBuffer(string typeName);// добавление в буфер типов
	void createNone();// создание типа none
	void setBlock(EBlock block);		// назначение текущего блока
	EType defineType(EVarType type, string identName);// определяет семант тип переменной из синт, находит тип передаваемого идент
	EType checkAssignTypes(string name, EType right);

	EType unionBoolExprTypes(EType left, EType right);// проверка выражений булев.
	EType unionTypes(EType left, EType right, string oper);// проверка выражений 

	list<string> getNamesBuff();
	EType getBuffType();
};

//класс семантического анализатора
class CSemantic {
private:
	list<CScope> scopesLst;	//список областей действия   нулевую позицию занимает фиктивный скоуп
	CErrorManager* eManager;//менеджер ошибок
	CLexic* lexic;//лексический анализатор
public:
	CSemantic(CErrorManager* eManager, CLexic* lexic);// конструктор
	~CSemantic();//деструктор
	void createFictiveScope(); //создание фиктивного скоупа (области действия)
	void createScope();// создание области действия
	CScope* getLast() { return &scopesLst.back(); }//возврат последней области действия (верхней)

	EType unionBoolExprTypes(EType left, EType right);// проверка выражений булев
	EType unionTypes(EType left, EType right, string oper);// проверка выражений 
};

