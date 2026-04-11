#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>

#include "hashmap.h"

using namespace std;
using namespace testing;

namespace {

class Random {
 private:
  static mt19937 rng;

 public:
  static void seed(int s) {
    Random::rng.seed(s);
  }

  /**
   * Generate a random integer in the range 0 (inclusive) to `max` (exclusive)
   */
  static int randInt(int max) {
    // Even though mt19937 is standardized, STL distributions aren't!
    // So, unfortunately, even though this is biased, we're forced to
    // do something like this. Technically uint32_fast_t isn't...
    // totally consistent across platforms? But within reason it works.
    return rng() % (max + 1);
  }
};

std::mt19937 Random::rng;

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "hashmap.h"

using namespace std;

TEST(HashMapCore, DefCons) {
  HashMap<int, int> hm;
  EXPECT_EQ(hm.size(), 0);
  EXPECT_TRUE(hm.empty());
  EXPECT_EQ(hm.get_capacity(), 10);
}

TEST(HashMapCore, InsertnSz) {
  HashMap<int, int> hm;
  hm.insert(1, 100);
  EXPECT_EQ(hm.size(), 1);
  EXPECT_FALSE(hm.empty());
}

TEST(HashMapCore, InsertDupesNoOver) {
  HashMap<int, int> hm;
  hm.insert(1, 100);
  hm.insert(1, 999);
  EXPECT_EQ(hm.size(), 1);
  EXPECT_EQ(hm.at(1), 100);
}

TEST(HashMapCore, InsertColl) {
  HashMap<int, int> hm(10);
  hm.insert(0, 1);
  hm.insert(10, 2);
  EXPECT_EQ(hm.size(), 2);
  EXPECT_EQ(hm.at(0), 1);
  EXPECT_EQ(hm.at(10), 2);
}

TEST(HashMapCore, Contains) {
  HashMap<int, int> hm;
  hm.insert(5, 50);
  EXPECT_TRUE(hm.contains(5));
  EXPECT_FALSE(hm.contains(99));
}

TEST(HashMapCore, AtThrowWhenMiss) {
  HashMap<int, int> hm;
  EXPECT_THROW(hm.at(42), out_of_range);
}

TEST(HashMapCore, Clear) {
  HashMap<int, int> hm;
  hm.insert(1, 1);
  hm.insert(2, 2);
  hm.clear();
  EXPECT_EQ(hm.size(), 0);
  EXPECT_TRUE(hm.empty());
  EXPECT_FALSE(hm.contains(1));
}

TEST(HashMapCore, Erase) {
  HashMap<int, int> hm;
  hm.insert(1, 100);
  EXPECT_EQ(hm.erase(1), 100);
  EXPECT_EQ(hm.size(), 0);
  EXPECT_FALSE(hm.contains(1));
}

TEST(HashMapCore, EraseColl) {
  HashMap<int, int> hm(10);
  hm.insert(0, 1);
  hm.insert(10, 2);
  hm.erase(0);
  EXPECT_FALSE(hm.contains(0));
  EXPECT_TRUE(hm.contains(10));
  EXPECT_EQ(hm.at(10), 2);
}

TEST(HashMapCore, EraseThrowsMiss) {
  HashMap<int, int> hm;
  EXPECT_THROW(hm.erase(99), out_of_range);
}

TEST(HashMapCore, CopyConstEmpty) {
  HashMap<int, int> hm;
  HashMap<int, int> copy(hm);
  EXPECT_EQ(copy.size(), 0);
  EXPECT_TRUE(copy.empty());
}

TEST(HashMapCore, CopyConst) {
  HashMap<int, int> hm;
  hm.insert(1, 10);
  hm.insert(2, 20);
  HashMap<int, int> copy(hm);
  EXPECT_EQ(copy.size(), 2);
  EXPECT_EQ(copy.at(1), 10);
  EXPECT_EQ(copy.at(2), 20);

  copy.at(1) = 999;
  EXPECT_EQ(hm.at(1), 10);
}

TEST(HashMapCore, AssignmentOpEmp) {
  HashMap<int, int> hm;
  HashMap<int, int> other;
  other.insert(1, 1);
  other = hm;
  EXPECT_EQ(other.size(), 0);
  EXPECT_TRUE(other.empty());
}

TEST(HashMapCore, AssignmentOp) {
  HashMap<int, int> hm;
  hm.insert(1, 10);
  hm.insert(2, 20);
  HashMap<int, int> other;
  other = hm;
  EXPECT_EQ(other.size(), 2);
  EXPECT_EQ(other.at(1), 10);
  EXPECT_EQ(other.at(2), 20);
}

TEST(HashMapCore, SelfAssignment) {
  HashMap<int, int> hm;
  hm.insert(1, 10);
  HashMap<int, int>& ref = hm;
  hm = ref;
  EXPECT_EQ(hm.size(), 1);
  EXPECT_EQ(hm.at(1), 10);
}

TEST(HashMapCore, AssignmentSz) {
  HashMap<int, int> hm;
  hm.insert(1, 10);
  hm.insert(2, 20);
  HashMap<int, int> other;
  other = hm;
  EXPECT_EQ(other.size(), hm.size());
}

TEST(HashMapAugmented, EqualEmpty) {
  HashMap<int, int> a, b;
  EXPECT_TRUE(a == b);
}

TEST(HashMapAugmented, NotEqDiffVals) {
  HashMap<int, int> a, b;
  a.insert(1, 10);
  b.insert(1, 99);
  EXPECT_FALSE(a == b);
}

TEST(HashMapAugmented, NotEqDiffSz) {
  HashMap<int, int> a, b;
  a.insert(1, 10);
  b.insert(1, 10);
  b.insert(2, 20);
  EXPECT_FALSE(a == b);
}

TEST(HashMapAugmented, BeginNxtEmpty) {
  HashMap<int, int> hm;
  hm.begin();
  int k, v;
  EXPECT_FALSE(hm.next(k, v));
}

TEST(HashMapAugmented, BeginNxtAllElems) {
  HashMap<int, int> hm;
  hm.insert(1, 10);
  hm.insert(2, 20);
  hm.insert(3, 30);

  hm.begin();
  int k, v;
  vector<pair<int, int>> result;
  while (hm.next(k, v)) {
    result.push_back({k, v});
  }

  vector<pair<int, int>> expected = {{1, 10}, {2, 20}, {3, 30}};
  EXPECT_THAT(result, UnorderedElementsAreArray(expected));
}

TEST(HashMapAugmented, BeginNxtColl) {
  HashMap<int, int> hm(10);
  hm.insert(0, 1);
  hm.insert(10, 2);
  hm.insert(5, 3);

  hm.begin();
  int k, v;
  vector<pair<int, int>> result;
  while (hm.next(k, v)) {
    result.push_back({k, v});
  }

  vector<pair<int, int>> expected = {{0, 1}, {10, 2}, {5, 3}};
  EXPECT_THAT(result, UnorderedElementsAreArray(expected));
}

TEST(HashMapAugmented, BeginNxtReturnsFalse) {
  HashMap<int, int> hm;
  hm.insert(1, 10);
  hm.begin();
  int k, v;
  hm.next(k, v);
  EXPECT_FALSE(hm.next(k, v));
}

}  // namespace
