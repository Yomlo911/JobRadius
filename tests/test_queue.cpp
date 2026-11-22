#include "test_helpers.h"
#include "../src/Queue.h"

int main() {
    cout << "Queue (Queue.h)\n";
    Queue<int> q;
    check(q.isEmpty() && q.size() == 0, "new queue is empty");
    // checks fifo, enqueue, dequeue 
    q.enqueue(10); q.enqueue(20); q.enqueue(30);
    check(q.size() == 3 && !q.isEmpty(), "size is 3 after three enqueues");
    check(q.front() == 10, "front shows the oldest item without removing it");
    check(q.size() == 3, "front does not change the size");

    check(q.dequeue() == 10 && q.dequeue() == 20, "dequeue returns items first in, first out");
    q.enqueue(40);
    check(q.dequeue() == 30 && q.dequeue() == 40, "enqueue between dequeues keeps the order");
    check(q.isEmpty() && q.size() == 0, "queue is empty after removing everything");

    bool threw = false;
    try { q.dequeue(); } catch (const runtime_error&) { threw = true; }
    check(threw, "dequeue on an empty queue throws");
    threw = false;
    try { q.front(); } catch (const runtime_error&) { threw = true; }
    check(threw, "front on an empty queue throws");

    for (int round = 1; round <= 3; round++) {
        for (int i = 0; i < 100; i++) q.enqueue(i);
        bool inOrder = true;
        for (int i = 0; i < 100; i++) if (q.dequeue() != i) inOrder = false;
        check(inOrder && q.isEmpty(), "100 items in, 100 out in order (reuse round " + to_string(round) + ")");
    }

    Queue<string> qs;
    qs.enqueue("a"); qs.enqueue("b");
    check(qs.dequeue() == "a" && qs.dequeue() == "b", "works with strings too");
    return finish();
}
