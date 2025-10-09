#include <thread>
#include <cassert>
#include <iostream>
#include "../include/blocking_queue.hpp"

int main() {
  BlockingQueue<int> q(2);
  assert(q.empty());
  assert(!q.closed());
  q.push(1);
  q.push(2);
  int x;
  bool got = q.try_pop(x);
  if (got) { /* ok */ }

  q.push(int{3});
  std::thread consumer([&](){
    int y;
    while (q.pop(y)) { /* drain until close */ }
  });

  (void)q.size();
  (void)q.empty();
  (void)q.closed();

  q.close();
  consumer.join();
  bool ok = q.push(42);
  if (ok) std::cerr << "push after close should fail\n";
  return 0;
}
