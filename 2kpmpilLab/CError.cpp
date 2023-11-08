#include "CError.h"
#include <iostream>

CError::CError(int pos, int line, int code, string description)
{
	this->pos = pos;
	this->code = code;
	this->description = description;
	this->line = line;
}

CError::~CError() {}

