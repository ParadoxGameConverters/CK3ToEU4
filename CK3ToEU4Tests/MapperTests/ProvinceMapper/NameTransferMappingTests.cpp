#include "../CK3toEU4/Source/CK3World/Titles/LandedTitles.h"
#include "../CK3toEU4/Source/CK3World/Titles/Title.h"
#include "../CK3toEU4/Source/CK3World/Titles/Titles.h"
#include "../CK3toEU4/Source/Mappers/ProvinceMapper/ProvinceMapper.h"
#include "gtest/gtest.h"
#include <sstream>

// Things to test
// 1:1
// N:1
// 1:M
// N:M
// non-latin base not carried forward
// duchy capital priority observed
// duchy capital priority not observed when duchy owner split
// duchy capital priority not observed when in separate mapping