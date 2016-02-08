#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <getopt.h>

#include <thread>
#include <vector>
#include <atomic>
#include <iostream>

#include <grpc++/grpc++.h>

#include "bench.pb.h"
#include "bench.grpc.pb.h"

using namespace std;
using namespace grpc;


const char *svr_addr = "127.0.0.1:6789";
int byte_size = 1;
int seconds = 10;
int outgoing_requests = 1000;
int client_threads = 8;
int worker_threads = 16;
std::atomic<int> done_count {0};


class Timer {

    struct timeval begin_;
    struct timeval end_;

public:
    Timer() { reset(); }
    inline void start() {
        reset();
        gettimeofday(&begin_, nullptr);
    }
    inline void stop() {
        gettimeofday(&end_, nullptr);
    }
    inline void reset() {
        begin_.tv_sec = 0;
        begin_.tv_usec = 0;
        end_.tv_sec = 0;
        end_.tv_usec = 0;
    }
    double elapsed() const {
        if (end_.tv_sec == 0 && end_.tv_usec == 0) {
            // not stopped yet
            struct timeval now;
            gettimeofday(&now, nullptr);
            return now.tv_sec - begin_.tv_sec +
                   (now.tv_usec - begin_.tv_usec) / 1000000.0;
        }
        return end_.tv_sec - begin_.tv_sec +
               (end_.tv_usec - begin_.tv_usec) / 1000000.0;
    }
};


class BenchServiceImpl final : public Bench::Service {
    Status add(ServerContext* context, const Request* request, Response* reply) override {
        int s = 0;
        for (const auto& v : request->v()) {
            s += v;
        }
        reply->set_v(s);
        return Status::OK;
    }
};

