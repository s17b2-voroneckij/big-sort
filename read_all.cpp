#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory.h>
#include <queue>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <optional>
#include <sys/mman.h>

typedef long long int64;


using namespace std;

int main(int argc, char *argv[]) {
    int fd = open(argv[1], O_RDONLY);
    struct stat fileinfo;
    fstat(fd, &fileinfo);
    int size = fileinfo.st_size;
    int64 *ptr = (int64 *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    for (int i = 0; i < size / sizeof(int64); i++) {
        cout << ptr[i] << "\n";
    }
 }