#include <utility/ostream.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "Hello world!" << endl;

    void * p1 = shared_memory();
    void * p2 = shared_memory();

    cout << p1 << endl;
    cout << p2 << endl;

    char ** p3 = reinterpret_cast<char **>(p1);
    char ** p4 = reinterpret_cast<char **>(p2);

    char hello[26] = "Hello from shared memory\0";
    *p3 = hello;

    cout << *p4 << endl;

    return 0;
}