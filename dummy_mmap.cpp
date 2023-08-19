#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

typedef long long int64;

using namespace std;


int main(int argc, char** argv) {
    if (argc < 6) {
        cerr << "Incorrect number of arguments: " << argc - 1 << ", expected 5" << endl;
        cerr << "Usage: <inputPath> <outputPath> <tmpPath> <memoryLimit> <blockSize>" << endl;
        return 1;
    }

    string inputPath = argv[1];
    string outputPath = argv[2];
    string tmpPath = argv[3];
    int64 memoryLimit, blockSize;
    {
        istringstream is(argv[4]);
        is >> memoryLimit;
    }
    {
        istringstream is(argv[5]);
        is >> blockSize;
    }
    int fd = open(inputPath.c_str(), O_RDWR);
    if (fd < 0) {
        cerr << "open() failed " << strerror(errno) << endl;
        return 1;
    }
    struct stat file_info;
    fstat(fd, &file_info);
    int64 length = file_info.st_size / sizeof(int64);
    int64* memory = (int64*)mmap(nullptr, file_info.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    sort(memory, memory + length);
    munmap(memory, file_info.st_size);
    close(fd);
    return 0;
}