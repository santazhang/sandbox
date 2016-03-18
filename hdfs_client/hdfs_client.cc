#include <stdio.h>

#include <hdfs/hdfs.h>

int main() {
    hdfsBuilder* builder = hdfsNewBuilder();
    printf("ALOHA!\n");
    hdfsBuilderSetNameNode(builder, "beaker-25.news.cs.nyu.edu");
    hdfsBuilderSetNameNodePort(builder, 56789);
    hdfsBuilderSetUserName(builder, "santa");
    hdfsFS hdfs = hdfsBuilderConnect(builder);

    int num_entries;
    hdfsFileInfo* file_info = hdfsListDirectory(hdfs, "/", &num_entries);
    printf("ls / : got %d entries:\n", num_entries);
    for (int i = 0; i < num_entries; i++) {
        printf("ty=%c name=%s sz=%ld owner=%s\n",
               file_info[i].mKind == kObjectKindFile ? 'F' : 'D',
               file_info[i].mName, file_info[i].mSize, file_info[i].mOwner);
    }
    hdfsFreeFileInfo(file_info, num_entries);

    hdfsCreateDirectory(hdfs, "/aloha_demo");

    // read and write file
    hdfsFile fout = hdfsOpenFile(hdfs, "/hello.txt", O_WRONLY, 0, 0, 0);
    hdfsWrite(hdfs, fout, "123", 3);
    hdfsCloseFile(hdfs, fout);

    hdfsFile fin = hdfsOpenFile(hdfs, "/hello.txt", O_RDONLY, 0, 0, 0);
    char buf[3];
    hdfsRead(hdfs, fin, buf, 3);
    printf("read: %c %c %c\n", buf[0], buf[1], buf[2]);
    hdfsCloseFile(hdfs, fin);

    hdfsDisconnect(hdfs);
    hdfsFreeBuilder(builder);
    return 0;
}
