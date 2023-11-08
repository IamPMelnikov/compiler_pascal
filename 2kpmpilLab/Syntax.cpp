#include "CCompiler.h"
#include <iomanip>
#include "ToLowerStr.h"


//инициализация 
CCompiler::CCompiler(CErrorManager* erManager, CLexic* lexic, CSemantic* semantic) {
	this->erManager = erManager;
	this->lexic = lexic;
	this->semantic = semantic;
}

CCompiler::~CCompiler() {}

//ЗАПУСК компилятора
void CCompiler::startVer() {
	try { getNext(); } // просим новый токен    (работа лексического анализатора)
	catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl; 
		return;
	}
	catch (EOFExcp& e) {
		cout << "[!]EOF EXCEPTION" << endl;
		return;
	}


	try { ifNullThrowExcp(); }// проверка на неожиданй конец файла
	catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl;
		return;
	}
	catch (EOFExcp& e) {
		cout << "[!]EOF EXCEPTION" << endl;
		return;
	}

	try { program(); }//основная часть
	catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl;
		return;
	}
	catch (EOFExcp& e) {
		cout << "[!]EOF EXCEPTION" << endl;
		return;
	}
}

//функция для вызова перемещения указателя старого токена и создания нового
void CCompiler::getNext() throw(PascalExcp, EOFExcp) {
	removeToken();
	curToken = this->lexic->getNext(true);//выбираем следующий рассматриваемый токен
}


//функция пропуска синтаксической ошибки
void CCompiler::skip(set<string> lexemes) {
	if (curToken == nullptr) return; // 
	// если уже на нужном слове
	if (curToken->getType() == OPER //если оператор и есть в заданном множестве lexemes
		&& lexemes.find(((COperToken*)curToken.get())->getLexem()) != lexemes.end()) {
		return;
	}
	removeToken();// если не из lexemes то переходим к следующему токену
	curToken = this->lexic->skip(lexemes);
}

//перемещам указатель токена 
void CCompiler::removeToken() {
	if (curToken != nullptr) //если не  nullptr делаем nullptr
		curToken.release();
}

void CCompiler::program() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();//проверка на конец файла 
	set<string> nameSet = { "(", ",", ";", "var", "begin" };//списки до чего скипать
	set <string> branchSet = { ";", "var", "begin" };
	set <string> semicolSet = { "var", "begin" };
	accept("program");//проверяем что прога начинается с program и вызываем новый токен

	string programName = "";//для названия программы
	try { programName = name(); } //проверяем наличие имени (идентификатор) для названия программы
	catch (PascalExcp& e) {//если не идентификатор
		skip(nameSet); // то скипаем скипаем до "(", ",", ";", "var", "begin"
	}

	//проверяем ожидания дальше
	try { accept("("); }//скобка
	catch (PascalExcp& e) {
		skip(nameSet);
	}
	try { name(); }//идентификатор
	catch (PascalExcp& e) {
		skip(nameSet);
	}
	while (checkOper(",")) {// пока получаем зпт переберам идентификаторы 
		getNext();//раз увидели ","  то пропускаем её 
		try { name(); }
		catch (PascalExcp& e) {
			skip(nameSet);
		}
	}
	try { accept(")"); }//скобку проверяем
	catch (PascalExcp& e) {
		skip(branchSet);//скипаем уже будто скобка была
	}
	try { accept(";"); }//точку с зпт
	catch (PascalExcp& e) {
		skip(branchSet);//скипаем будто ; была
	}

	block();// с этого момента ожидаем что угодно

	accept(".");//заключительная точка после end
	if (curToken != nullptr) { //если что-то еще есть то мы этого не ожидаем соответсвтенно ошибка
		writeMistake(1001);
		throw PascalExcp();
	}
}

// ожидаем идентификатор если не он то ошибка если он то возвращем имя его и берем следующий токен
string CCompiler::name() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp(); //проверка конца файла
	if (curToken->getType() != IDENT) { //проверка токена на тип )ожидаем идентификатор)
		writeMistake(2);//если не идентификатор то ошибку пишем
		throw PascalExcp();
	}
	string nameStr = ((CIdentToken*)curToken.get())->getLexem();
	getNext(); //переходим на следующий токен
	return nameStr;//возвращаем имя идентификатора 
}

