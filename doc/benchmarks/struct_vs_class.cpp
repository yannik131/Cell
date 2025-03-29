/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <chrono>
#include <iostream>

struct data1
{
    int i = 0;
};

class data2
{
public:
    inline int getI() const noexcept
    {
        return i;
    }
    inline void setI(int val) noexcept
    {
        i = val;
    }

private:
    int i = 0;
};

int workStruct()
{
    data1 d;
    int sum = 0;
    for (int i = 0; i < 1e6; ++i)
    {
        sum += d.i;
        d.i += i - sum * d.i;
    }
    return sum;
}

int workClass()
{
    data2 d;
    int sum = 0;
    for (int i = 0; i < 1e6; ++i)
    {
        sum += d.getI();
        d.setI(d.getI() + i - sum * (d.getI() - i));
    }
    return sum;
}

void measure(int (*func)(), const char* msg)
{
    auto start = std::chrono::high_resolution_clock::now();
    long long sum = 0;
    for (int i = 0; i < 100; ++i)
        sum += func();
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << msg << duration.count() / 100. << "ms\t(" << sum << ")\n";
}

int main()
{
    measure(&workStruct, "struct: ");
    measure(&workClass, "class: ");

    return 0;
}