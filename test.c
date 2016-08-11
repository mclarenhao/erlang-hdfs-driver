#include <hdfs.h>
#include <stdio.h>


int main(int argc,char **argv) {
    hdfsFS fs = hdfsConnect("+120.25.252.229", 9000);
    return 0;
}
