#include "CTokenFactory.h"
#include "ToLowerStr.h"

using namespace std;

//инициализац€ объекта класса дл€ определени€ типов токена по лексеме и создани€ соответствуюих оъектов классов
CTokenFactory::CTokenFactory() {}

//создание токена из лексемы
unique_ptr<CToken> CTokenFactory::createToken(string lexem) {
	string lowerLexem = toLower(lexem);// переводим лексему в заниженный вид
	// €вл€етс€ ли char (тип value)
	if (lexem.length() == 3 && lexem[0] == '\'' && lexem[lexem.length() - 1] == '\'')
		return make_unique<CValueToken>(CCharVariant(lexem[1]));

	// €вл€етс€ ли string (тип value)
	if (lexem[0] == '\'' && lexem[lexem.length() - 1] == '\'')
		return make_unique<CValueToken>(CStringVariant(lexem));

	// €вл€етс€ ли лексема оператором
	if (sOperators.find(lowerLexem) != sOperators.end())//провер€ем наличие лексемы в списке опреаторов
		return make_unique<COperToken>(lowerLexem);

	// можно ли сконвертировать в число  int (тип value)		
	if (regex_match(lexem, regex(R"(\d+)"))) {
		int temp = stoi(lexem);//переводчим в число
		return make_unique<CValueToken>(CIntVariant(temp));
	}

	//проверка на real число
	if (regex_match(lexem, regex(R"(\d+\.\d+)"))) {
		float temp = stof(lexem);
		return make_unique<CValueToken>(CRealVariant(temp));
	}
	// может ли она быть идентификатором       проверка на UNDEF токен 
	const regex pattern(R"(^[a-zA-Z_]+[a-zA-Z_0-9]*$)");
	if (regex_match(lexem, pattern))
		return make_unique<CIdentToken>(lexem);

	return make_unique<CUndefToken>(lexem);//если ничего не сработало, то отмечаем как неопознанный токен
}


CTokenFactory::~CTokenFactory() {}
