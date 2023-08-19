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
#include "iolib.h"

typedef unsigned long long int64;

using namespace std;

const int64 SORT_DIVIDE = 1;
const int64 MAX_P = 1000;
const bool NEED_DATASYNC = false;

string makeFileName(const string &tmpPath, int i) {
    return tmpPath + "/" + to_string(i);
}

void makeSort(const string &inputPath, const string &outputPath, const string &tmpPath, int64 memoryLimit, int64 blockSize) {
    int64 max_in_mem = memoryLimit / sizeof(int64) / SORT_DIVIDE;
    int64* buf = (int64 *)malloc(max_in_mem * sizeof(int64));
    int first_free_file = 0; //
    int fd = open(inputPath.c_str(), O_RDONLY);
    if (fd < 0) {
        cerr << "open() failed " << strerror(errno) << endl;
        return;
    }
    struct stat file_info;
    fstat(fd, &file_info);
    int64 length = file_info.st_size / sizeof(int64);
    for (int64 i = 0; i < length; i += max_in_mem) {
        int64 curr_len = min(max_in_mem, length - i);
        int out_fd = -1;
        if (length <= max_in_mem) {
            out_fd = open(outputPath.c_str(), O_RDWR | O_TRUNC | O_CREAT, 0666);
        } else {
            out_fd = open(makeFileName(tmpPath, first_free_file++).c_str(), O_RDWR | O_TRUNC | O_CREAT, 0666);
        }
        my_read(fd, (char *)buf, curr_len * sizeof(int64));
        sort(buf, buf + curr_len);
        my_write(out_fd, (char *)buf, curr_len * sizeof(int64));
        if (NEED_DATASYNC) {
            fdatasync(out_fd);
        }
        close(out_fd);
    }
    int64 p = memoryLimit / blockSize;
    if (p > MAX_P) {
        p = MAX_P;
    }
    int64 next_p = first_free_file % (p - 1);
    if (next_p <= 1) {
        next_p += p - 1;
    }
    int first_not_processed = 0; // first_free_file - first_not_processed is the number of files to process
    while (first_free_file - first_not_processed > 1) {
        vector<BufferedReader> readers;
        priority_queue<pair<int64, int>, vector<pair<int64, int>>, greater<pair<int64, int>>> pq;
        for (int i = 0; i < next_p; i++) {
            readers.emplace_back(makeFileName(tmpPath, first_not_processed + i), blockSize);
            pq.push(make_pair(readers.back().Next().value(), i));
        }
        string new_file_path = makeFileName(tmpPath, first_free_file++);
        if (first_free_file - first_not_processed - next_p <= 1) {
            new_file_path = outputPath;
        }
        BufferedWriter writer(new_file_path, blockSize);
        while (!pq.empty()) {
            auto top = pq.top();
            pq.pop();
            writer.Write(top.first);
            auto new_number = readers[top.second].Next();
            if (new_number.has_value()) {
                pq.push(make_pair(new_number.value(), top.second));
            }
        }
        if (NEED_DATASYNC) {
            writer.Sync();
        }
        for (int i = 0; i < next_p; i++) {
            readers[i].Close();
            //cerr << "removing " << makeFileName(tmpPath, first_not_processed) << endl;
            remove(makeFileName(tmpPath, first_not_processed++).c_str());
        }
        next_p = p;
    }
    if (NEED_TIME) {
        cerr << "\n\nIO TIME: " << io_time << "\n\n";
    }
}

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

  makeSort(inputPath, outputPath, tmpPath, memoryLimit, blockSize);

  return 0;
}

