#include <folly/Conv.h>
#include <folly/ApplyTuple.h>
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

#include <iostream>
#include <string>

using namespace std;

int main() {
    cout << folly::to<string>(1.987) << endl;
    cout << folly::codePointToUtf8(128123).toStdString() << endl;
    cout << folly::fs::path("aloha/t.txt").filename() << endl;
    cout << folly::fs::path("aloha/t.txt").extension() << endl;
    cout << folly::fs::current_path().parent_path() << endl;
    cout << folly::applyTuple(std::plus<int>(), std::make_tuple(12, 122)) << endl;
    int x = 1987;
    cout << folly::crc32c(reinterpret_cast<const uint8_t*>(&x), sizeof(x)) << endl;

    {
        auto skiplist = folly::ConcurrentSkipList<int>::createInstance();
        folly::ConcurrentSkipList<int>::Accessor accessor(skiplist);
        accessor.insert(23);
        for (auto& e : accessor) {
            cout << e << endl;
        }
    }

    {
        folly::CpuId cpu;
        cout << cpu.avx512vbmi() << endl;
    }

    {
        folly::dynamic twelve = 12;
        cout << twelve << endl;
        folly::dynamic d = folly::dynamic::object;
        d["key"] = 12;
        d["something_else"] = { 1, 2, 3, nullptr };
        cout << d << endl;
    }

    {
        folly::fbvector<int> vec;
        vec.push_back(1);
        vec.push_back(4);
        vec.push_back(2);
    }

    {
        folly::IPAddress v4addr("192.0.2.129");
        cout << v4addr << endl;
    }

    {
        folly::MPMCQueue<int> q(123);
        cout << q.sizeGuess() << endl;
        q.blockingWrite(123);
        cout << q.sizeGuess() << endl;
    }

    return 0;
}