void run_as_sync_server() {
    BenchServiceImpl service;
    ServerBuilder builder;
    builder.AddListeningPort(svr_addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    printf("server started at %s\n", svr_addr);
    server->Wait();
}

void run_as_sync_client_work() {
    printf("connecting to %s\n", svr_addr);

    auto channel = CreateChannel(svr_addr, InsecureChannelCredentials());
    auto stub = Bench::NewStub(channel);

    Request request;
    for (int j = 0; j < byte_size; j++) {
        request.add_v(1);
    }

    Timer t;
    t.start();
    while (t.elapsed() < seconds) {
        for (int i = 0; i < 10; i++) {
            ClientContext context;
            Response reply;
            Status status = stub->add(&context, request, &reply);
            done_count++;
        }
    }
}

void run_as_sync_client() {
    std::thread** threads = new std::thread*[client_threads];
    Timer t;
    t.start();
    for (int i = 0; i < client_threads; i++) {
        threads[i] = new std::thread(run_as_sync_client_work);
    }
    for (int i = 0; i < client_threads; i++) {
        threads[i]->join();
        delete threads[i];
    }
    t.stop();
    delete[] threads;
    int done = done_count.load();
    printf("  *** done count = %d, elapsed = %.2lf sec, qps = %.0f\n",
           done, t.elapsed(), done / t.elapsed());
}

void async_server_handle_rpcs(ServerCompletionQueue* cq, Bench::AsyncService* service) {
    class CallData {
    public:
        CallData(Bench::AsyncService* service, ServerCompletionQueue* cq)
                : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {
            Proceed();
        }

        void Proceed() {
            if (status_ == CREATE) {
                service_->Requestadd(&ctx_, &request_, &responder_, cq_, cq_, this);
                status_ = PROCESS;
            } else if (status_ == PROCESS) {
                new CallData(service_, cq_);

                // do work
                int s = 0;
                for (const auto& v : request_.v()) {
                    s += v;
                }
                reply_.set_v(s);

                // reply
                responder_.Finish(reply_, Status::OK, this);
                // cout << "done in thread: " << std::this_thread::get_id() << endl;

                status_ = FINISH;
            } else {
                GPR_ASSERT(status_ == FINISH);
                delete this;
            }
        }

    private:
        Bench::AsyncService* service_;
        ServerCompletionQueue* cq_;
        ServerContext ctx_;
        Request request_;
        Response reply_;
        ServerAsyncResponseWriter<Response> responder_;

        enum CallStatus { CREATE, PROCESS, FINISH };
        CallStatus status_;  // The current serving state.
    };

    new CallData(service, cq);

    for (;;) {
        void* tag;
        bool ok;
        cq->Next(&tag, &ok);
        GPR_ASSERT(ok);
        static_cast<CallData*>(tag)->Proceed();
    }
}

void run_as_async_server() {
    ServerBuilder builder;
    builder.AddListeningPort(svr_addr, grpc::InsecureServerCredentials());

    Bench::AsyncService service;
    builder.RegisterAsyncService(&service);

    vector<ServerCompletionQueue*> cqs;
    for (int i = 0; i < worker_threads; i++) {
        cqs.push_back(builder.AddCompletionQueue().release());
    }

    std::unique_ptr<Server> server(builder.BuildAndStart());

    printf("server started at %s\n", svr_addr);

    std::thread** threads = new std::thread*[worker_threads];
    for (int i = 0; i < worker_threads; i++) {
        auto* cq = cqs[i];
        threads[i] = new std::thread([cq, &service] {
            async_server_handle_rpcs(cq, &service);
        });
    }

    for (int i = 0; i < worker_threads; i++) {
        threads[i]->join();
        delete threads[i];
    }
    delete[] threads;
}

void run_as_async_client_work() {
    printf("connecting to %s\n", svr_addr);

    auto channel = CreateChannel(svr_addr, InsecureChannelCredentials());
    auto stub = Bench::NewStub(channel);

    Request request;
    for (int j = 0; j < byte_size; j++) {
        request.add_v(1);
    }

    Timer t;
    t.start();
    CompletionQueue cq;

    struct call_info {
        ClientContext* context = nullptr;
        Status* status = nullptr;
        Response* response = nullptr;
        ClientAsyncResponseReader<Response>* rpc = nullptr;

        ~call_info() {
            delete context;
            delete status;
            delete response;
            delete rpc;
        }
    };

    auto make_one_call = [&cq, &stub, request] {
        call_info* info = new call_info;
        info->context = new ClientContext;
        info->status = new Status;
        info->response = new Response;
        info->rpc = stub->Asyncadd(info->context, request, &cq).release();
        info->rpc->Finish(info->response, info->status, static_cast<void*>(info));
    };

    for (int i = 0; i < outgoing_requests; i++) {
        make_one_call();
    }

    for (;;) {
        void* got_tag;
        bool ok = false;
        cq.Next(&got_tag, &ok);
        GPR_ASSERT(ok);
        call_info* info = static_cast<call_info*>(got_tag);
        delete info;

        int done = done_count++;
        if (done % 10 == 0) {
            if (t.elapsed() > seconds) {
                break;
            }
        }
        make_one_call();
    }
}

void run_as_async_client() {
    std::thread** threads = new std::thread*[client_threads];
    Timer t;
    t.start();
    for (int i = 0; i < client_threads; i++) {
        threads[i] = new std::thread(run_as_async_client_work);
    }
    for (int i = 0; i < client_threads; i++) {
        threads[i]->join();
        delete threads[i];
    }
    t.stop();
    delete[] threads;
    int done = done_count.load();
    printf("  *** done count = %d, elapsed = %.2lf sec, qps = %.0f\n",
           done, t.elapsed(), done / t.elapsed());
}

int main(int argc, char* argv[]) {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    bool is_client = false, is_server = false, is_async = false;

    if (argc < 2) {
        printf("usage: bench OPTIONS\n");
        printf("                -a    run in async mode\n");
        printf("                -c|-s ip:port\n");
        printf("                -b    byte_size         (client only)\n");
        printf("                -n    seconds           (client only)\n");
        printf("                -o    outgoing_requests (clinet only)\n");
        printf("                -t    client_threads    (client only)\n");
        printf("                -w    worker_threads    (server only)\n");
        exit(1);
    }

    char ch = 0;
    while ((ch = getopt(argc, argv, "ac:s:b:n:o:t:w:"))!= -1) {
        switch (ch) {
        case 'a':
            is_async = true;
            break;
        case 'c':
            is_client = true;
            svr_addr = optarg;
            break;
        case 's':
            is_server = true;
            svr_addr = optarg;
            break;
        case 'b':
            byte_size = atoi(optarg);
            break;
        case 'n':
            seconds = atoi(optarg);
            break;
        case 'o':
            outgoing_requests = atoi(optarg);
            break;
        case 't':
            client_threads = atoi(optarg);
            break;
        case 'w':
            worker_threads = atoi(optarg);
            break;
        default:
            break;
        }
    }

    if (is_server) {
        if (is_async) {
            run_as_async_server();
        } else {
            run_as_sync_server();
        }
    } else if (is_client) {
        if (is_async) {
            run_as_async_client();
        } else {
            run_as_sync_client();
        }
    } else {
        printf("Must specify server/client!\n");
        exit(1);
    }

    printf("Goodbye.\n");

    return 0;
}
