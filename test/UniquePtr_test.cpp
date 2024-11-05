#include "UniquePtr.hpp"
#include <cstdio>
#include <gtest/gtest.h>
#include <vector>

struct MyClass {
  int a, b, c;
};

struct Animal {
  virtual void speak() = 0;
  virtual ~Animal() = default;
};

struct Dog : Animal {
  int age;

  Dog(int age_) : age(age_) {}

  virtual void speak() { printf("Bark! I'm %d Year Old!\n", age); }
};

struct Cat : Animal {
  int &age;

  Cat(int &age_) : age(age_) {}

  virtual void speak() { printf("Meow! I'm %d Year Old!\n", age); }
};

TEST(MyTest, Test1) { EXPECT_EQ(1, 1); }

TEST(MyTest, Test2) { EXPECT_EQ(2, 2); }

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  std::vector<UniquePtr<Animal>> zoo;
  int age = 3;
  zoo.push_back(makeUnique<Cat>(age));
  zoo.push_back(makeUnique<Dog>(age));
  for (auto const &a : zoo) {
    a->speak();
  }
  age++;
  for (auto const &a : zoo) {
    a->speak();
  }
  return RUN_ALL_TESTS();
}
