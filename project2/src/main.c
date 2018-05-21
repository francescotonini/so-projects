#include <padre.h>
#include <tools.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        println("Usage: key_finder <input> <output>");
        return 1;
    }

    // Here we go!
    exit(padre(argv[1], argv[2]));
}
