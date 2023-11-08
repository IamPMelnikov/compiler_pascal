#include "ToLowerStr.h"
#include <regex>
using namespace std;
//функция для понижения регистра
string toLower(string lexem)
{
	transform(lexem.begin(), lexem.end(), lexem.begin(), ::tolower);
	return lexem;
}
