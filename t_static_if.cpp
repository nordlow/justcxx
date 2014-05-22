/*!
 * \file t_static_if.cpp
 * \brief
 */

#include <iostream>
#include <string>
#include <../static_if-gist-827090/static_if.hpp>

template<int N> //(REQUIRED) function template or member function template
void func() {
    int a;
    STATIC_IF( N>0 ) { //(REQUIRED) if block
        func<N-1>();
    } STATIC_ELSEIF( N==1 ) { //(optional) elseif block
        STATIC_IF(true){ //(optional) nested if block
            cout << "foo";
        }STATIC_ENDIF
             } STATIC_ELSEIF( N==0 ) { //(optional) can use elseif as many times
        cout << a; // can use local variable
    } STATIC_ELSE { //(optional) else block
        STATIC_BREAKIF //(optional) break from innermost STATIC block
            } STATIC_ENDIF //(REQUIRED) end block
                  }

int main(int argc, const char * argv[], const char * envp[])
{
    func();
    return 0;
}
