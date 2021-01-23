#include "binary_search_tree.h"

#include <gtest/gtest.h>

#include <algorithm>

class TrickyClass {
 public:
  TrickyClass() = delete;

  auto operator<=>(const TrickyClass& rhs) const {
    return a_ - rhs.a_;
  }

  bool operator==(const TrickyClass& rhs) const {
    return a_ == rhs.a_;
  }

  explicit TrickyClass(int a) : a_(a) {}

  TrickyClass(const TrickyClass& rhs) {
    *this = rhs;
  }

  TrickyClass& operator=(const TrickyClass& rhs) {
    a_ = rhs.a_;
    return *this;
  }

 private:
  int a_ = 0;
};

class TrickyClassTwo {
 public:
  TrickyClassTwo() = delete;

  bool operator<(const TrickyClassTwo& rhs) const {
    return a_ < rhs.a_;
  }

  explicit TrickyClassTwo(int a) : a_(a) {}

  TrickyClassTwo(const TrickyClassTwo& rhs) {
    *this = rhs;
  }

  TrickyClassTwo& operator=(const TrickyClassTwo& rhs) {
    a_ = rhs.a_;
    return *this;
  }

 private:
  int a_ = 0;
};

TEST(BinarySearchTree, ConstructorsBasicsTests) {
  {
    BinarySearchTree<TrickyClass> bst;
    EXPECT_EQ(bst.size(), 0);
    EXPECT_TRUE(bst.empty());
    EXPECT_EQ(bst.to_vector(), std::vector<TrickyClass>({}));
  }
  {
    std::vector<TrickyClass> sorted =
        {TrickyClass(1), TrickyClass(2), TrickyClass(3), TrickyClass(4),
         TrickyClass(5), TrickyClass(6), TrickyClass(7)};
    std::vector<TrickyClass> sorted_2 =
        {TrickyClass(1), TrickyClass(2), TrickyClass(3), TrickyClass(4),
         TrickyClass(5), TrickyClass(6), TrickyClass(8)};
    BinarySearchTree<TrickyClass>
        bst_1({TrickyClass(7), TrickyClass(2), TrickyClass(5), TrickyClass(3),
               TrickyClass(1), TrickyClass(4), TrickyClass(6)});
    EXPECT_EQ(bst_1.size(), 7);
    EXPECT_FALSE(bst_1.empty());
    EXPECT_EQ(bst_1.to_vector(), sorted);

    BinarySearchTree<TrickyClass>
        bst_2({TrickyClass(1), TrickyClass(4), TrickyClass(5), TrickyClass(8),
               TrickyClass(6), TrickyClass(3), TrickyClass(2)});
    EXPECT_EQ(bst_2.size(), 7);
    EXPECT_FALSE(bst_2.empty());
    EXPECT_EQ(bst_2.to_vector(), sorted_2);

    BinarySearchTree<TrickyClass>
        bst_3({TrickyClass(7), TrickyClass(2), TrickyClass(5), TrickyClass(3),
               TrickyClass(1), TrickyClass(4), TrickyClass(6)});
    EXPECT_EQ(bst_3.size(), 7);
    EXPECT_FALSE(bst_3.empty());
    EXPECT_EQ(bst_3.to_vector(), sorted);

    bst_3 = bst_2;
    EXPECT_EQ(bst_3.size(), 7);
    EXPECT_FALSE(bst_3.empty());
    EXPECT_EQ(bst_3.to_vector(), sorted_2);

    bst_3 = std::move(bst_1);
    EXPECT_EQ(bst_3.size(), 7);
    EXPECT_FALSE(bst_3.empty());
    EXPECT_EQ(bst_3.to_vector(), sorted);

    EXPECT_EQ(bst_1.size(), 0);
    EXPECT_TRUE(bst_1.empty());
    EXPECT_EQ(bst_1.to_vector(), std::vector<TrickyClass>{});
  }
  {
    BinarySearchTree<int> bst = {1, 1, 2, 3, 7, 4, 4, 2, 4};
    EXPECT_EQ(bst.size(), 9);
    EXPECT_FALSE(bst.empty());
    EXPECT_EQ(bst.to_vector(), std::vector<int>({1, 1, 2, 2, 3, 4, 4, 4, 7}));

    BinarySearchTree<int> bst_2 = {6, 2, 3, 3, 2, 1};
    EXPECT_EQ(bst_2.size(), 6);
    EXPECT_FALSE(bst_2.empty());
    EXPECT_EQ(bst_2.to_vector(), std::vector<int>({1, 2, 2, 3, 3, 6}));

    bst = bst_2;
    EXPECT_EQ(bst.size(), 6);
    EXPECT_FALSE(bst.empty());
    EXPECT_EQ(bst.to_vector(), std::vector<int>({1, 2, 2, 3, 3, 6}));
  }
  {
    BinarySearchTree<TrickyClassTwo> bst = {TrickyClassTwo(1)};
  }
}

