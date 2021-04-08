/**
 * Exercise with 3 exam/interview style questions to help students
 * review questions for upcoming exam.
 * Copyright (C) 2021 John Doll
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <atomic>
#include <numeric>
#include <condition_variable>

// Forward declaration of a method to ease testing.
void increment(int& entry);

/** NOTE: To keep things simple for exams, you may add global
    variables as you see fit */

namespace shared {
    std::atomic<int> num = ATOMIC_VAR_INIT(0);
}

/**
 * The following printInOrder method is called from many threads, with
 * id (i.e., 0, 1, 2, ...) indicating the logical number of
 * thread. Implement the following method to print (to std::cout) the
 * supplied data in the order id value, as shown in the sample output.
 *
 * \param[in] The unique ID associated with the thread that is called
 * this method.
 *
 * \param[in] data The data to be printed by this thread.
 */
void printInOrder(const int id, std::string data) {
    // Implement this method.
    // while loop to iteratae until it is this thread's turn to print (when id
    // is equal to the shared value num
    while (shared::num != id) {}
    // print and increment num
    std::cout << "id " << id << ": " << data << std::endl;
    shared::num++;
}

/**
 * The following method is called from a separate thread to produce a
 * series of numbers (i.e., 0, 1, 2, ..., n-1) in a shared queue for
 * another thread to process.  Complete this method to operate in
 * concert with the consumer method.
 *
 * NOTE: You must not modify the consumer method and the maximum queue
 * size is 5.
 *
 * \param[out] q The vector to which the must be added (to the end of
 * the queue).
 *
 * \param[out] qMutex A mutex that is shared between the producer and
 * consumer methods.
 *
 * \param[out] qCond A condition variable that is shared between the
 * producer and consumer methods.
 *
 * \param[in] n The maximum value up to (but not included) which a
 * series of numbers are to be generated by the producer.
 */
void producer(std::vector<int>& q, std::mutex& qMutex,
             std::condition_variable& qCondVar, int n) {
    // Impelement this method so that it works consistently with the
    // consumer method (see further below)
    if (q.size() == 5) {
        std::unique_lock<std::mutex> lock(qMutex);
        // Wait until queue is not empty
        qCondVar.wait(lock, [&q]{ return q.size() < 5; });
    }
    
}


/**
 * This method counts numbers that end with a given digit.  However,
 * it has not been effectively multithreaded.  Fix the issues with
 * this method so that it operates effectively.
 *
 * @param values The values to be distributed by this method.
 *
 * @param n The number of threads to be used.
 */
std::vector<int> countDigits(const std::vector<int> values, int n) {
    // Results to be returned by this method.  This vector has fixed
    // size of 10, because numbers can end with only one of the 10
    // digits.
    std::vector<int> room(10);

    // Here is an inner-method that is run from separate threads.
    // Since it is a small method it has been placed here to
    // streamline working on this exam problem.
    auto thrMain = [&](int id) {
        std::vector<std::mutex> lock;
            for (size_t i = id; (i < values.size()); i += n) {
                std::mutex mut;
                lock[id] = mut;
                mut.lock();
                // Use helper method to increment appropriate counter
                const int roomNum = values[i] % 10;
                increment(room[roomNum]);
                mut.unlock();
            }
        };


    // You will not need to modify code below this line for this question

    // Create the threads to run the loop below:
    std::vector<std::thread> thrList;
    for (int i = 0; (i < n); i++) {
        thrList.push_back(std::thread(thrMain, i));
    }
    // Wait for the threads to finish
    for (auto& t : thrList) {
        t.join();
    }

    // Return the results back to the caller
    return room;
}

//---------------------------------------------------------
//      DO  NOT  MODIFY  CODE  BELOW  THIS  LINE
//---------------------------------------------------------

/**
 * The consumer method used for some basic testing of the operations
 * of the producer method.
 *
 * \param[out] q The vector to which the must be added (to the end of
 * the queue).
 *
 * \param[out] qMutex A mutex that is shared between the producer and
 * consumer methods.
 *
 * \param[out] qCond A condition variable that is shared between the
 * producer and consumer methods.
 *
 * \param[in] n The maximum value up to (but not included) which a
 * series of numbers are to be generated by the producer.
 *
 */
void consumer(std::vector<int>& q, std::mutex& qMutex,
             std::condition_variable& qCondVar, int n) {
    // Consume 'n' values that are going to be produced.
    for (int i = 0; (i < n); i++) {
        int value = -1;   // Changed in the CS below.
        
        {   // begin Critical Section (CS)
            std::unique_lock<std::mutex> lock(qMutex);
            // Wait until queue is not empty
            qCondVar.wait(lock, [&q]{ return !q.empty(); });
            // Extract value and process it outside the critical section.
            assert(q.size() <= 5);
            value = q.front();   // Get the value
            q.erase(q.begin());  // Erase the value

            // Let the producer know it can work more. If we forget to
            // notify (i.e., wake-up the producer) then
            // the program will hang.
            qCondVar.notify_one();
        }   // end Critical Section (CS)

        // Process the value
        assert(value == i);
    }
}

// End of source code
