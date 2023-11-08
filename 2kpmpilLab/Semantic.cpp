#include "Semantic.h"
#include <stdexcept>
#include "CVariant.h"
#include <set>

using namespace std;

//создание нового объекта сем.ан, закидка в него объекта лекс.ан. и менеджер ошибок + 
CSemantic::CSemantic(CErrorManager* eManager, CLexic* lexic) {
	this->eManager = eManager;
	this->lexic = lexic;

	createFictiveScope();//создание первой области видмости
	createScope();
}
CSemantic::~CSemantic() {}

//создание первой области видмости
void CSemantic::createFictiveScope() {
	// integer, real, string, char - BaseType, true, false - EnumType
	scopesLst.push_back(CScope(nullptr, this->lexic, this->eManager)); //для scopesLst объекта первым ставим нулевой score
	auto fictiveScp = &scopesLst.back();//ссылка на последний scope
	(*fictiveScp).createFictive();
}

void CSemantic::createScope() {
	scopesLst.push_back(CScope(&scopesLst.back(), lexic, eManager));
}


// проверка выражений 
EType CScope::unionBoolExprTypes(EType left, EType right) {
	if (left == eNONE || right == eNONE)
		return eNONE;
	if (left == eREAL && (right == eREAL || right == eINT))
		return eBOOLEAN;
	if (right == eREAL && (left == eREAL || left == eINT))
		return eBOOLEAN;
	if (left == eINT && right == eINT)
		return eBOOLEAN;
	if ((left == eSTRING || left == eCHAR) && (right == eSTRING || right == eCHAR))
		return eBOOLEAN;
	if (left == eBOOLEAN && right == eBOOLEAN)
		return eBOOLEAN;
	writeMistake(328);
	return eNONE;
}

//проверка типа для виражений
EType CScope::unionTypes(EType left, EType right, string oper) {
	if (left == eNONE || right == eNONE)
		return eNONE;
	if (oper == "/") {
		if ((right == eREAL || right == eINT) && (left == eREAL || left == eINT))
			return eREAL;
		writeMistake(1004);
		return eNONE;
	}
	if (oper == "*") {
		if ((left == eSTRING || left == eCHAR) && right == eINT || (right == eSTRING || right == eCHAR) && left == eINT)
			return eSTRING;
		if ((left == eREAL) && (right == eREAL || right == eINT))
			return eREAL;
		if ((right == eREAL) && (left == eREAL || left == eINT))
			return eREAL;
		if (right == eINT && left == eINT)
			return eINT;
		writeMistake(1004);
		return eNONE;
	}
	if (oper == "+" || oper == "-") {
		if (oper == "+" && (left == eSTRING || left == eCHAR) && (right == eSTRING || right == eCHAR))
			return eSTRING;
		if ((left == eREAL) && (right == eREAL || right == eINT))
			return eREAL;
		if ((right == eREAL) && (left == eREAL || left == eINT))
			return eREAL;
		if (right == eINT && left == eINT)
			return eINT;
		writeMistake(1004);
		return eNONE;
	}
	if (oper == "div" || oper == "mod") {
		if ((left == eREAL) && (right == eREAL || right == eINT))
			return eREAL;
		if ((right == eREAL) && (left == eREAL || left == eINT))
			return eREAL;
		if (right == eINT && left == eINT)
			return eINT;
		writeMistake(1004);
		return eNONE;
	}
	if (oper == "and" || oper == "or") {
		if (left == eBOOLEAN && right == eBOOLEAN)
			return eBOOLEAN;
		writeMistake(1004);
		return eNONE;
	}
	writeMistake(1004);
	return eNONE;

}
list<string> CScope::getNamesBuff() {
	return namesBuff;
}
EType CScope::getBuffType() {
	if (typesBuff.empty())
		return eNONE;
	return typesBuff.front()->getType();
}

//проверка выражения для опреаторов сравнения
EType CSemantic::unionBoolExprTypes(EType left, EType right) {
	return scopesLst.back().unionBoolExprTypes(left, right);
}
//проверка выражения с оператором
EType CSemantic::unionTypes(EType left, EType right, string oper) {
	return scopesLst.back().unionTypes(left, right, oper);
}

CScope::CScope(CScope* outerScope, CLexic* lexic, CErrorManager* eManager) {
	this->outerScope = outerScope;
	this->lexic = lexic;
	this->eManager = eManager;
}

