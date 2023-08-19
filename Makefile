all: build

build:
	g++ main.cpp -O2 -march=native -std=c++17 -o main

test2GB:
	time ./main big_tests/test_250000000_1.in answer.out tmp 26214400 262144
	time ./main big_tests/test_250000000_1.in answer.out tmp 52428800 262144
	time ./main big_tests/test_250000000_1.in answer.out tmp 104857600 262144
	time ./main big_tests/test_250000000_1.in answer.out tmp 419430400 262144
	time ./main big_tests/test_250000000_1.in answer.out tmp 838860800 262144
	time ./main big_tests/test_250000000_1.in answer.out tmp 1073741824 262144
	time ./main big_tests/test_250000000_1.in answer.out tmp 2147483648 262144
	time ./main big_tests/test_250000000_1.in answer.out tmp 4294967296 262144

	time ./stxxl/main big_tests/test_250000000_1.in answer.out  26214400
	time ./stxxl/main big_tests/test_250000000_1.in answer.out  52428800
	time ./stxxl/main big_tests/test_250000000_1.in answer.out  104857600
	time ./stxxl/main big_tests/test_250000000_1.in answer.out  419430400
	time ./stxxl/main big_tests/test_250000000_1.in answer.out  838860800
	time ./stxxl/main big_tests/test_250000000_1.in answer.out  1073741824
	time ./stxxl/main big_tests/test_250000000_1.in answer.out  2147483648
	time ./stxxl/main big_tests/test_250000000_1.in answer.out  4294967296

test_14GB:
	time ./main big_tests/test_14_GB.in answer.out tmp 26214400 262144
	time ./main big_tests/test_14_GB.in answer.out tmp 52428800 262144
	time ./main big_tests/test_14_GB.in answer.out tmp 104857600 262144
	time ./main big_tests/test_14_GB.in answer.out tmp 419430400 262144
	time ./main big_tests/test_14_GB.in answer.out tmp 838860800 262144
	time ./main big_tests/test_14_GB.in answer.out tmp 1073741824 262144
	time ./main big_tests/test_14_GB.in answer.out tmp 2147483648 262144
	time ./main big_tests/test_14_GB.in answer.out tmp 4294967296 262144

	time ./stxxl/main big_tests/test_14_GB.in answer.out  26214400
	time ./stxxl/main big_tests/test_14_GB.in answer.out  52428800
	time ./stxxl/main big_tests/test_14_GB.in answer.out  104857600
	time ./stxxl/main big_tests/test_14_GB.in answer.out  419430400
	time ./stxxl/main big_tests/test_14_GB.in answer.out  838860800
	time ./stxxl/main big_tests/test_14_GB.in answer.out  1073741824
	time ./stxxl/main big_tests/test_14_GB.in answer.out  2147483648
	time ./stxxl/main big_tests/test_14_GB.in answer.out  4294967296