TEST(BinarySearchTree, ContainsCountTests) {
  {
    BinarySearchTree<TrickyClass>
        bst({TrickyClass(1), TrickyClass(8), TrickyClass(5), TrickyClass(4),
             TrickyClass(6), TrickyClass(1), TrickyClass(1), TrickyClass(8)});
    EXPECT_TRUE(bst.contains(TrickyClass(1)));
    EXPECT_FALSE(bst.contains(TrickyClass(2)));
    EXPECT_FALSE(bst.contains(TrickyClass(3)));
    EXPECT_TRUE(bst.contains(TrickyClass(4)));
    EXPECT_TRUE(bst.contains(TrickyClass(5)));
    EXPECT_TRUE(bst.contains(TrickyClass(6)));
    EXPECT_FALSE(bst.contains(TrickyClass(7)));
    EXPECT_TRUE(bst.contains(TrickyClass(8)));
    EXPECT_EQ(bst.count(TrickyClass(1)), 3);
    EXPECT_EQ(bst.count(TrickyClass(4)), 1);
    EXPECT_EQ(bst.count(TrickyClass(5)), 1);
    EXPECT_EQ(bst.count(TrickyClass(6)), 1);
    EXPECT_EQ(bst.count(TrickyClass(8)), 2);
    EXPECT_EQ(bst.count(TrickyClass(2)), 0);
  }
}

TEST(BinarySearchTree, InsertEmplaceTests) {
  {
    std::vector<TrickyClass> sorted = {TrickyClass(4)};
    BinarySearchTree<TrickyClass> bst;
    bst.insert(TrickyClass(4));
    EXPECT_EQ(bst.to_vector(), sorted);

    sorted.emplace_back(2);
    std::sort(sorted.begin(), sorted.end());
    bst.insert(TrickyClass(2));
    EXPECT_EQ(bst.to_vector(), sorted);

    sorted.emplace_back(4);
    std::sort(sorted.begin(), sorted.end());
    bst.emplace(4);
    EXPECT_EQ(bst.to_vector(), sorted);
  }
}

TEST(BinarySearchTree, EraseClearTests) {
  {
    std::vector<TrickyClass> sorted =
        {TrickyClass(1), TrickyClass(1), TrickyClass(3), TrickyClass(4),
         TrickyClass(5), TrickyClass(5), TrickyClass(7)};
    BinarySearchTree<TrickyClass>
        bst({TrickyClass(7), TrickyClass(1), TrickyClass(5), TrickyClass(3),
             TrickyClass(1), TrickyClass(4), TrickyClass(5)});

    bst.erase(TrickyClass(4));
    sorted.erase(sorted.begin() + 3);
    EXPECT_EQ(bst.to_vector(), sorted);
    EXPECT_EQ(bst.size(), 6);

    bst.erase(TrickyClass(5));
    sorted.erase(sorted.begin() + 3);
    EXPECT_EQ(bst.to_vector(), sorted);
    EXPECT_EQ(bst.size(), 5);

    bst.erase(TrickyClass(5));
    sorted.erase(sorted.begin() + 3);
    EXPECT_EQ(bst.to_vector(), sorted);
    EXPECT_EQ(bst.size(), 4);

    bst.erase(TrickyClass(5));
    EXPECT_EQ(bst.to_vector(), sorted);
    EXPECT_EQ(bst.size(), 4);

    sorted.clear();
    bst.clear();
    EXPECT_EQ(bst.to_vector(), sorted);
    EXPECT_EQ(bst.size(), 0);
    EXPECT_TRUE(bst.empty());
  }
  {
    BinarySearchTree<int> bst;
    bst.erase(3);
    EXPECT_EQ(bst.size(), 0);
    bst.clear();
    EXPECT_EQ(bst.size(), 0);
  }
}

