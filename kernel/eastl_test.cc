#include <EASTL/vector.h>

int main()
{
    eastl::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    for(auto it: vec)
        cout<< it<< " ";
    return 0;
}