//
bool CCompiler::skipUntilBlock(set<string> searchingLexemes, string searchingWord) throw(PascalExcp, EOFExcp) {
	// проверяем находимся ли на лексема  
	// если нет, пытаемся скипнуть до  или до начала следующих блоков
	ifNullThrowExcp();//проверка на конец файла 
	int oldPosInLine = lexic->getStartPosition(); 
	int oldLineNum = lexic->getCurLine();

	if (curToken->getType() != OPER) { //если не оператор
		skip(searchingLexemes); //пропускамем до вхождения в заданное множетсво searchingLexemes  (до начала одного из ожидаемых разделов)
		writeMistake(6, oldPosInLine, oldLineNum);//запрещенный/неожиданный символ
		return true;
	}
	else {//если оператор
		if (((COperToken*)curToken.get())->getLexem() != searchingWord) {//проверяем совпадает ли с искомым именем если да то все хорошо и мы идем дальше, если нет то то пропускаем до  оператора из searchingLexemes
			int oldPos = lexic->getCurPos();
			skip(searchingLexemes);
			int curNewPos = lexic->getCurPos();
			if (curNewPos != oldPos)
				writeMistake(6, oldPosInLine, oldLineNum);
			return true;
		}
	}

	return false;
}

//обработка переменной
string CCompiler::var(set<string> skippingSet)throw(PascalExcp, EOFExcp) {// у нас только полная
	//<переменная>::=<полная переменная>|<компонента переменной> | <указанная переменная>
	ifNullThrowExcp();
	return fullVar();
}

//обработка полной переменной (имя переменной получаем) //
string CCompiler::fullVar() throw(PascalExcp, EOFExcp) {
	//<полная переменная>::=<имя переменной>
	return name();
}

//проход по основной чсти программы (между program  и end.)
void CCompiler::block() throw(PascalExcp, EOFExcp) {  
	ifNullThrowExcp();//проверка на конец файла
	
	set <string> blockTypeSet = { "type", "var", "begin" };//списоки возможных ожиданий
	set <string> blockVarsSet = { "var","begin" };
	set <string> blockOpersSet = { "begin" };
	
	skipUntilBlock(blockTypeSet, "type"); // пропускаем до "type", "var", "begin" 
	blockTypes(); //проверяем  раздел типов

	ifNullThrowExcp();
	skipUntilBlock(blockVarsSet, "var"); // пропускаем до  "var", "begin"
	blockVars();//проверяем раздел описания переменных

	ifNullThrowExcp();
	skipUntilBlock(blockOpersSet, "begin");// пропуск до "begin"
	blockOpers();//тело программы
}

//обработка раздела типов
void CCompiler::blockTypes() {
	//<раздел типов>::=<пусто>|type <определение типа>;{<определение типа>; }
	semantic->getLast()->setBlock(TYPEBL);
	semantic->getLast()->clearBuffs();//чистим буферы имен и типов
	ifNullThrowExcp();//проверка на конец файла 
	set <string> skipSet = { ";", "begin", "var" };
	if (checkOper("type")) {//проверка лексемы на соотвтствие ожидаемому оператору
		getNext();//берем следующий токен
		try { typeDef(); }//<определение типа>
		catch (PascalExcp& e) {
			skip(skipSet);
			semantic->getLast()->createNone();//закидываем неопределенный тип
		}
		semantic->getLast()->clearBuffs();
		// {<определение типа>;}
		while (checkOper(";")) {
			getNext();
			if (curToken != nullptr) {
				if (curToken->getType() == OPER && (//если наткнулись на начало следующего блока, то выходим 
					((COperToken*)curToken.get())->getLexem() == "begin" ||
					((COperToken*)curToken.get())->getLexem() == "var"))
					return;
			}
			else return; // неожиданный конец файла
			try { typeDef(); }//<определение типа>
			catch (PascalExcp& e) {
				skip(skipSet);
				semantic->getLast()->createNone();
			}
			semantic->getLast()->clearBuffs();
		}
	}
}

