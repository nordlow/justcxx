class A;

class A
{
public:
    A(int x) : m_x(x) {}
    void run();
private:
    int m_x;
};

int g_x = 1;
int g_y = 1;

int fun1(int x);

#define INT int
int fun2(INT x);

typedef int i32;

union pair { int x, y; };
pair p;

enum days { monday, tuesday };
days d;

namespace ext {

class A;

class A
{
public:
    A(int x) : m_x(x) {}
    void run();
private:
    int m_x;
};

int g_x = 1;
int g_y = 1;

int fun1(int x);

#define INT int
int fun2(INT x);

typedef int i32;

union pair { int x, y; };
pair p;

enum days { monday, tuesday };
days d;

}
