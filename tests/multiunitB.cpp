#include <cmath>
    
int
somefunc(int i, float f)
{
    auto x = i * f;
    auto y = pow(x, f);
    return static_cast<int>(y);
}
