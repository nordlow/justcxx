#include <type_traits>

/*! Add \p a with \p b.
  * \param[in] a is first
  * \param[in] b is second
  * \return sum of \p a and \p b.
  */
template<class U, class S, class C = typename std::common_type<U,S>::type>
typename std::enable_if<std::is_unsigned<U>::value && std::is_signed<S>::value, C>::type add(U a, S b) {
    return a + b;
}

int main(int argc, const char * argv[], const char * envp[])
{
    unsigned int ui;
    int i;
    auto a = add(ui, i);
    return 0;
}
