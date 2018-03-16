#include "node.h"

namespace llnode {
namespace node {

template <typename T, typename C>
T Queue<T, C>::Iterator::operator*() const {
  return T::FromListNode(node_, current_);
}

template <typename T, typename C>
const typename Queue<T, C>::Iterator Queue<T, C>::Iterator::operator++() {
  // TODO (mmarchini) check errors
  lldb::SBError sberr;

  addr_t current = current_ + constants_->kNextOffset;
  current = node_->process().ReadPointerFromMemory(current, sberr);
  current_ = current;
  return Iterator(node_, current, constants_);
}

template <typename T, typename C>
bool Queue<T, C>::Iterator::operator!=(const Iterator& that) const {
  return current_ != that.current_;
}

template <typename T, typename C>
typename Queue<T, C>::Iterator Queue<T, C>::begin() const {
  // TODO (mmarchini) check errors
  lldb::SBError sberr;
  addr_t currentNode = raw_ + constants_->kHeadOffset;

  currentNode = currentNode + constants_->kNextOffset;
  currentNode = node_->process().ReadPointerFromMemory(currentNode, sberr);

  return Iterator(node_, currentNode, constants_);
}

template <typename T, typename C>
typename Queue<T, C>::Iterator Queue<T, C>::end() const {
  return Iterator(node_, raw_ + constants_->kHeadOffset, constants_);
}
}
}