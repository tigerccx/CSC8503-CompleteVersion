#pragma once
#include "json/json.h"

class IImportable {
public:
	virtual void ImportFromJson(Json::Value json) = 0;
};