#include <stxxl/io>
#include <stxxl/mng>
#include <stxxl/sort>
#include <stxxl/vector>
#include <stxxl/stream>
#include <unistd.h>

#include <omp.h>

#include <sstream>

typedef unsigned long long int64;

struct MyInt
{
    int64 key_;

    int64 key() const
    {
        return key_;
    }

    MyInt() { }
    MyInt(int64 k): key_(k) { }

    static int64 min_value()
    {
        return std::numeric_limits<int64>::min();
    }
    static int64 max_value()
    {
        return std::numeric_limits<int64>::max();
    }
};

inline bool operator < (const MyInt& lhs, const MyInt& rhs)
{
    return lhs.key() < rhs.key();
}

inline bool operator == (const MyInt& lhs, const MyInt& rhs)
{
    return lhs.key() == rhs.key();
}

struct Cmp
{
    typedef MyInt first_argument_type;
    typedef MyInt second_argument_type;
    typedef bool result_type;
    bool operator () (const MyInt& lhs, const MyInt& rhs) const
    {
        return lhs < rhs;
    }
    static MyInt min_value()
    {
        return MyInt::min_value();
    }
    static MyInt max_value()
    {
        return MyInt::max_value();
    }
};

std::ostream& operator << (std::ostream& o, const MyInt& obj)
{
    o << obj.key();
    return o;
}

int main(int argc, char** argv)
{

    close(1);
    close(2); // so that it won`t write anything

    if (argc != 4)
    {
        std::cout << "Usage: " << argv[0] << " <in> <out> <memory in bytes>" << std::endl;
        return 1;
    }

    // Disable cpu parallelism.
    omp_set_num_threads(1);

    stxxl::syscall_file fin(argv[1], stxxl::file::DIRECT | stxxl::file::RDONLY);
    stxxl::syscall_file out(argv[2], stxxl::file::DIRECT | stxxl::file::RDWR | stxxl::file::CREAT);
    long long memory_to_use = strtol(argv[3], NULL, 10);
    std::cerr << "memory_to_use " << memory_to_use << std::endl;

    // Block size is hardcoded for SSD.
    const stxxl::unsigned_type block_size = 256 * 1024;
    typedef stxxl::vector<MyInt, 1, stxxl::lru_pager<8>, block_size> vector_type;

    vector_type input(&fin);
    vector_type output(&out);
    output.resize(input.size());

    typedef stxxl::stream::streamify_traits<vector_type::iterator>::stream_type input_stream_type;
    input_stream_type input_stream = stxxl::stream::streamify(input.begin(), input.end());

    // WRITE_CODE_HERE: call stxxl sorting.
    typedef Cmp comparator_type;
    typedef stxxl::stream::sort<input_stream_type, comparator_type, block_size> sort_stream_type;
    sort_stream_type sort_stream(input_stream, comparator_type(), memory_to_use);
    vector_type::iterator o = stxxl::stream::materialize(sort_stream, output.begin(), output.end());
    STXXL_ASSERT(o == output.end());

    STXXL_MSG((stxxl::is_sorted(output.begin(), output.end(), comparator_type()) ? "OK" : "WRONG"));

    return 0;
}

