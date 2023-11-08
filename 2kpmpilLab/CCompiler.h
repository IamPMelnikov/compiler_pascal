#pragma once
#include "Lexic.h"
#include "Semantic.h"

class CCompiler {
private:
	CErrorManager* erManager;//менеджер ошибок
	CLexic* lexic;// объект класса лексического анализатора
	CSemantic* semantic;// объект класса семантического анализатора
	unique_ptr<CToken> curToken;//нынешний токен

	void getNext();		// получить следующий токен
	void removeToken();	// очистить память
	void skip(set<string> lexem);	// пропуск до определенной лексемы. Лексема curToken равна одной из переданных или nullptr
	bool skipUntilBlock(set<string> lexemes, string word);// пропуск до начала какого-либо блока. Возвращает истину, если был осуществлен пропуск

	void accept(string oper);	// принять операцию
	string acceptSign();			// возвращает знак или пустую строку, принимает знак

	void program(); // обработка имени программы
	string name();// проверяет если текущий токен - идентификатор то возврщает его имя
	string var(set<string> skippingSet);// если пустой пропускной сет, кидает ошибку
	string fullVar();//обработка полной переменной

	void block(); //обработка программы после блока program
	void blockTypes();//обработка раздела объявления типов
	void blockVars();//обработка раздела объявления переменных
	void blockOpers();//обработка раздела операторов

	void descrMonotypeVars();// пропуск до след имени

	string type();//обработка типа (простого типа) 
	void typeDef();//обработка определения типа 

	void diffOper(set<string> skippingSet);// сложный оператор
	void compoundOper(set<string> skippingSet);// составной оператор
	void oper(set<string> skippingSet);// оператор
	void unmarkedOper(set<string> skippingSet);
	void ifOper(set<string> skippingSet);//обработка if
	void whileOper(set<string> skippingSet);//обработка while
	void simpleOper(set<string> skippingSet);
	void assignOper(set<string> skippingSet);// оператор присваивания


	EType expression(set<string> skippingSet);//обработка выражения
	EType simpleExpr();
	EType term();//обработка слагаемого
	EType factor();//Обработка множителя

	bool isBoolOper();// является ли операцией отношения
	bool isAdditiveOper();//является ли аддит. операцией
	bool isMultOper();//является ли мультипл. операцией
	bool checkOper(string oper);// совпадает ли оператор

	bool ifNullThrowExcp();
	void checkForbiddenSymbol();//проверка запрещенного символа
	void writeMistake(int code);
	void writeMistake(int code, int pos, int line);

	bool eTypeIsDefine(EType type);
public:
	CCompiler(CErrorManager* erManager, CLexic* lexic, CSemantic* semantic);//конструктор
	~CCompiler();//деструктор
	void startVer();//ЗАПУСК компилятора
};

/// <summary>
/// Паскаль - ошибка
/// </summary>
struct PascalExcp : public std::exception {
	const char* what() const throw() {
		return "Some exception in Pascal code";
	}
};

/// <summary>
/// Паскаль-ошибка конца файла
/// </summary>
struct EOFExcp : public std::exception {
	const char* what() const throw() {
		return "Reached eof";
	}
};