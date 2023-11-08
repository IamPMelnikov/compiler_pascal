#include "CCompiler.h"
#include <iomanip>
#include "ToLowerStr.h"


//������������� 
CCompiler::CCompiler(CErrorManager* erManager, CLexic* lexic, CSemantic* semantic) {
	this->erManager = erManager;
	this->lexic = lexic;
	this->semantic = semantic;
}

CCompiler::~CCompiler() {}

//������ �����������
void CCompiler::startVer() {
	try { getNext(); } // ������ ����� �����    (������ ������������ �����������)
	catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl; 
		return;
	}
	catch (EOFExcp& e) {
		cout << "[!]EOF EXCEPTION" << endl;
		return;
	}


	try { ifNullThrowExcp(); }// �������� �� ��������� ����� �����
	catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl;
		return;
	}
	catch (EOFExcp& e) {
		cout << "[!]EOF EXCEPTION" << endl;
		return;
	}

	try { program(); }//�������� �����
	catch (PascalExcp& e) {
		cout << "[!]EXCEPTION" << endl;
		return;
	}
	catch (EOFExcp& e) {
		cout << "[!]EOF EXCEPTION" << endl;
		return;
	}
}

//������� ��� ������ ����������� ��������� ������� ������ � �������� ������
void CCompiler::getNext() throw(PascalExcp, EOFExcp) {
	removeToken();
	curToken = this->lexic->getNext(true);//�������� ��������� ��������������� �����
}


//������� �������� �������������� ������
void CCompiler::skip(set<string> lexemes) {
	if (curToken == nullptr) return; // 
	// ���� ��� �� ������ �����
	if (curToken->getType() == OPER //���� �������� � ���� � �������� ��������� lexemes
		&& lexemes.find(((COperToken*)curToken.get())->getLexem()) != lexemes.end()) {
		return;
	}
	removeToken();// ���� �� �� lexemes �� ��������� � ���������� ������
	curToken = this->lexic->skip(lexemes);
}

//��������� ��������� ������ 
void CCompiler::removeToken() {
	if (curToken != nullptr) //���� ��  nullptr ������ nullptr
		curToken.release();
}

void CCompiler::program() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();//�������� �� ����� ����� 
	set<string> nameSet = { "(", ",", ";", "var", "begin" };//������ �� ���� �������
	set <string> branchSet = { ";", "var", "begin" };
	set <string> semicolSet = { "var", "begin" };
	accept("program");//��������� ��� ����� ���������� � program � �������� ����� �����

	string programName = "";//��� �������� ���������
	try { programName = name(); } //��������� ������� ����� (�������������) ��� �������� ���������
	catch (PascalExcp& e) {//���� �� �������������
		skip(nameSet); // �� ������� ������� �� "(", ",", ";", "var", "begin"
	}

	//��������� �������� ������
	try { accept("("); }//������
	catch (PascalExcp& e) {
		skip(nameSet);
	}
	try { name(); }//�������������
	catch (PascalExcp& e) {
		skip(nameSet);
	}
	while (checkOper(",")) {// ���� �������� ��� ��������� �������������� 
		getNext();//��� ������� ","  �� ���������� � 
		try { name(); }
		catch (PascalExcp& e) {
			skip(nameSet);
		}
	}
	try { accept(")"); }//������ ���������
	catch (PascalExcp& e) {
		skip(branchSet);//������� ��� ����� ������ ����
	}
	try { accept(";"); }//����� � ���
	catch (PascalExcp& e) {
		skip(branchSet);//������� ����� ; ����
	}

	block();// � ����� ������� ������� ��� ������

	accept(".");//�������������� ����� ����� end
	if (curToken != nullptr) { //���� ���-�� ��� ���� �� �� ����� �� ������� �������������� ������
		writeMistake(1001);
		throw PascalExcp();
	}
}

// ������� ������������� ���� �� �� �� ������ ���� �� �� ��������� ��� ��� � ����� ��������� �����
string CCompiler::name() throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp(); //�������� ����� �����
	if (curToken->getType() != IDENT) { //�������� ������ �� ��� )������� �������������)
		writeMistake(2);//���� �� ������������� �� ������ �����
		throw PascalExcp();
	}
	string nameStr = ((CIdentToken*)curToken.get())->getLexem();
	getNext(); //��������� �� ��������� �����
	return nameStr;//���������� ��� �������������� 
}

