#pragma once
#include "Lexic.h"
#include "Semantic.h"

class CCompiler {
private:
	CErrorManager* erManager;//�������� ������
	CLexic* lexic;// ������ ������ ������������ �����������
	CSemantic* semantic;// ������ ������ �������������� �����������
	unique_ptr<CToken> curToken;//�������� �����

	void getNext();		// �������� ��������� �����
	void removeToken();	// �������� ������
	void skip(set<string> lexem);	// ������� �� ������������ �������. ������� curToken ����� ����� �� ���������� ��� nullptr
	bool skipUntilBlock(set<string> lexemes, string word);// ������� �� ������ ������-���� �����. ���������� ������, ���� ��� ����������� �������

	void accept(string oper);	// ������� ��������
	string acceptSign();			// ���������� ���� ��� ������ ������, ��������� ����

	void program(); // ��������� ����� ���������
	string name();// ��������� ���� ������� ����� - ������������� �� ��������� ��� ���
	string var(set<string> skippingSet);// ���� ������ ���������� ���, ������ ������
	string fullVar();//��������� ������ ����������

	void block(); //��������� ��������� ����� ����� program
	void blockTypes();//��������� ������� ���������� �����
	void blockVars();//��������� ������� ���������� ����������
	void blockOpers();//��������� ������� ����������

	void descrMonotypeVars();// ������� �� ���� �����

	string type();//��������� ���� (�������� ����) 
	void typeDef();//��������� ����������� ���� 

	void diffOper(set<string> skippingSet);// ������� ��������
	void compoundOper(set<string> skippingSet);// ��������� ��������
	void oper(set<string> skippingSet);// ��������
	void unmarkedOper(set<string> skippingSet);
	void ifOper(set<string> skippingSet);//��������� if
	void whileOper(set<string> skippingSet);//��������� while
	void simpleOper(set<string> skippingSet);
	void assignOper(set<string> skippingSet);// �������� ������������


	EType expression(set<string> skippingSet);//��������� ���������
	EType simpleExpr();
	EType term();//��������� ����������
	EType factor();//��������� ���������

	bool isBoolOper();// �������� �� ��������� ���������
	bool isAdditiveOper();//�������� �� �����. ���������
	bool isMultOper();//�������� �� ��������. ���������
	bool checkOper(string oper);// ��������� �� ��������

	bool ifNullThrowExcp();
	void checkForbiddenSymbol();//�������� ������������ �������
	void writeMistake(int code);
	void writeMistake(int code, int pos, int line);

	bool eTypeIsDefine(EType type);
public:
	CCompiler(CErrorManager* erManager, CLexic* lexic, CSemantic* semantic);//�����������
	~CCompiler();//����������
	void startVer();//������ �����������
};

/// <summary>
/// ������� - ������
/// </summary>
struct PascalExcp : public std::exception {
	const char* what() const throw() {
		return "Some exception in Pascal code";
	}
};

/// <summary>
/// �������-������ ����� �����
/// </summary>
struct EOFExcp : public std::exception {
	const char* what() const throw() {
		return "Reached eof";
	}
};