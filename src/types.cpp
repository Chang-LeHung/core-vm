

#include "types.h"

// keep the order of the following 2 statements or runtime error will occur
std::unordered_map<std::string, CVMType> CVMType::_type_set;

CVMType void_op = CVMType::GetOrCreateType("void");
