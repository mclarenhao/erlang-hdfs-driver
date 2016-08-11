#include <hdfs.h>
#include <stdio.h>


int main(int argc,char **argv) {
    hdfsFS fs = hdfsConnect("master", 9000);
    return 0;
}