//обработка определения типа
void CCompiler::typeDef()throw(PascalExcp, EOFExcp) {
	//<определение типа>::= <имя>=<тип>
	semantic->getLast()->addToNameBuffer(name());// для области видимости в которой сейчас вставляем новое имя идентификатора
	accept("=");//проверяем на наличие =  и смотрим следующий токен
	auto typeName = type();//получаем имя типа
	semantic->getLast()->addToBuffer(typeName);//добавляем в буфер имя нового типа
}
//тип (только простой)
string CCompiler::type()throw(PascalExcp, EOFExcp) {
	// <тип>:: = <простой тип> 
	ifNullThrowExcp();//проверка на конец файла
	try {
		return name();	//<имя типа>
	}
	catch (PascalExcp& e) {
		writeMistake(324);// нет такого типа
		throw e;
	}
	return "";

}


// обработка раздела описания переменных
void CCompiler::blockVars() throw(PascalExcp, EOFExcp) {
	semantic->getLast()->setBlock(VARBL);//помечаем как раздел переменных
	semantic->getLast()->clearBuffs();//чистим буферы
	//<раздел переменных>::= var <описание однотипных переменных>;{<описание однотипных переменных>; } | <пусто>
	ifNullThrowExcp();//проверка на коннец файла
	set <string> skipSet = { ";", "begin" };
	// var <описание однотипных переменных>;
	if (checkOper("var")) {
		getNext();//переходим к следующему токену если var есть
		try { descrMonotypeVars(); }
		catch (PascalExcp& e) {
			skip(skipSet);
			semantic->getLast()->createNone();
		}
		semantic->getLast()->clearBuffs();
		// {<описание однотипных переменных>;}
		while (checkOper(";")) {// обработка других переменных 
			getNext();
			if (curToken != nullptr) {
				if (curToken->getType() == OPER && (//если попали на бегин то выходим
					((COperToken*)curToken.get())->getLexem() == "begin"))
					return;
			}
			else return;	// неожиданный конец файла
			try {
				descrMonotypeVars();
			}
			catch (PascalExcp& e) {
				skip(skipSet);
				semantic->getLast()->createNone();
			}
			semantic->getLast()->clearBuffs();
		}
	}
}

//обработка описания однотипных переменных  
void CCompiler::descrMonotypeVars() throw(PascalExcp, EOFExcp) {
	// <описание однотипных переменных>::=<имя>{,<имя>}:<тип>
	set <string> constDefSet = { ",", ":", ";", "begin" };
	try {
		semantic->getLast()->addToNameBuffer(name());//закидываем имя идентификатора (ожидается , что будет он)
	}
	catch (PascalExcp& e) {
		skip(constDefSet);//пропуск до одного из операторов constDefSet
	}
	while (checkOper(",")) {//если встречаем , то значит, что идет перечисение идентификаторов (имен переменных)
		getNext();
		try {
			semantic->getLast()->addToNameBuffer(name());//закидываем в буфер имя переменной
		}
		catch (PascalExcp& e) {
			skip(constDefSet);//пропуск до одного из операторов constDefSet 
		}
	}
	accept(":");//ожидаем :  и перееходим к следуюзему токену
	auto typeName = type();//берем следующий токен(тип) 
	semantic->getLast()->addToBuffer(typeName);//закидываем тип в буфер
}

