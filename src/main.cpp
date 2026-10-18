#include "ConsoleUI.h"

#include <iostream>

int main() {
    PhoneDirectory directory;
    ConsoleUI ui(directory);

    ui.run();

    return 0;
}
