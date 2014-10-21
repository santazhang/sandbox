#include <fstream>
#include <string>

#include <sys/time.h>
#include <unistd.h>

#include "picojson.h"
#include "yajl/yajl_tree.h"
#include "rapidjson/document.h"

using namespace std;

int main() {
    ifstream fin("superbig.json");
    string superbig_json((std::istreambuf_iterator<char>(fin)), (std::istreambuf_iterator<char>()));

    {
        // picojson
        printf("*** picojson ***\n");
        int n_rounds = 3;
        struct timeval start, finish;
        gettimeofday(&start, nullptr);
        for (int i = 0; i < n_rounds; i++) {
            picojson::value v;
            string err;
            printf("round %d\n", i);
            picojson::parse(v, superbig_json.begin(), superbig_json.end(), &err);
            if (!err.empty()) {
                cout << err << endl;
            }
        }
        gettimeofday(&finish, nullptr);
        double elapsed = finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec) / 1000.0 / 1000.0;
        printf("per round: %lf sec\n", elapsed / n_rounds);
        printf("---\n\n");
    }

    {
        // yajl
        printf("*** yajl ***\n");
        int n_rounds = 3;
        struct timeval start, finish;
        gettimeofday(&start, nullptr);
        for (int i = 0; i < n_rounds; i++) {
            printf("round %d\n", i);
            yajl_val node;
            char errbuf[1024];
            node = yajl_tree_parse((const char *) superbig_json.data(), errbuf, sizeof(errbuf));
            if (node == NULL) {
                fprintf(stderr, "parse_error: ");
                if (strlen(errbuf)) fprintf(stderr, " %s", errbuf);
                else fprintf(stderr, "unknown error");
                fprintf(stderr, "\n");
            }
            yajl_tree_free(node);
        }
        gettimeofday(&finish, nullptr);
        double elapsed = finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec) / 1000.0 / 1000.0;
        printf("per round: %lf sec\n", elapsed / n_rounds);
        printf("---\n\n");
    }
    
    {
        // rapidjson
        printf("*** rapidjson ***\n");
        int n_rounds = 3;
        struct timeval start, finish;
        gettimeofday(&start, nullptr);
        for (int i = 0; i < n_rounds; i++) {
            printf("round %d\n", i);
            rapidjson::Document d;
            d.Parse(superbig_json.data());
        }
        gettimeofday(&finish, nullptr);
        double elapsed = finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec) / 1000.0 / 1000.0;
        printf("per round: %lf sec\n", elapsed / n_rounds);
        printf("---\n\n");
    }

    return 0;
}