// обработка раздела операторов
void CCompiler::blockOpers()throw(PascalExcp, EOFExcp) {
	semantic->getLast()->setBlock(BODYBL);//означаем текущий раздел 
	semantic->getLast()->clearBuffs();//чистим
	// <раздел операторов>::=<составной оператор>
	set<string> skipSet = { "end" };
	try { compoundOper(skipSet); }
	catch (PascalExcp& e) {
		skip(skipSet);
	}
}
// обработка <сложный оператор>  begin if  while
void CCompiler::diffOper(set<string> skippingSet)throw(PascalExcp, EOFExcp) {
	// <сложный оператор>::=<составной оператор>|<выбирающий оператор> (в нашем случае сразу условный) | <оператор цикла> |<оператор присоединения>
	ifNullThrowExcp();
	if (curToken->getType() == OPER && (checkOper("if") || checkOper("while"))) {
		if (checkOper("if")) {//обработка if 
			try { ifOper(skippingSet); } 
			catch (PascalExcp& e) { skip(skippingSet); }
		}
		else
			if (checkOper("while")) { //обработка while
				try { whileOper(skippingSet); }
				catch (PascalExcp& e) { skip(skippingSet); }
			}
	}
	else {
		try {
			if (skippingSet.find("end") == skippingSet.end()) skippingSet.insert("end");//если нет в мнозжестве для пропуска end то вставляем его
			compoundOper(skippingSet);
		}
		catch (PascalExcp& e) {
			skip(skippingSet);
			cout << "compound excp" << endl;
		}

	}
}
//обработка if  <условный оператор>  (выбирающий оператор)
void CCompiler::ifOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <условный оператор>::= if <выражение> then <оператор>| if <выражение> then <оператор> else <оператор>
	ifNullThrowExcp();
	set<string> skipIfSet(skippingSet);	// создаем новый список для if для пропусков
	skipIfSet.insert("then");//добавляем в список для пропуска
	accept("if");//ожидаем if 
	try { expression(skipIfSet); }//проверка выражения
	catch (PascalExcp& e) {
		skip(skipIfSet);
	}
	accept("then"); //ожидаем then
	set<string> skipThenSet(skippingSet);
	skipThenSet.insert("else");
	try { oper(skipThenSet); }//анализ опреатора 
	catch (PascalExcp& e) {
		skip(skipThenSet);
	}

	if (checkOper("else")) { //ожидаем else
		accept("else");
		try { oper(skippingSet); } // 
		catch (PascalExcp& e) {
			skip(skippingSet);
		}
	}
}
void CCompiler::whileOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	//<цикл с предусловием>::= while <выражение> do <оператор>
	ifNullThrowExcp();
	set<string> skipIfSet(skippingSet);	// copy set
	skipIfSet.insert("do");
	accept("while");

	try { expression(skipIfSet); }
	catch (PascalExcp& e) {
		skip(skipIfSet);
	}
	accept("do");


	try { oper(skippingSet); }
	catch (PascalExcp& e) {
		skip(skippingSet);
	}

}

//бегин end оператор  
void CCompiler::compoundOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <составной оператор>::= begin <оператор>{;<оператор>} end
	ifNullThrowExcp();
	accept("begin");// проверка на бегин

	if (curToken != nullptr) {
		if (curToken->getType() == OPER	// проверяем, есть ли операторы или сразу end
			&& ((COperToken*)curToken.get())->getLexem() == "end")
		{
			getNext();// если энд то делаем возврат ип ерходим кс ледующему токену
			return;
		}
	}
	skippingSet.insert(";");
	try { oper(skippingSet); }
	catch (PascalExcp& e) {
		// writeMistake(6);
		skip(skippingSet);
	}

	while (!checkOper("end")) {
		try { accept(";"); }
		catch (PascalExcp& e) {}
		if (checkOper("else")) {
			writeMistake(6);
			getNext();
		}
		try { oper(skippingSet); }
		catch (PascalExcp& e) {
			// writeMistake(6);
			skip(skippingSet);
		}
	}

	accept("end");
}

//обработка оператора в нашем случае рассматриваем только непомеченный
void CCompiler::oper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <оператор>::=<непомеченный оператор>|<метка><непомеченный оператор>
	unmarkedOper(skippingSet);
}

//обработка непомеченного оператора 
void CCompiler::unmarkedOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <непомеченный оператор>:: = <простой оператор> |<сложный оператор>
	ifNullThrowExcp();//проверка на конец файла
	// если ;, то в простом операторе есть <пусто>
	if (curToken->getType() == OPER && (checkOper("begin") || checkOper("if") || checkOper("while"))) { // обработка операторов if begin while  это все <сложный оператор>
		{
			try { diffOper(skippingSet); }
			catch (PascalExcp& e) {
				skippingSet.insert("end");
				skip(skippingSet);
			}
		}
	}
	else
		if (curToken->getType() != OPER || ((COperToken*)curToken.get())->getLexem() == ";" ||//обработка остальных о
			((COperToken*)curToken.get())->getLexem() == "end")
		{
			try { simpleOper(skippingSet); } //обработка всех остальных
			catch (PascalExcp& e) { skip(skippingSet); }
		}	// встречен неожиданный оператор
		else {
			writeMistake(6);
			throw PascalExcp();
		}
}

void CCompiler::simpleOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <простой оператор>::=<оператор присваивания>|<оператор процедуры> | <оператор перехода> |<пустой оператор>
	ifNullThrowExcp();
	// <пусто>
	if (curToken->getType() == OPER && (checkOper(";") || checkOper("end")))
		return;
	assignOper(skippingSet);
}
//проверка оператора присваивания
void CCompiler::assignOper(set<string> skippingSet)throw(PascalExcp, EOFExcp) {

	// <оператор присваивания>:: = <переменная>: = <выражение> |<имя функции> : = <выражение>
	ifNullThrowExcp();
	auto varName = var(set<string>());
	ifNullThrowExcp();
	accept(":=");
	auto rightType = expression(skippingSet);
	auto leftType = semantic->getLast()->checkAssignTypes(varName, rightType);
}

