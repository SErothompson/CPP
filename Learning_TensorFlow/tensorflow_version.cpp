#include <tensorflow/c/c_api.h>
#include <iostream>

int main() {
    std::cout << "TensorFlow C++ version: " << TF_Version() << std::endl;
    return 0;
}
