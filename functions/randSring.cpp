#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

string generateRandomString(int length) {
    const string chars =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

    string result;
    result.reserve(length);

    srand(time(0)); // Seed the random number generator (once per run)

    for (int i = 0; i < length; ++i) {
        result += chars[rand() % chars.size()];
    }

    return result;
}

int main() {
    return 0;
}
