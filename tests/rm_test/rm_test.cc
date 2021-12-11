#include <machine/display.h>
#include <time.h>
#include <real-time.h>

using namespace EPOS;

OStream cout;
Chronometer chronometer;

int handler_thread1() {
    int count = 0;
    while (Periodic_Thread::wait_next()) {
        Microsecond current_time = chronometer.read() / 1000;
        count++;
        cout << "\n"<< current_time << " ms\t" << "Running Thread1";
    }
    return 0;
}

int handler_thread2() {
    int count = 0;
    while (Periodic_Thread::wait_next()) {
        Microsecond current_time = chronometer.read() / 1000;
        count++;
        cout << "\n"<< current_time << " ms\t" << "Running Thread2";
    }
    return 0;
}

int handler_thread3() {
    int count = 0;
    while (Periodic_Thread::wait_next()) {
        Microsecond current_time = chronometer.read() / 1000;
        count++;
        cout << "\n"<< current_time << " ms\t" << "Running Thread3";
    }
    return 0;
}

int handler_thread4() {
    int count = 0;
    while (Periodic_Thread::wait_next()) {
        Microsecond current_time = chronometer.read() / 1000;
        count++;
        cout << "\n"<< current_time << " ms\t" << "Running Thread4";
    }
    return 0;
}

int main() {
    Periodic_Thread * thread1;
    Periodic_Thread * thread2;
    Periodic_Thread * thread3;
    Periodic_Thread * thread4;

    int times = 10;
    thread1 = new Periodic_Thread(RTConf(100000, 0, 0, 0, times), &handler_thread1);
    thread2 = new Periodic_Thread(RTConf(200000, 0, 0, 0, times), &handler_thread2);
    thread3 = new Periodic_Thread(RTConf(300000, 0, 0, 0, times), &handler_thread3);
    thread4 = new Periodic_Thread(RTConf(400000, 0, 0, 0, times), &handler_thread4);

    cout << "Priority - "
        << "Thread1=" << thread1->priority() << ", "
        << "Thread2=" << thread2->priority() << ", "
        << "Thread3=" << thread3->priority() << ", "
        << "Thread4=" << thread4->priority();

    chronometer.start();
    thread1->join();
    thread2->join();
    thread3->join();
    thread4->join();
    chronometer.stop();

    return 0;
}
