#include "CTokenFactory.h"
#include "ToLowerStr.h"

using namespace std;

//������������ ������� ������ ��� ����������� ����� ������ �� ������� � �������� �������������� ������� �������
CTokenFactory::CTokenFactory() {}

//�������� ������ �� �������
unique_ptr<CToken> CTokenFactory::createToken(string lexem) {
	string lowerLexem = toLower(lexem);// ��������� ������� � ���������� ���
	// �������� �� char (��� value)
	if (lexem.length() == 3 && lexem[0] == '\'' && lexem[lexem.length() - 1] == '\'')
		return make_unique<CValueToken>(CCharVariant(lexem[1]));

	// �������� �� string (��� value)
	if (lexem[0] == '\'' && lexem[lexem.length() - 1] == '\'')
		return make_unique<CValueToken>(CStringVariant(lexem));

	// �������� �� ������� ����������
	if (sOperators.find(lowerLexem) != sOperators.end())//��������� ������� ������� � ������ ����������
		return make_unique<COperToken>(lowerLexem);

	// ����� �� ��������������� � �����  int (��� value)		
	if (regex_match(lexem, regex(R"(\d+)"))) {
		int temp = stoi(lexem);//���������� � �����
		return make_unique<CValueToken>(CIntVariant(temp));
	}

	//�������� �� real �����
	if (regex_match(lexem, regex(R"(\d+\.\d+)"))) {
		float temp = stof(lexem);
		return make_unique<CValueToken>(CRealVariant(temp));
	}
	// ����� �� ��� ���� ���������������       �������� �� UNDEF ����� 
	const regex pattern(R"(^[a-zA-Z_]+[a-zA-Z_0-9]*$)");
	if (regex_match(lexem, pattern))
		return make_unique<CIdentToken>(lexem);

	return make_unique<CUndefToken>(lexem);//���� ������ �� ���������, �� �������� ��� ������������ �����
}


CTokenFactory::~CTokenFactory() {}
