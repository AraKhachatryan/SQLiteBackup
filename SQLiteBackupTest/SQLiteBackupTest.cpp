#include "gtest/gtest.h"
#include "TestSQLite.h"
#include "TestFTPClient.h"

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}