#ifndef BINARY_SEARCH_TREE_H_
#define BINARY_SEARCH_TREE_H_

#include <initializer_list>
#include <iterator>
#include <vector>

template<class T>
class BinarySearchTree {
 private:
  struct TreeNode;

 public:
  BinarySearchTree() = default;

  BinarySearchTree(const std::initializer_list<T>& list);

  BinarySearchTree(const BinarySearchTree& rhs);
  BinarySearchTree(BinarySearchTree&& rhs) noexcept;

  ~BinarySearchTree();

  BinarySearchTree& operator=(const BinarySearchTree& rhs);
  BinarySearchTree& operator=(BinarySearchTree&& rhs) noexcept;

  int size() const;
  bool empty() const;

  bool contains(const T& value) const;

  int count(const T& value) const;

  template<class... Args>
  void emplace(Args&& ... args);

  template<class U>
  void insert(U&& value);

  void erase(const T& value);

  void clear();

  std::vector<T> to_vector() const;

  bool operator==(const BinarySearchTree<T>& rhs) const;
  bool operator!=(const BinarySearchTree<T>& rhs) const;

  class ConstIterator : std::iterator<std::bidirectional_iterator_tag, T> {
    friend class BinarySearchTree;
   public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;
    using iterator_category = std::bidirectional_iterator_tag;

    const T& operator*() const;

    const T* operator->() const;

    ConstIterator& operator++();
    ConstIterator operator++(int);

    ConstIterator& operator--();
    ConstIterator operator--(int);

    bool operator==(ConstIterator rhs) const;
    bool operator!=(ConstIterator rhs) const;

   private:
    ConstIterator(TreeNode* tree_node, const BinarySearchTree<T>* owner);

    TreeNode* tree_node_;
    const BinarySearchTree<T>* owner_;
  };
  ConstIterator begin() const;

  ConstIterator end() const;

  ConstIterator find(const T& value) const;

  void erase(ConstIterator iter);

 private:
  void FindFirstNode();
  void FindLastNode();

  int CalcCount(const TreeNode* node, const T& value) const;

  struct TreeNode {
    template<class... Args>
    explicit TreeNode(Args&& ... args);

    T value;
    TreeNode* parent = nullptr;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
  };

  // return pointer to copied node
  TreeNode* CopyTree(const TreeNode& node_to_copy);

  void DeleteTree(TreeNode* node);

  // pointers in node do not change
  void Detach(TreeNode* node);

  void DetachBothNull(TreeNode* node);
  void DetachRightNull(TreeNode* node);
  void DetachLeftNull(TreeNode* node);
  void DetachNeitherNull(TreeNode* node);

  TreeNode** FindPointerToPointerToChild(TreeNode* parent,
                                         TreeNode* child) const;
  // Do not change new_child, old_child
  void ChangeChild(TreeNode* parent, TreeNode* old_child, TreeNode* new_child);

  TreeNode* root_ = nullptr;
  TreeNode* first_node_ = nullptr;
  TreeNode* last_node_ = nullptr;
  int size_ = 0;
};

// definitions

template<class T>
BinarySearchTree<T>::BinarySearchTree(const std::initializer_list<T>& list) {
  for (auto& value : list) {
    insert(value);
  }
}

template<class T>
BinarySearchTree<T>::BinarySearchTree(const BinarySearchTree& rhs) :
    root_(CopyTree(*(rhs.root_))), size_(rhs.size_) {
  FindFirstNode();
  FindLastNode();
}

template<class T>
BinarySearchTree<T>::BinarySearchTree(BinarySearchTree&& rhs) noexcept :
    root_(rhs.root_), first_node_(rhs.first_node_), size_(rhs.size_) {
  rhs.root_ = nullptr;
  rhs.first_node_ = nullptr;
  rhs.size_ = 0;
}

template<class T>
BinarySearchTree<T>::~BinarySearchTree() {
  clear();
}

