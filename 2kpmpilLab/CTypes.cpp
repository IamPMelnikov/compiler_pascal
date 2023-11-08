#include "CTypes.h"

//конструктор
CType::CType(EType type) {
	this->type = type;
}

CType::~CType() {}

CNoneType::CNoneType() :CType(eNONE) {}

CNoneType::~CNoneType() {}

CBaseType::CBaseType(EType type) : CType(type) {}

CBaseType::~CBaseType() {}

CEnumType::CEnumType() : CType(eBOOLEAN) {}

CEnumType::~CEnumType() {}

void CEnumType::addEl(CEnumElType* el) {
	els.push_back(el);
}

CEnumElType::CEnumElType() : CType(eBOOLEAN) {}

CEnumElType::CEnumElType(CEnumType* parent) : CType(eBOOLEAN) {
	this->parent = parent;
}

CEnumElType::~CEnumElType() {}

void CEnumElType::setParent(CEnumType* parent) {
	this->parent = parent;
}