TEST(BinarySearchTree, FindEraseTests) {
  {
    BinarySearchTree<TrickyClass>
        bst({TrickyClass(7), TrickyClass(1), TrickyClass(5), TrickyClass(3),
             TrickyClass(1), TrickyClass(4), TrickyClass(5)});

    bst.erase(TrickyClass(4));
    auto it = bst.find(TrickyClass(4));
    EXPECT_EQ(it, bst.end());

    it = bst.find(TrickyClass(7));
    EXPECT_EQ(*it, TrickyClass(7));

    bst.erase(TrickyClass(5));
    auto it2 = bst.find(TrickyClass(5));
    EXPECT_EQ(*it2, TrickyClass(5));

    bst.erase(TrickyClass(5));
    it2 = bst.find(TrickyClass(5));
    EXPECT_EQ(it2, bst.end());

    EXPECT_EQ(*it, TrickyClass(7));
    bst.erase(it);

    it = bst.find(TrickyClass(7));
    EXPECT_EQ(it, bst.end());
  }
  {
    BinarySearchTree<TrickyClass>
        bst({TrickyClass(5), TrickyClass(1), TrickyClass(7), TrickyClass(3),
             TrickyClass(1), TrickyClass(4), TrickyClass(5)});

    bst.erase(TrickyClass(4));
    auto it = bst.find(TrickyClass(4));
    EXPECT_EQ(it, bst.end());

    it = bst.find(TrickyClass(7));
    EXPECT_EQ(*it, TrickyClass(7));

    bst.erase(TrickyClass(5));
    auto it2 = bst.find(TrickyClass(5));
    EXPECT_EQ(*it2, TrickyClass(5));

    bst.erase(TrickyClass(5));
    it2 = bst.find(TrickyClass(5));
    EXPECT_EQ(it2, bst.end());

    EXPECT_EQ(*it, TrickyClass(7));
    bst.erase(it);

    it = bst.find(TrickyClass(7));
    EXPECT_EQ(it, bst.end());
  }
}

TEST(BinarySearchTree, ComparisonTests) {
  {
    BinarySearchTree<TrickyClass>
        bst1({TrickyClass(7), TrickyClass(1), TrickyClass(5), TrickyClass(3),
              TrickyClass(1), TrickyClass(4), TrickyClass(5)});
    BinarySearchTree<TrickyClass>
        bst2({TrickyClass(5), TrickyClass(1), TrickyClass(7), TrickyClass(3),
              TrickyClass(1), TrickyClass(4), TrickyClass(5)});
    EXPECT_TRUE(bst1 == bst2);
    EXPECT_FALSE(bst1 != bst2);
  }

  {
    BinarySearchTree<TrickyClass>
        bst1({TrickyClass(7), TrickyClass(2), TrickyClass(5), TrickyClass(3),
              TrickyClass(1), TrickyClass(4), TrickyClass(5)});
    BinarySearchTree<TrickyClass>
        bst2({TrickyClass(5), TrickyClass(1), TrickyClass(7), TrickyClass(3),
              TrickyClass(1), TrickyClass(4), TrickyClass(5)});
    EXPECT_TRUE(bst1 != bst2);
    EXPECT_FALSE(bst1 == bst2);
  }
}

TEST(BinarySearchTree, MiscTests) {
  {
    BinarySearchTree<int> bst_int = {1, 2, 3, 4, 5};
    EXPECT_EQ(bst_int.to_vector(), std::vector<int>({1, 2, 3, 4, 5}));
    EXPECT_EQ(bst_int.count(1), 1);
    EXPECT_TRUE(bst_int.contains(1));
    bst_int.erase(4);
    bst_int.erase(2);
    EXPECT_EQ(bst_int.to_vector(), std::vector<int>({1, 3, 5}));
    bst_int.erase(1);
    bst_int.erase(3);
    bst_int.erase(5);
    EXPECT_EQ(bst_int.to_vector(), std::vector<int>({}));
  }
  {
    BinarySearchTree<int> bst_int = {2, 3, 1, 5, 4};
    EXPECT_EQ(bst_int.to_vector(), std::vector<int>({1, 2, 3, 4, 5}));
    EXPECT_EQ(bst_int.count(1), 1);
    EXPECT_TRUE(bst_int.contains(1));
    bst_int.erase(4);
    bst_int.erase(2);
    EXPECT_EQ(bst_int.to_vector(), std::vector<int>({1, 3, 5}));
  }
  {
    BinarySearchTree<int> bst_int;
    bst_int.insert(1);
    EXPECT_EQ(bst_int.to_vector(), std::vector<int>({1}));
    bst_int.erase(1);
    EXPECT_EQ(bst_int.to_vector(), std::vector<int>({}));
  }
}