template<class T>
BinarySearchTree<T>& BinarySearchTree<T>::operator=
    (const BinarySearchTree& rhs) {
  if (this != &rhs) {
    clear();
    root_ = CopyTree(*(rhs.root_));
    size_ = rhs.size_;
    FindFirstNode();
    FindLastNode();
  }
  return *this;;
}

template<class T>
BinarySearchTree<T>& BinarySearchTree<T>::operator=
    (BinarySearchTree&& rhs) noexcept {
  if (this != &rhs) {
    clear();

    root_ = rhs.root_;
    first_node_ = rhs.first_node_;
    size_ = rhs.size_;

    rhs.root_ = nullptr;
    rhs.first_node_ = nullptr;
    rhs.size_ = 0;
  }
  return *this;
}

template<class T>
int BinarySearchTree<T>::size() const {
  return size_;
}

template<class T>
bool BinarySearchTree<T>::empty() const {
  return size_ == 0;
}

template<class T>
bool BinarySearchTree<T>::contains(const T& value) const {
  return find(value) != end();
}

template<class T>
int BinarySearchTree<T>::count(const T& value) const {
  return CalcCount(root_, value);
}

template<class T>
void BinarySearchTree<T>::clear() {
  DeleteTree(root_);

  root_ = nullptr;
  first_node_ = nullptr;
  last_node_ = nullptr;
  size_ = 0;
}

template<class T>
bool BinarySearchTree<T>::operator==(const BinarySearchTree<T>& rhs) const {
  if (size_ != rhs.size_) {
    return false;
  }
  auto it_this = begin();
  for (const auto& value : rhs) {
    if (!(*it_this == value)) {
      return false;
    }
    ++it_this;
  }
  return true;
}

template<class T>
bool BinarySearchTree<T>::operator!=(const BinarySearchTree<T>& rhs) const {
  return !(*this == rhs);
}

// ConstIterator

template<class T>
BinarySearchTree<T>::ConstIterator::ConstIterator
    (BinarySearchTree::TreeNode* tree_node, const BinarySearchTree<T>* owner) :
    tree_node_(tree_node), owner_(owner) {}

template<class T>
const T& BinarySearchTree<T>::ConstIterator::operator*() const {
  return tree_node_->value;
}

template<class T>
const T* BinarySearchTree<T>::ConstIterator::operator->() const {
  return &(tree_node_->value);
}

template<class T>
typename BinarySearchTree<T>::ConstIterator&
BinarySearchTree<T>::ConstIterator::operator++() {
  if (tree_node_->right != nullptr) {
    tree_node_ = tree_node_->right;
    while (tree_node_->left != nullptr) {
      tree_node_ = tree_node_->left;
    }
  } else {
    TreeNode* prev = nullptr;
    while (tree_node_ != nullptr && tree_node_->right == prev) {
      prev = tree_node_;
      tree_node_ = tree_node_->parent;
    }
  }

  return *this;
}

template<class T>
typename BinarySearchTree<T>::ConstIterator
BinarySearchTree<T>::ConstIterator::operator++(int) {
  auto copy = *this;
  ++(*this);
  return copy;
}

template<class T>
typename BinarySearchTree<T>::ConstIterator&
BinarySearchTree<T>::ConstIterator::operator--() {
  if (tree_node_ == nullptr) {
    tree_node_ = owner_->last_node_;
    return *this;
  }

  if (tree_node_->left != nullptr) {
    tree_node_ = tree_node_->left;
    while (tree_node_->right != nullptr) {
      tree_node_ = tree_node_->right;
    }
  } else {
    TreeNode* prev = nullptr;
    while (tree_node_ != nullptr && tree_node_->left == prev) {
      prev = tree_node_;
      tree_node_ = tree_node_->parent;
    }
  }

  return *this;
}

template<class T>
typename BinarySearchTree<T>::ConstIterator
BinarySearchTree<T>::ConstIterator::operator--(int) {
  auto copy = *this;
  --(*this);
  return copy;
}