CScope::~CScope() {}

CType CScope::makeType(EType type) {
	switch (type) {
	case eINT: case eREAL: case eSTRING: case eCHAR:
		return CBaseType(type);
	default:
		return CNoneType();
	}
}

// определяет семант тип переменной из синт, находит тип передаваемого идент
EType CScope::defineType(EVarType type, string identName) {
	// попытка найти и определить тип идентификатора
	if (identName != "") {
		auto typePtr = findType(identName, set<EBlock>{VARBL, BODYBL});
		if (typePtr == nullptr) {
			writeMistake(1002);
			typeTbl.push_back(CNoneType());
			identTbl.insert({ identName, CIdetificator(identName, BODYBL, &typeTbl.back()) });
			return eNONE;
		}
		else return typePtr->getType()->getType();
	}
	switch (type) {
	case INT:
		return eINT;
	case REAL:
		return eREAL;
	case STRING:
		return eSTRING;
	case CHAR:
		return eCHAR;
	default:
		return eNONE;
	}
}

EType CScope::checkAssignTypes(string name, EType right) {
	auto leftPtr = findType(name, set<EBlock>{ VARBL, BODYBL});
	if (leftPtr == nullptr) {
		writeMistake(1002);
		typeTbl.push_back(CNoneType());
		identTbl.insert({ name, CIdetificator(name, BODYBL, &typeTbl.back()) });
		return eNONE;
	}
	else if (leftPtr->getType()->getType() == eNONE || right == eNONE) {
		return leftPtr->getType()->getType();
	}
	else {
		if (leftPtr->getType()->getType() == eREAL && (right == eREAL || right == eINT))
			return leftPtr->getType()->getType();
		if (leftPtr->getType()->getType() == eINT && right == eINT)
			return leftPtr->getType()->getType();
		if (leftPtr->getType()->getType() == eSTRING && (right == eSTRING || right == eCHAR))
			return leftPtr->getType()->getType();
		if (leftPtr->getType()->getType() == eCHAR && right == eCHAR)
			return leftPtr->getType()->getType();
		if (leftPtr->getType()->getType() == eBOOLEAN && right == eBOOLEAN)
			return leftPtr->getType()->getType();
		writeMistake(328);
	}

}

//чистим буфер типов
void CScope::clearTypesBuff() {
	typesBuff.clear();
}
//чистим буфер имен и перекидкав таблицу идентификаторов
void CScope::clearNamesBuff() {
	CType* none = nullptr;
	
	if (namesBuff.empty()) return; //в основном в начале
	if (typesBuff.empty()) {// если произошла какая-то ошибка в синтаксисе и не был передан тип 
		typeTbl.push_back(CNoneType());
		for (auto name : namesBuff) {
			identTbl.insert({ name, CIdetificator(name, flagBlock, &typeTbl.back()) });
		}
		namesBuff.clear();
		return;
	}
	// для всех типо в typesBuff проверяем ...
	for (auto type : typesBuff) {
		if (type->getType() == eNONE) {// если хотя бы один из типов не корректен, то и тип, включающий его также некорректен
			none = type;
			break;
		}
	}
	if (none != nullptr) {//если есть eNONE типы
		for (auto name : namesBuff) {
			// если уже объвлена, то кидаем ошибку и присваиваем повторенному имени тип NONE
			if (identTbl.find(name) != identTbl.end()) {
				writeMistake(101);
				identTbl.find(name)->second = CIdetificator(name, flagBlock, none);
			}
			else identTbl.insert({ name, CIdetificator(name, flagBlock, none) });		// создаем новый идентификатор типа eNONE
		}
	}
	else
		// если тип определен корректно, то присваиваем его всем переменным
		for (auto name : namesBuff) {
			if (identTbl.find(name) != identTbl.end()) {
				// если уже объвлена, то кидаем ошибку и присваиваем тип NONE
				writeMistake(101);
				typeTbl.push_back(CNoneType());
				identTbl.find(name)->second = CIdetificator(name, flagBlock, &typeTbl.back());
			}
			else
				identTbl.insert({ name, CIdetificator(name, flagBlock, typesBuff.front()) });
		}
	namesBuff.clear();//чистим  буфер имен
}
//вставляем в список имен  идентификаторов новое 
void CScope::addToNameBuffer(string name) {
	namesBuff.push_back(name);
}

