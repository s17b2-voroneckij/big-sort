typedef unsigned long long int64;
using std::vector;
using std::string;
using std::optional;

double io_time = 0;
const bool NEED_TIME = true;

double my_gettime() {
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    return time.tv_sec + time.tv_nsec * 1e-9;
}

int my_read(int fd, char* curr_buf, int64 count) {
    if (NEED_TIME) {
        io_time -= my_gettime();
    }
    int completed = 0;
    while (completed < count) {
        int r = read(fd, curr_buf + completed, count - completed);
        if (r == 0) {
            break;
        }
        if (errno != 0) {
            if (errno == EAGAIN || errno == EINTR) {
                errno = 0;
                continue;
            } else {
                errno = 0;
                break;
            }
        }
        completed += r;
    }
    if (NEED_TIME) {
        io_time += my_gettime();
    }
    return completed;
}

void my_write(int fd, char* curr_buf, int64 count) {
    if (NEED_TIME) {
        io_time -= my_gettime();
    }
    int completed = 0;
    while (completed < count) {
        int r = write(fd, curr_buf + completed, count - completed);
        if (errno != 0) {
            if (errno == EAGAIN || errno == EINTR) {
                errno = 0;
                continue;
            } else {
                errno = 0;
                break;
            }
        }
        completed += r;
    }
    if (NEED_TIME) {
        io_time += my_gettime();
    }
}

class BufferedWriter {
public:
    BufferedWriter(const string &file, int64 blockSize_) {
        fd = open(file.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
        blockSize = blockSize_ / sizeof(int64);
        ints.reserve(blockSize);
    }

    ~BufferedWriter() {
        if (fd >= 0) {
            Destroy();
        }
    }

    void Sync() {
        Flush();
        fdatasync(fd);
    }

    void Destroy() {
        Sync();
        close(fd);
        fd = -1;
    }

    void Write(int64 a) {
        if (ints.size() == blockSize) {
            Flush();
        }
        ints.push_back(a);
    }

    BufferedWriter(const BufferedWriter &) = delete;
    BufferedWriter(BufferedWriter &&) = delete;
    BufferedWriter& operator = (const BufferedWriter&) = delete;
    BufferedWriter& operator = (BufferedWriter &&) = delete;

private:
    void Flush() {
        if (!ints.empty()) {
            my_write(fd, (char *) &ints[0], ints.size() * sizeof(int64));
            ints.clear();
        }
    }

    vector<int64> ints;
    int64 blockSize;
    int fd;
};

class BufferedReader {
public:
    BufferedReader(const string &file, int64 blockSize_) {
        fd = open(file.c_str(), O_RDONLY);
        ints.resize(blockSize_ / sizeof(int64));
        next = ints.size();
        finished = false;
    }

    ~BufferedReader() {
        if (fd >= 0) {
            close(fd);
        }
    }

    void Close() {
        close(fd);
        fd = -1;
    }

    optional<int64> Next() {
        if (next >= ints.size() && !finished) {
            ReadNew();
        }
        if (next >= ints.size()) {
            return optional<int64>();
        }
        return ints[next++];
    }

    BufferedReader(const BufferedReader &) = delete;
    BufferedReader(BufferedReader &&other) noexcept {
        ints = other.ints;
        next = other.next;
        fd = other.fd;
        finished = other.finished;
        other.fd = -1;
    }

    BufferedReader& operator = (const BufferedReader &) = delete;
    BufferedReader& operator = (BufferedReader &&) = delete;

    auto IsFinished() const {
        return finished;
    }


private:
    void ReadNew() {
        next = 0;
        int last_read = my_read(fd, (char *)&ints[0], ints.size() * sizeof(int64));
        if (last_read < ints.size() * sizeof(int64)) {
            finished = true;
            ints.resize(last_read / sizeof(int64));
        }
    }

    vector<int64> ints;
    int next;
    int fd;
    bool finished;
};