template<class T>
std::vector<T> BinarySearchTree<T>::to_vector() const {
  std::vector<T> vec;
  for (const T& value : *this) {
    vec.push_back(value);
  }
  return vec;
}

template<class T>
bool BinarySearchTree<T>::ConstIterator::operator==(ConstIterator rhs) const {
  return (tree_node_ == rhs.tree_node_) && (owner_ == rhs.owner_);
}

template<class T>
bool BinarySearchTree<T>::ConstIterator::operator!=
    (BinarySearchTree::ConstIterator rhs) const {
  return !(*this == rhs);
}

// -ConstIterator

template<class T>
template<class... Args>
BinarySearchTree<T>::TreeNode::TreeNode(Args&& ... args) :
    value(std::forward<Args>(args)...) {}

template<class T>
typename BinarySearchTree<T>::ConstIterator BinarySearchTree<T>::begin() const {
  return {first_node_, this};
}

template<class T>
typename BinarySearchTree<T>::ConstIterator BinarySearchTree<T>::end() const {
  return ConstIterator(nullptr, this);
}

template<class T>
typename BinarySearchTree<T>::ConstIterator
BinarySearchTree<T>::find(const T& value) const {
  TreeNode* cur_node = root_;
  while (cur_node != nullptr && !(cur_node->value == value)) {
    if (value < cur_node->value) {
      cur_node = cur_node->left;
    } else {
      cur_node = cur_node->right;
    }
  }

  return {cur_node, this};
}

template<class T>
void BinarySearchTree<T>::erase(BinarySearchTree::ConstIterator iter) {
  --size_;
  Detach(iter.tree_node_);
  delete iter.tree_node_;
}

template<class T>
void BinarySearchTree<T>::erase(const T& value) {
  auto it = find(value);
  if (it != end()) {
    erase(it);
  }
}

template<class T>
template<class U>
void BinarySearchTree<T>::insert(U&& value) {
  emplace(std::forward<U>(value));
}

template<class T>
template<class... Args>
void BinarySearchTree<T>::emplace(Args&& ... args) {
  auto* added_node = new TreeNode(std::forward<Args>(args)...);

  TreeNode* cur_node = root_;
  TreeNode* parent = nullptr;
  while (cur_node != nullptr) {
    if (added_node->value < cur_node->value) {
      parent = cur_node;
      cur_node = cur_node->left;
    } else {
      parent = cur_node;
      cur_node = cur_node->right;
    }
  }

  added_node->parent = parent;

  if (parent != nullptr) {
    if (added_node->value < parent->value) {
      parent->left = added_node;
      if (parent == first_node_) {
        first_node_ = added_node;
      }
    } else {
      parent->right = added_node;
      if (parent == last_node_) {
        last_node_ = added_node;
      }
    }
  } else {
    root_ = added_node;
    first_node_ = added_node;
    last_node_ = added_node;
  }
  ++size_;
}

template<class T>
int BinarySearchTree<T>::CalcCount(const TreeNode* node, const T& value) const {
  if (node == nullptr) {
    return 0;
  }

  if (value < node->value) {
    return CalcCount(node->left, value);
  } else {
    return CalcCount(node->right, value) + (value == node->value ? 1 : 0);
  }
}

template<class T>
void BinarySearchTree<T>::FindFirstNode() {
  first_node_ = root_;

  if (root_ == nullptr) {
    return;
  }

  while (first_node_->left != nullptr) {
    first_node_ = first_node_->left;
  }
}

template<class T>
void BinarySearchTree<T>::FindLastNode() {
  last_node_ = root_;

  if (root_ == nullptr) {
    return;
  }

  while (last_node_->right != nullptr) {
    last_node_ = last_node_->right;
  }
}

