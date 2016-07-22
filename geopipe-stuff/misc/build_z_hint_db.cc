#include <stdio.h>
#include <inttypes.h>

#include <glog/logging.h>

#include "caffe/proto/caffe.pb.h"
#include "caffe/util/db.hpp"
#include "caffe/util/format.hpp"
#include "utils/inc_gflags.h"

#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>

using std::unique_ptr;
using namespace caffe;

struct tile_info_t {
    std::string fpath;

    int x;
    int y;
    int w;
    int h;

    int label;
};

struct cached_open_file_t {
    FILE* fp;
    int32_t img_width;
    int32_t img_height;
};

int main(int argc, char* argv[]) {
    ::google::InitGoogleLogging(argv[0]);
    // Print output to stderr (while still logging)
    FLAGS_alsologtostderr = 1;

    gflags::SetUsageMessage("Convert las z hint to the lmdb format used as input for Caffe.\n"
          "Usage:\n"
          "    build_z_hint_db [FLAGS] ROOTFOLDER/ LISTFILE DB_NAME\n");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    if (argc < 4) {
        gflags::ShowUsageWithFlags(argv[0]);
        return 1;
    }

    std::ifstream infile(argv[2]);
    std::vector<tile_info_t> lines;
    std::string line;
    while (std::getline(infile, line)) {
        tile_info_t info;
        size_t pos = line.find_last_of(' ');
        info.label = atoi(line.substr(pos + 1).c_str());
        line.resize(pos);

        pos = line.find_last_of(' ');
        info.h = atoi(line.substr(pos + 1).c_str());
        line.resize(pos);

        pos = line.find_last_of(' ');
        info.w = atoi(line.substr(pos + 1).c_str());
        line.resize(pos);
        
        pos = line.find_last_of(' ');
        info.y = atoi(line.substr(pos + 1).c_str());
        line.resize(pos);

        pos = line.find_last_of(' ');
        info.x = atoi(line.substr(pos + 1).c_str());
        line.resize(pos);

        info.fpath = line;

        lines.push_back(info);
    }

    std::string root_folder(argv[1]);
    Datum datum;
    int count = 0;

    unique_ptr<db::DB> db(db::GetDB("lmdb"));
    db->Open(argv[3], db::NEW);
    unique_ptr<db::Transaction> txn(db->NewTransaction());
    std::map<string, cached_open_file_t*> open_files;
    const size_t max_open_files = 500;

    for (size_t line_id = 0; line_id < lines.size(); ++line_id) {
        const tile_info_t& info = lines[line_id];
        string full_fpath = root_folder + "/" + info.fpath;

        cached_open_file_t* file_handle = nullptr;
        auto it_file = open_files.find(full_fpath);
        if (it_file == open_files.end()) {
            // open file
            if (open_files.size() > max_open_files) {
                // close all open files
                for (auto& it : open_files) {
                    ::fclose(it.second->fp);
                    delete it.second;
                }
                open_files.clear();
            }
            file_handle = new cached_open_file_t;
            file_handle->fp = ::fopen(full_fpath.c_str(), "rb");
            CHECK(file_handle->fp != NULL);
            ::fread(&file_handle->img_width, sizeof(int32_t), 1, file_handle->fp);
            ::fread(&file_handle->img_height, sizeof(int32_t), 1, file_handle->fp);
            open_files[full_fpath] = file_handle;
        } else {
            // reuse cached file
            file_handle = open_files[full_fpath];
        }

        datum.set_channels(1);
        datum.set_height(info.h);
        datum.set_width(info.w);
        datum.clear_data();
        datum.clear_float_data();

        // load data into DATUM
        google::protobuf::RepeatedField<float>* datumFloatData = datum.mutable_float_data();
        for (int y = info.y; y < info.y + info.h; y++) {
            ::fseek(file_handle->fp, 2*sizeof(int32_t) + sizeof(float) * (y*file_handle->img_width + info.x), SEEK_SET);
            for (int x = info.x; x < info.x + info.w; x++) {
                float f_val;
                CHECK(::fread(&f_val, sizeof(float), 1, file_handle->fp) != 0);
                datumFloatData->Add(f_val);
            }
        }

        datum.set_encoded(false);  // we are storing raw data
        datum.set_label(info.label);

        // sequential
        string key_str = caffe::format_int(line_id, 8) + "_" + full_fpath;
        std::ostringstream oss;
        oss << "@" << info.x << "," << info.y << "," << info.w << "," << info.h;
        key_str += oss.str();

        // Put in db
        string out;
        CHECK(datum.SerializeToString(&out));
        txn->Put(key_str, out);

        if (++count % 1000 == 0) {
            // Commit db
            txn->Commit();
            txn.reset(db->NewTransaction());
            LOG(INFO) << "Processed " << count << " tiles.";
        }
    }
    // write the last tile
    if (count % 1000 != 0) {
        txn->Commit();
        LOG(INFO) << "Processed " << count << " tiles.";
    }
    
    // close all open files
    for (auto& it : open_files) {
        ::fclose(it.second->fp);
        delete it.second;
    }
    open_files.clear();

    return 0;
}
