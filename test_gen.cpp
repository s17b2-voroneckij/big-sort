#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <random>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " out_file num\n";
        return 1;
    }
    int fd = open(argv[1], O_CREAT | O_TRUNC | O_RDWR, 0666);
    long long num_ints = strtol(argv[2], NULL, 10);
    ftruncate(fd, num_ints * sizeof(long long));
    long long *memory = (long long *)mmap(NULL, num_ints * sizeof(long long), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    random_device rd;  //Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<long long > distrib(INT64_MIN, INT64_MAX);
    for (long long i = 0; i < num_ints; i++) {
        memory[i] = distrib(gen);
    }
    msync(memory, num_ints * sizeof(long long), MS_SYNC);
    munmap(memory, num_ints * sizeof(long long));
    close(fd);
}