template<class T>
typename BinarySearchTree<T>::TreeNode* BinarySearchTree<T>::CopyTree
    (const BinarySearchTree::TreeNode& node_to_copy) {
  auto* copied_node = new TreeNode(node_to_copy.value);
  if (node_to_copy.left != nullptr) {
    copied_node->left = CopyTree(*(node_to_copy.left));
    (copied_node->left)->parent = copied_node;
  }

  if (node_to_copy.right != nullptr) {
    copied_node->right = CopyTree(*(node_to_copy.right));
    (copied_node->right)->parent = copied_node;
  }

  return copied_node;
}

template<class T>
void BinarySearchTree<T>::DeleteTree(BinarySearchTree::TreeNode* node) {
  if (node == nullptr) {
    return;
  }

  DeleteTree(node->left);
  DeleteTree(node->right);

  delete node;
}

// pointers in node do not change
template<class T>
void BinarySearchTree<T>::Detach(TreeNode* node) {
  if (node->left == nullptr && node->right == nullptr) {
    DetachBothNull(node);
    return;
  }

  if (node->left != nullptr && node->right == nullptr) {
    DetachRightNull(node);
    return;
  }

  if (node->left == nullptr && node->right != nullptr) {
    DetachLeftNull(node);
    return;
  }

  DetachNeitherNull(node);
}

template<class T>
void BinarySearchTree<T>::DetachBothNull(BinarySearchTree::TreeNode* node) {
  if (node == first_node_) {
    first_node_ = node->parent;
  }
  if (node == last_node_) {
    last_node_ = node->parent;
  }

  ChangeChild(node->parent, node, nullptr);
  if (node == root_) {
    root_ = nullptr;
  }
}

template<class T>
void BinarySearchTree<T>::DetachRightNull(BinarySearchTree::TreeNode* node) {
  if (node == last_node_) {
    ConstIterator it(last_node_, this);
    --it;
    last_node_ = it.tree_node_;
  }

  ChangeChild(node->parent, node, node->left);
  if (node == root_) {
    root_ = node->left;
  }
  (node->left)->parent = node->parent;
}

template<class T>
void BinarySearchTree<T>::DetachLeftNull(BinarySearchTree::TreeNode* node) {
  if (node == first_node_) {
    ConstIterator it(first_node_, this);
    ++it;
    first_node_ = it.tree_node_;
  }

  ChangeChild(node->parent, node, node->right);
  (node->right)->parent = node->parent;
  if (node == root_) {
    root_ = node->right;
  }
}

template<class T>
void BinarySearchTree<T>::DetachNeitherNull(BinarySearchTree::TreeNode* node) {
  TreeNode* almost_left = node->right;
  while (almost_left->left != nullptr) {
    almost_left = almost_left->left;
  }

  Detach(almost_left);

  almost_left->parent = node->parent;
  almost_left->left = node->left;
  almost_left->right = node->right;

  (almost_left->left)->parent = almost_left;
  if (almost_left->right != nullptr) {
    (almost_left->right)->parent = almost_left;
  }

  ChangeChild(almost_left->parent, node, almost_left);
  if (node == root_) {
    root_ = almost_left;
  }
}

template<class T>
typename BinarySearchTree<T>::TreeNode**
BinarySearchTree<T>::FindPointerToPointerToChild
    (BinarySearchTree::TreeNode* parent,
     BinarySearchTree::TreeNode* child) const {
  if (parent == nullptr) {
    return nullptr;
  }

  if (parent->left == child) {
    return &(parent->left);
  } else {
    return &(parent->right);
  }
}

// Do not change new_child, old_child
template<class T>
void BinarySearchTree<T>::ChangeChild(BinarySearchTree::TreeNode* parent,
                                      BinarySearchTree::TreeNode* old_child,
                                      BinarySearchTree::TreeNode* new_child) {
  TreeNode** place_for_child = FindPointerToPointerToChild(parent, old_child);
  if (place_for_child == nullptr) {
    return;
  }
  *place_for_child = new_child;
}

#endif  // BINARY_SEARCH_TREE_H_
