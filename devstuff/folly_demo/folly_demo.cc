#include <gtest/gtest.h>

#include <folly/Conv.h>
#include <folly/ApplyTuple.h>
#include <folly/Benchmark.h>
#include <folly/Checksum.h>
#include <folly/ConcurrentSkipList.h>
#include <folly/CpuId.h>
#include <folly/FBString.h>
#include <folly/FBVector.h>
#include <folly/Unicode.h>
#include <folly/IPAddress.h>
#include <folly/dynamic.h>
#include <folly/MPMCQueue.h>
#include <folly/experimental/io/FsUtil.h>

#include <folly/futures/Future.h>

#include <iostream>
#include <string>

using namespace std;

TEST(folly, conv) {
    cout << folly::to<string>(1.987) << endl;
}

TEST(folly, fs) {
    cout << folly::fs::path("aloha/t.txt").filename() << endl;
    cout << folly::fs::path("aloha/t.txt").extension() << endl;
    cout << folly::fs::current_path().parent_path() << endl;
}

TEST(folly, apply_tuple) {
    cout << folly::applyTuple(std::plus<int>(), std::make_tuple(12, 122)) << endl;
}

TEST(folly, utf8) {
    cout << folly::codePointToUtf8(128123).toStdString() << endl;
}

TEST(folly, skiplist) {
    auto skiplist = folly::ConcurrentSkipList<int>::createInstance();
    folly::ConcurrentSkipList<int>::Accessor accessor(skiplist);
    accessor.insert(23);
    for (auto& e : accessor) {
        cout << e << endl;
    }
}

TEST(folly, cpuid) {
    folly::CpuId cpu;
    cout << cpu.avx512vbmi() << endl;
}

TEST(folly, dynamic) {
    folly::dynamic twelve = 12;
    cout << twelve << endl;
    folly::dynamic d = folly::dynamic::object;
    d["key"] = 12;
    d["something_else"] = { 1, 2, 3, nullptr };
    cout << d << endl;
}

TEST(folly, fbvec) {
    folly::fbvector<int> vec;
    vec.push_back(1);
    vec.push_back(4);
    vec.push_back(2);
}

TEST(folly, ipaddr) {
    folly::IPAddress v4addr("192.0.2.129");
    cout << v4addr << endl;
}

TEST(folly, queue) {
    folly::MPMCQueue<int> q(123);
    cout << q.sizeGuess() << endl;
    q.blockingWrite(123);
    cout << q.sizeGuess() << endl;
}

TEST(folly, future) {
    folly::Future<int> fut(234);
    cout << fut.isReady() << endl;
    cout << fut.value() << endl;

    using folly::Future;
    Future<std::string> fut2 = fut.then([](const int& v) -> std::string {
        cout << "fut done, fut2 ready, v = " << v << endl;
        return "value for fut2";
    });

    cout << fut2.value() << endl;

    Future<double> fut3 = fut2.then([] (const std::string& x) -> double {
        cout << "x = " << x << endl;
        return 1.987;
    });

    cout << folly::to<std::string>(fut3.value()) << endl;
}

TEST(folly, crc) {
    int x = 1987;
    cout << folly::crc32c(reinterpret_cast<const uint8_t*>(&x), sizeof(x)) << endl;
}

BENCHMARK(folly_future, iter) {
    folly::Future<int> fut(234);
    for (int i = 0; i < iter; i++) {
        fut.isReady();
        fut = fut.then([] (const int v) { return v + 1; });
    }
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    folly::runBenchmarks();
    return RUN_ALL_TESTS();
}
