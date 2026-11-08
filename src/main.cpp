#include "core/Engine.h"
#include <iostream>

int main() {
    try {
        Engine engine(1280, 720);
        engine.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