// проверка выражения
EType CCompiler::expression(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <выражение>::=<простое выражение>|<простое выражение><операция отношения><простое выражение>
	EType leftType = EType();//для типа левого слагаемого в выражении
	EType rightType = EType();//для типа правого слагаемого в выражении
	try {
		leftType = simpleExpr();// проверка простого выражения
	}
	catch (PascalExcp& e) {
		skip(skippingSet);
	}
	if (isBoolOper()) {
		string oper = ((COperToken*)curToken.get())->getLexem();
		getNext();		// accept
		try {
			rightType = simpleExpr();
			if (eTypeIsDefine(leftType) && eTypeIsDefine(rightType)) leftType = (*semantic).unionBoolExprTypes(leftType, rightType);
		}
		catch (PascalExcp& e) {
			skip(skippingSet);
			leftType = eNONE;
		}

	}
	return leftType;
}

//проверка простого выражения
EType CCompiler::simpleExpr() throw(PascalExcp, EOFExcp) {
	//<простое выражение>:: = <знак><слагаемое>{ <аддитивная операция><слагаемое> } + - or
	ifNullThrowExcp();
	auto sign = acceptSign();//проверка знака  если он есть его пропустим если нет остаёмся 
	EType leftType = term();//проверка слагаемого

	//{ <аддитивная операция><слагаемое> } + - or
	while (isAdditiveOper()) {
		string oper = ((COperToken*)curToken.get())->getLexem();
		getNext();
		EType rightType = term();
		if (eTypeIsDefine(leftType) && eTypeIsDefine(rightType)) leftType = (*semantic).unionTypes(leftType, rightType, oper);

	}
	return leftType;
}

//проверка слагаемого
EType CCompiler::term() throw(PascalExcp, EOFExcp) {
	//<слагаемое>::=<множитель>{<мультипликативная операция><множитель>}
	ifNullThrowExcp();//проверка конца файла
	EType leftType = factor();//проверка множителя возвращается тип 
	while (curToken != nullptr && isMultOper()) {// пока идут мультипликативные операции продолжаем
		string oper = ((COperToken*)curToken.get())->getLexem();
		getNext();
		EType rightType = factor();//проверка множителя возвращается тип 
		if (eTypeIsDefine(leftType) && eTypeIsDefine(rightType)) 
			leftType = (*semantic).unionTypes(leftType, rightType, oper);
	}
	return leftType;
}

//проверка множителя возвращаем тип для проверки
EType CCompiler::factor() throw(PascalExcp, EOFExcp) {
	//<множитель>::=<переменная>|<константа без знака>| (<выражение>) | <обозначение функции> | <множество> | not <множитель>
	ifNullThrowExcp();
	if (checkOper("not")) {// not <множитель>
		getNext();
		auto type = factor();//проверка на множитель     ожидается уже переменная
		if (type != eBOOLEAN) {//проверка типа 
			writeMistake(1004);
			return eNONE;
		}
		return type;
	}
	
	if (curToken->getType() == VALUE) {	// константа без знака (просто число)
		auto curTokenType = ((CValueToken*)curToken.get())->getVariant().getType();
		auto eType = semantic->getLast()->defineType(curTokenType, "");
		getNext();
		return eType;
	}

	if (checkOper("(")) {// (<выражение>)
		accept("(");
		auto factorType = expression(set<string>{"and", "or", "then", "else", ";" });//обработка выражения  (задано множетсво для пропуска)
		accept(")");
		return factorType;
	}
	else {// <переменная>
		string varName = var(set<string>());
		auto varType = semantic->getLast()->defineType(EVarType(), varName);				
		return varType;
	}
}

//проверка запрещенного символа
void CCompiler::checkForbiddenSymbol() throw(PascalExcp, EOFExcp) {
	if (curToken != nullptr && curToken->getType() == UNDEF) {
		writeMistake(6);
		throw PascalExcp();
	}
}

