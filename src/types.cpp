

#include "types.h"

CVMType void_op = CVMType::GetOrCreateType("void");

std::unordered_map<std::string, CVMType> CVMType::_type_set = {};