//
bool CCompiler::skipUntilBlock(set<string> searchingLexemes, string searchingWord) throw(PascalExcp, EOFExcp) {
	// ��������� ��������� �� �� �������  
	// ���� ���, �������� �������� ��  ��� �� ������ ��������� ������
	ifNullThrowExcp();//�������� �� ����� ����� 
	int oldPosInLine = lexic->getStartPosition(); 
	int oldLineNum = lexic->getCurLine();

	if (curToken->getType() != OPER) { //���� �� ��������
		skip(searchingLexemes); //����������� �� ��������� � �������� ��������� searchingLexemes  (�� ������ ������ �� ��������� ��������)
		writeMistake(6, oldPosInLine, oldLineNum);//�����������/����������� ������
		return true;
	}
	else {//���� ��������
		if (((COperToken*)curToken.get())->getLexem() != searchingWord) {//��������� ��������� �� � ������� ������ ���� �� �� ��� ������ � �� ���� ������, ���� ��� �� �� ���������� ��  ��������� �� searchingLexemes
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

//��������� ����������
string CCompiler::var(set<string> skippingSet)throw(PascalExcp, EOFExcp) {// � ��� ������ ������
	//<����������>::=<������ ����������>|<���������� ����������> | <��������� ����������>
	ifNullThrowExcp();
	return fullVar();
}

//��������� ������ ���������� (��� ���������� ��������) //
string CCompiler::fullVar() throw(PascalExcp, EOFExcp) {
	//<������ ����������>::=<��� ����������>
	return name();
}

//������ �� �������� ���� ��������� (����� program  � end.)
void CCompiler::block() throw(PascalExcp, EOFExcp) {  
	ifNullThrowExcp();//�������� �� ����� �����
	
	set <string> blockTypeSet = { "type", "var", "begin" };//������� ��������� ��������
	set <string> blockVarsSet = { "var","begin" };
	set <string> blockOpersSet = { "begin" };
	
	skipUntilBlock(blockTypeSet, "type"); // ���������� �� "type", "var", "begin" 
	blockTypes(); //���������  ������ �����

	ifNullThrowExcp();
	skipUntilBlock(blockVarsSet, "var"); // ���������� ��  "var", "begin"
	blockVars();//��������� ������ �������� ����������

	ifNullThrowExcp();
	skipUntilBlock(blockOpersSet, "begin");// ������� �� "begin"
	blockOpers();//���� ���������
}

//��������� ������� �����
void CCompiler::blockTypes() {
	//<������ �����>::=<�����>|type <����������� ����>;{<����������� ����>; }
	semantic->getLast()->setBlock(TYPEBL);
	semantic->getLast()->clearBuffs();//������ ������ ���� � �����
	ifNullThrowExcp();//�������� �� ����� ����� 
	set <string> skipSet = { ";", "begin", "var" };
	if (checkOper("type")) {//�������� ������� �� ����������� ���������� ���������
		getNext();//����� ��������� �����
		try { typeDef(); }//<����������� ����>
		catch (PascalExcp& e) {
			skip(skipSet);
			semantic->getLast()->createNone();//���������� �������������� ���
		}
		semantic->getLast()->clearBuffs();
		// {<����������� ����>;}
		while (checkOper(";")) {
			getNext();
			if (curToken != nullptr) {
				if (curToken->getType() == OPER && (//���� ���������� �� ������ ���������� �����, �� ������� 
					((COperToken*)curToken.get())->getLexem() == "begin" ||
					((COperToken*)curToken.get())->getLexem() == "var"))
					return;
			}
			else return; // ����������� ����� �����
			try { typeDef(); }//<����������� ����>
			catch (PascalExcp& e) {
				skip(skipSet);
				semantic->getLast()->createNone();
			}
			semantic->getLast()->clearBuffs();
		}
	}
}

//��������� ����������� ����
void CCompiler::typeDef()throw(PascalExcp, EOFExcp) {
	//<����������� ����>::= <���>=<���>
	semantic->getLast()->addToNameBuffer(name());// ��� ������� ��������� � ������� ������ ��������� ����� ��� ��������������
	accept("=");//��������� �� ������� =  � ������� ��������� �����
	auto typeName = type();//�������� ��� ����
	semantic->getLast()->addToBuffer(typeName);//��������� � ����� ��� ������ ����
}
//��� (������ �������)
string CCompiler::type()throw(PascalExcp, EOFExcp) {
	// <���>:: = <������� ���> 
	ifNullThrowExcp();//�������� �� ����� �����
	try {
		return name();	//<��� ����>
	}
	catch (PascalExcp& e) {
		writeMistake(324);// ��� ������ ����
		throw e;
	}
	return "";

}


// ��������� ������� �������� ����������
void CCompiler::blockVars() throw(PascalExcp, EOFExcp) {
	semantic->getLast()->setBlock(VARBL);//�������� ��� ������ ����������
	semantic->getLast()->clearBuffs();//������ ������
	//<������ ����������>::= var <�������� ���������� ����������>;{<�������� ���������� ����������>; } | <�����>
	ifNullThrowExcp();//�������� �� ������ �����
	set <string> skipSet = { ";", "begin" };
	// var <�������� ���������� ����������>;
	if (checkOper("var")) {
		getNext();//��������� � ���������� ������ ���� var ����
		try { descrMonotypeVars(); }
		catch (PascalExcp& e) {
			skip(skipSet);
			semantic->getLast()->createNone();
		}
		semantic->getLast()->clearBuffs();
		// {<�������� ���������� ����������>;}
		while (checkOper(";")) {// ��������� ������ ���������� 
			getNext();
			if (curToken != nullptr) {
				if (curToken->getType() == OPER && (//���� ������ �� ����� �� �������
					((COperToken*)curToken.get())->getLexem() == "begin"))
					return;
			}
			else return;	// ����������� ����� �����
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

//��������� �������� ���������� ����������  
void CCompiler::descrMonotypeVars() throw(PascalExcp, EOFExcp) {
	// <�������� ���������� ����������>::=<���>{,<���>}:<���>
	set <string> constDefSet = { ",", ":", ";", "begin" };
	try {
		semantic->getLast()->addToNameBuffer(name());//���������� ��� �������������� (��������� , ��� ����� ��)
	}
	catch (PascalExcp& e) {
		skip(constDefSet);//������� �� ������ �� ���������� constDefSet
	}
	while (checkOper(",")) {//���� ��������� , �� ������, ��� ���� ����������� ��������������� (���� ����������)
		getNext();
		try {
			semantic->getLast()->addToNameBuffer(name());//���������� � ����� ��� ����������
		}
		catch (PascalExcp& e) {
			skip(constDefSet);//������� �� ������ �� ���������� constDefSet 
		}
	}
	accept(":");//������� :  � ���������� � ���������� ������
	auto typeName = type();//����� ��������� �����(���) 
	semantic->getLast()->addToBuffer(typeName);//���������� ��� � �����
}

// ��������� ������� ����������
void CCompiler::blockOpers()throw(PascalExcp, EOFExcp) {
	semantic->getLast()->setBlock(BODYBL);//�������� ������� ������ 
	semantic->getLast()->clearBuffs();//������
	// <������ ����������>::=<��������� ��������>
	set<string> skipSet = { "end" };
	try { compoundOper(skipSet); }
	catch (PascalExcp& e) {
		skip(skipSet);
	}
}
// ��������� <������� ��������>  begin if  while
void CCompiler::diffOper(set<string> skippingSet)throw(PascalExcp, EOFExcp) {
	// <������� ��������>::=<��������� ��������>|<���������� ��������> (� ����� ������ ����� ��������) | <�������� �����> |<�������� �������������>
	ifNullThrowExcp();
	if (curToken->getType() == OPER && (checkOper("if") || checkOper("while"))) {
		if (checkOper("if")) {//��������� if 
			try { ifOper(skippingSet); } 
			catch (PascalExcp& e) { skip(skippingSet); }
		}
		else
			if (checkOper("while")) { //��������� while
				try { whileOper(skippingSet); }
				catch (PascalExcp& e) { skip(skippingSet); }
			}
	}
	else {
		try {
			if (skippingSet.find("end") == skippingSet.end()) skippingSet.insert("end");//���� ��� � ���������� ��� �������� end �� ��������� ���
			compoundOper(skippingSet);
		}
		catch (PascalExcp& e) {
			skip(skippingSet);
			cout << "compound excp" << endl;
		}

	}
}
//��������� if  <�������� ��������>  (���������� ��������)
void CCompiler::ifOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <�������� ��������>::= if <���������> then <��������>| if <���������> then <��������> else <��������>
	ifNullThrowExcp();
	set<string> skipIfSet(skippingSet);	// ������� ����� ������ ��� if ��� ���������
	skipIfSet.insert("then");//��������� � ������ ��� ��������
	accept("if");//������� if 
	try { expression(skipIfSet); }//�������� ���������
	catch (PascalExcp& e) {
		skip(skipIfSet);
	}
	accept("then"); //������� then
	set<string> skipThenSet(skippingSet);
	skipThenSet.insert("else");
	try { oper(skipThenSet); }//������ ��������� 
	catch (PascalExcp& e) {
		skip(skipThenSet);
	}

	if (checkOper("else")) { //������� else
		accept("else");
		try { oper(skippingSet); } // 
		catch (PascalExcp& e) {
			skip(skippingSet);
		}
	}
}
void CCompiler::whileOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	//<���� � ������������>::= while <���������> do <��������>
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

//����� end ��������  
void CCompiler::compoundOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <��������� ��������>::= begin <��������>{;<��������>} end
	ifNullThrowExcp();
	accept("begin");// �������� �� �����

	if (curToken != nullptr) {
		if (curToken->getType() == OPER	// ���������, ���� �� ��������� ��� ����� end
			&& ((COperToken*)curToken.get())->getLexem() == "end")
		{
			getNext();// ���� ��� �� ������ ������� �� ������� �� ��������� ������
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

//��������� ��������� � ����� ������ ������������� ������ ������������
void CCompiler::oper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <��������>::=<������������ ��������>|<�����><������������ ��������>
	unmarkedOper(skippingSet);
}

//��������� ������������� ��������� 
void CCompiler::unmarkedOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <������������ ��������>:: = <������� ��������> |<������� ��������>
	ifNullThrowExcp();//�������� �� ����� �����
	// ���� ;, �� � ������� ��������� ���� <�����>
	if (curToken->getType() == OPER && (checkOper("begin") || checkOper("if") || checkOper("while"))) { // ��������� ���������� if begin while  ��� ��� <������� ��������>
		{
			try { diffOper(skippingSet); }
			catch (PascalExcp& e) {
				skippingSet.insert("end");
				skip(skippingSet);
			}
		}
	}
	else
		if (curToken->getType() != OPER || ((COperToken*)curToken.get())->getLexem() == ";" ||//��������� ��������� �
			((COperToken*)curToken.get())->getLexem() == "end")
		{
			try { simpleOper(skippingSet); } //��������� ���� ���������
			catch (PascalExcp& e) { skip(skippingSet); }
		}	// �������� ����������� ��������
		else {
			writeMistake(6);
			throw PascalExcp();
		}
}

void CCompiler::simpleOper(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <������� ��������>::=<�������� ������������>|<�������� ���������> | <�������� ��������> |<������ ��������>
	ifNullThrowExcp();
	// <�����>
	if (curToken->getType() == OPER && (checkOper(";") || checkOper("end")))
		return;
	assignOper(skippingSet);
}
//�������� ��������� ������������
void CCompiler::assignOper(set<string> skippingSet)throw(PascalExcp, EOFExcp) {

	// <�������� ������������>:: = <����������>: = <���������> |<��� �������> : = <���������>
	ifNullThrowExcp();
	auto varName = var(set<string>());
	ifNullThrowExcp();
	accept(":=");
	auto rightType = expression(skippingSet);
	auto leftType = semantic->getLast()->checkAssignTypes(varName, rightType);
}

// �������� ���������
EType CCompiler::expression(set<string> skippingSet) throw(PascalExcp, EOFExcp) {
	// <���������>::=<������� ���������>|<������� ���������><�������� ���������><������� ���������>
	EType leftType = EType();//��� ���� ������ ���������� � ���������
	EType rightType = EType();//��� ���� ������� ���������� � ���������
	try {
		leftType = simpleExpr();// �������� �������� ���������
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

//�������� �������� ���������
EType CCompiler::simpleExpr() throw(PascalExcp, EOFExcp) {
	//<������� ���������>:: = <����><���������>{ <���������� ��������><���������> } + - or
	ifNullThrowExcp();
	auto sign = acceptSign();//�������� �����  ���� �� ���� ��� ��������� ���� ��� ������� 
	EType leftType = term();//�������� ����������

	//{ <���������� ��������><���������> } + - or
	while (isAdditiveOper()) {
		string oper = ((COperToken*)curToken.get())->getLexem();
		getNext();
		EType rightType = term();
		if (eTypeIsDefine(leftType) && eTypeIsDefine(rightType)) leftType = (*semantic).unionTypes(leftType, rightType, oper);

	}
	return leftType;
}

//�������� ����������
EType CCompiler::term() throw(PascalExcp, EOFExcp) {
	//<���������>::=<���������>{<����������������� ��������><���������>}
	ifNullThrowExcp();//�������� ����� �����
	EType leftType = factor();//�������� ��������� ������������ ��� 
	while (curToken != nullptr && isMultOper()) {// ���� ���� ����������������� �������� ����������
		string oper = ((COperToken*)curToken.get())->getLexem();
		getNext();
		EType rightType = factor();//�������� ��������� ������������ ��� 
		if (eTypeIsDefine(leftType) && eTypeIsDefine(rightType)) 
			leftType = (*semantic).unionTypes(leftType, rightType, oper);
	}
	return leftType;
}

//�������� ��������� ���������� ��� ��� ��������
EType CCompiler::factor() throw(PascalExcp, EOFExcp) {
	//<���������>::=<����������>|<��������� ��� �����>| (<���������>) | <����������� �������> | <���������> | not <���������>
	ifNullThrowExcp();
	if (checkOper("not")) {// not <���������>
		getNext();
		auto type = factor();//�������� �� ���������     ��������� ��� ����������
		if (type != eBOOLEAN) {//�������� ���� 
			writeMistake(1004);
			return eNONE;
		}
		return type;
	}
	
	if (curToken->getType() == VALUE) {	// ��������� ��� ����� (������ �����)
		auto curTokenType = ((CValueToken*)curToken.get())->getVariant().getType();
		auto eType = semantic->getLast()->defineType(curTokenType, "");
		getNext();
		return eType;
	}

	if (checkOper("(")) {// (<���������>)
		accept("(");
		auto factorType = expression(set<string>{"and", "or", "then", "else", ";" });//��������� ���������  (������ ��������� ��� ��������)
		accept(")");
		return factorType;
	}
	else {// <����������>
		string varName = var(set<string>());
		auto varType = semantic->getLast()->defineType(EVarType(), varName);				
		return varType;
	}
}

//�������� ������������ �������
void CCompiler::checkForbiddenSymbol() throw(PascalExcp, EOFExcp) {
	if (curToken != nullptr && curToken->getType() == UNDEF) {
		writeMistake(6);
		throw PascalExcp();
	}
}

//������� ���������� ������ � ������ ������
void CCompiler::writeMistake(int code) {
	erManager->addError(lexic->getStartPosition(), lexic->getCurLine(), code);
}

void CCompiler::writeMistake(int code, int pos, int oldLineNum) {
	erManager->addError(pos, oldLineNum, code);
}

//�������� ����������� ������� � ������� ������ ���� ��� �� ���
void CCompiler::accept(string oper) throw(PascalExcp, EOFExcp) {
	ifNullThrowExcp();

	oper = toLower(oper);
	if (curToken->getType() != OPER || ((COperToken*)curToken.get())->getLexem() != oper) {
		if (oper == "program") writeMistake(3);//�������� �������� 
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

//�������� ������������ ������� ���������
bool CCompiler::checkOper(string oper) {
	if (curToken == nullptr) return false;//�������� �� �������
	if (curToken->getType() == OPER && //�������� �������� �� ����������  � 
		((COperToken*)curToken.get())->getLexem() == oper)
		return true;
	return false;
}

//�������� �� ����� �����
bool CCompiler::ifNullThrowExcp() throw(PascalExcp, EOFExcp) {
	if (curToken == nullptr) {
		erManager->addError(lexic->getCurPosInLine(), lexic->getCurLine(), 1000);//����� ����� ��������� ����������
		throw EOFExcp();
	}
	return false;
}

//�������� �������� �� ����� ����� �� �������� ����������
bool CCompiler::isBoolOper() {
	// <�������� ���������>::==|<>|<|<=|>=|>|in
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

//�������� �� ���������� �������� +|-|or
bool CCompiler::isAdditiveOper() {
	if (curToken == nullptr) return false;
	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken.get())->getLexem() == "+" ||
		((COperToken*)curToken.get())->getLexem() == "or" ||
		((COperToken*)curToken.get())->getLexem() == "-";
}

//�������� �������� �� ���������������� ��������� *|/|div|mod|and
bool CCompiler::isMultOper() {
	if (curToken->getType() != OPER) return false;
	return ((COperToken*)curToken.get())->getLexem() == "*" ||
		((COperToken*)curToken.get())->getLexem() == "/" ||
		((COperToken*)curToken.get())->getLexem() == "div" ||
		((COperToken*)curToken.get())->getLexem() == "and" ||
		((COperToken*)curToken.get())->getLexem() == "mod";
}

// ����������� ����� 
string CCompiler::acceptSign() throw (PascalExcp, EOFExcp) {
	ifNullThrowExcp();

	if (curToken->getType() != OPER) return "";
	string sign = ((COperToken*)curToken.get())->getLexem();
	if (sign == "+" || sign == "-")// ���� ��� ����������� ����, �� ��������� � �������� ������� ����� �������� �� ���
		getNext();
	return sign;
}

bool CCompiler::eTypeIsDefine(EType type) {
	return !(type <= INT_MIN || type >= INT_MAX);
}