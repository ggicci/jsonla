#include "../jsonla.h"
#include <fstream>
#include <iostream>
#include "gtest/gtest.h"

using namespace std;
using namespace ggicci;

namespace {
class JsonTest : public ::testing::Test {
 protected:
  JsonTest() {}
  ~JsonTest() {}
};
};  // namespace

TEST_F(JsonTest, ParseWrongCases) {
  ifstream ifs("./testcases-wrong.txt");
  string line;
  if (ifs.is_open()) {
    while (getline(ifs, line)) {
      if (line.find("#") == 0) {
        continue;
      }
      try {
        Json js = Json::Parse(line.c_str());
        FAIL() << "Expected parse error";
      } catch (...) {
        ;
      }
    }
    ifs.close();
  }
}

TEST_F(JsonTest, ParseRightCases) {
  ifstream ifs("./testcases-right.txt");
  string line;
  if (ifs.is_open()) {
    while (getline(ifs, line)) {
      if (line.find("#") == 0) {
        continue;
      }
      try {
        Json js = Json::Parse(line.c_str());
      } catch (...) {
        FAIL() << "Expected no exception:" << line;
      }
    }
    ifs.close();
  }
}

TEST_F(JsonTest, GetArraySize) {
  Json arr = Json::Parse("[1,2,3,4]");
  EXPECT_EQ(arr.Size(), 4);
}