void CScope::createNone() {
	typeTbl.push_back(CNoneType());
	typesBuff.push_back(&typeTbl.back());
}

//добавляем в буфер имя нового типа
void CScope::addToBuffer(string typeName) {
	auto type = findType(typeName, set<EBlock>{TYPEBL});//проверяем это имя типа и находим соответсвтующий иддентификатор
	// если тип не найден
	if (type == nullptr) {
		writeMistake(1002);
	}
	else {//если есть тип то вставляем в буфер типов
		typesBuff.push_back(type->getType());
	}
}

//чистим буферы  
void CScope::clearBuffs() {
	clearNamesBuff();//
	clearTypesBuff();//
}

//ставим идентификатор раздела
void CScope::setBlock(EBlock block) {
	flagBlock = block;
}

//находит идентификатор по названию, проходится по всем скоупам
CIdetificator* CScope::findType(string name, const set<EBlock> block) {
	CIdetificator* type = nullptr;
	if (identTbl.find(name) != identTbl.end()) {//если есть такое имя в таблице идентификаторов то 
		// идентификатор найден
		if (block.find((identTbl[name]).getBlock()) != block.end()) {//если идентификатор соответствующий полученному имени name находится в переданном блоке  block
			return (&identTbl[name]);// тип найден, возвращаем идентификатор
		}
		else {
			// идентификатор найден, но это не того типа
			return nullptr;
		}
	}
	else {// нет такого имени в таблице идентификаторов
		if (outerScope != nullptr)//если есть объемлющая область действия 
			type = outerScope->findType(name, block);//то ищем в ней идентификатор по заданном у названию 
		else return nullptr;
	}
	return type;//возвращаем идентификатор
}

void CScope::writeMistake(int code) {
	cout << "Mistake " << code << ": " << lexic->getStartPosition() << endl;
	eManager->addError(lexic->getStartPosition(), lexic->getCurLine(), code);
}


//создание первых данных для фиктивного(нулевого) scope - окружения
void CScope::createFictive() {
	typeTbl.push_back(CBaseType(eINT));//закидываем в таблицу типов
	identTbl.insert({ "integer", CIdetificator("integer", TYPEBL, &(typeTbl.back())) }); //закидываем в таблицу идентификаторов

	typeTbl.push_back(CBaseType(eREAL));
	identTbl.insert(pair<string, CIdetificator>("real", CIdetificator("real", TYPEBL, &(typeTbl.back()))));

	typeTbl.push_back(CBaseType(eSTRING));
	identTbl.insert(pair<string, CIdetificator>("string", CIdetificator("string", TYPEBL, &(typeTbl.back()))));

	typeTbl.push_back(CBaseType(eCHAR));
	identTbl.insert(pair<string, CIdetificator>("char", CIdetificator("char", TYPEBL, &(typeTbl.back()))));

	// boolean
	typeTbl.push_back(CEnumType());
	//auto boolPtr = &(typeTbl.back());
	identTbl.insert({ "boolean", CIdetificator("boolean", TYPEBL, &(typeTbl.back())) });
	typeTbl.push_back(CEnumElType());
	identTbl.insert({ "true", CIdetificator("true", VARBL, &(typeTbl.back())) });
	typeTbl.push_back(CEnumElType());
	identTbl.insert({ "false", CIdetificator("false", VARBL, &(typeTbl.back())) });

	// создание предопределенных констант
	typeTbl.push_back(CBaseType(eINT));
	identTbl.insert({ "maxint", CIdetificator("maxint", VARBL, &(typeTbl.back())) });
	typeTbl.push_back(CBaseType(eINT));
	identTbl.insert({ "minint", CIdetificator("minint", VARBL, &(typeTbl.back())) });
	typeTbl.push_back(CBaseType(eREAL));
	identTbl.insert({ "pi", CIdetificator("pi", VARBL, &(typeTbl.back())) });
}

//конструктор класса идентификатора с заданием переданных значений
CIdetificator::CIdetificator(string name, EBlock block, CType* type) {
	this->name = name;
	this->block = block;
	this->type = type;
}
//конструктор
CIdetificator::CIdetificator() {}
//деструктор
CIdetificator::~CIdetificator() {}