//функция добавления ошибки в список ошибок
void CCompiler::writeMistake(int code) {
	erManager->addError(lexic->getStartPosition(), lexic->getCurLine(), code);
}

void CCompiler::writeMistake(int code, int pos, int oldLineNum) {
	erManager->addError(pos, oldLineNum, code);
}

//проверка ожидаеммого лексера с вызовом ошибки если это не так
void CCompiler::accept(string oper) throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();

	oper = toLower(oper);
	if (curToken->getType() != OPER || ((COperToken*)curToken.get())->getLexem() != oper) {
		if (oper == "program") writeMistake(3);//ожидание лексеров 
		else if (oper == ")") writeMistake(4);
		else if (oper == ":") writeMistake(5);
		else if (oper == "OF") writeMistake(8);
		else if (oper == "(") writeMistake(9);
		else if (oper == "[") writeMistake(11);
		else if (oper == "]") writeMistake(12);
		else if (oper == "end") writeMistake(13);
		else if (oper == ";") writeMistake(14);
		else if (oper == "=") writeMistake(16);
		else if (oper == "begin") writeMistake(17);
		else if (oper == ",") writeMistake(20);
		else if (oper == ":=") writeMistake(51);
		else if (oper == "then") writeMistake(52);
		else if (oper == "until") writeMistake(53);
		else if (oper == "do") writeMistake(54);
		else if (oper == "to" || oper == "downto") writeMistake(55);
		else if (oper == "if") writeMistake(56);
		else if (oper == ".") writeMistake(61);
		else if (oper == "..") writeMistake(74);
		throw PascalExcp();
		return;
	}
	if (((COperToken*)curToken.get())->getLexem() != oper) {
		throw PascalExcp();
		return;
	}
	getNext();
}

//проверка соответствия лексемы оператору
bool CCompiler::checkOper(string oper) {
	if (curToken == nullptr) return false;//проверка на пустоту
	if (curToken->getType() == OPER && //проверка является ли оператором  и 
		((COperToken*)curToken.get())->getLexem() == oper)
		return true;
	return false;
}

//проверка на конец файла
bool CCompiler::ifNullThrowExcp() throw(PascalExcp, EOFExcp) {
	if (curToken == nullptr) {
		erManager->addError(lexic->getCurPosInLine(), lexic->getCurLine(), 1000);//конец файла встретили неожиданно
		throw EOFExcp();
	}
	return false;
}

//проверка является ли токен одним из бинарных операторов
bool CCompiler::isBoolOper() {
	// <операция отношения>::==|<>|<|<=|>=|>|in
	if (curToken == nullptr) return false;
	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken.get())->getLexem() == "=" ||
		((COperToken*)curToken.get())->getLexem() == "<>" ||
		((COperToken*)curToken.get())->getLexem() == "<" ||
		((COperToken*)curToken.get())->getLexem() == "<=" ||
		((COperToken*)curToken.get())->getLexem() == ">=" ||
		((COperToken*)curToken.get())->getLexem() == ">" ||
		((COperToken*)curToken.get())->getLexem() == "in";
}

//проверка на аддетивную операцию +|-|or
bool CCompiler::isAdditiveOper() {
	if (curToken == nullptr) return false;
	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken.get())->getLexem() == "+" ||
		((COperToken*)curToken.get())->getLexem() == "or" ||
		((COperToken*)curToken.get())->getLexem() == "-";
}

//проверка является ли мульипликативной операцией *|/|div|mod|and
bool CCompiler::isMultOper() {
	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken.get())->getLexem() == "*" ||
		((COperToken*)curToken.get())->getLexem() == "/" ||
		((COperToken*)curToken.get())->getLexem() == "div" ||
		((COperToken*)curToken.get())->getLexem() == "and" ||
		((COperToken*)curToken.get())->getLexem() == "mod";
}

// определение знака 
string CCompiler::acceptSign() throw (PascalExcp, EOFExcp) {
	ifNullThrowExcp();

	if (curToken->getType() != OPER) return "";
	string sign = ((COperToken*)curToken.get())->getLexem();
	if (sign == "+" || sign == "-")// если это оказывается знак, то переходим к следуюей лексеме иначе остаемся на ней
		getNext();
	return sign;
}

bool CCompiler::eTypeIsDefine(EType type) {
	return !(type <= INT_MIN || type >= INT_MAX);
}