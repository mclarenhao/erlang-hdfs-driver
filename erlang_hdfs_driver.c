/*
 *  Erlang HDFS Driver
 *  @author liuhao0927@163.com
 */

#include <erl_nif.h>
#include <stdio.h>
#include <hdfs.h>
#include <stdlib.h>
#include <string.h>


#define LOG(msg)  printf("[erlang_hdfs_driver INFO] (%s:%d) - %s \r\n",__FILE__,__LINE__,msg)

ErlNifResourceType * HDFS_TYPE;
ERL_NIF_TERM atom_ok;


/**
 * 实现一个简单的List
 */
typedef struct tagList{
  char* store;
  int size;
  int capacity;
  int (*add)(struct tagList*,char*,int length);
  char* (*get)(struct tagList*,int);
} List;

/**
 * 向集合内添加元素
 */
int list_add(List* this,char *item,int length) {
     int increment = this->size + length;
     int *new = malloc(increment * sizeof(int));
     if(this->store != NULL) {
        int k=0;
        for(k=0;k< this->size; k++) {
          new[k] = this->store[k];
        }
     }
     
     int i=0;

     

     printf("increment is ::%d, and size is %d\n",increment,this->size);

     for(i=0; i < length; i++) {
        new[this->size + i]  = item[i];
     }

     printf("++++++%s\n",new);
     if(this->store != NULL) {
        free(this->store);
     }
     this->store = new;
     this->size = increment;
     this->capacity = this->size;
}

/**
 * 从集合内获取元素
 */
char* list_get(List *this,int idx) {
   if(idx < 0 || idx >= this-> capacity) {
       LOG("ArrayOutOfBoundException");
       return NULL;
   }

   return this -> store[idx];
}


int destroy_list(List *list) {
   if(list == NULL) {
      return -1;
   }

   if(list->store != NULL)
      free(list -> store);
   free(list);
}

/**
 * 创建新集合
 */
List *new_list() {
  List *this = malloc(sizeof(List));
  this-> size = 0;
  this -> capacity = 0;
  this -> store = NULL;
  this -> get = list_get;
  this -> add = list_add;
  return this;
}


typedef struct {
  hdfsFS fs;
} HDFS;


/**
 * 释放HDFS链接
 */
void freeHdfs(ErlNifEnv* env, void* ref) {
   LOG("释放资源");
   HDFS *hdfs = (HDFS*) ref;
   hdfsDisconnect(hdfs -> fs);
}

const ERL_NIF_TERM mkstring(ErlNifEnv *env,char *format,void *value) {
   const char string[255];
   sprintf(string,format,value);
   return enif_make_string(env,string,ERL_NIF_LATIN1);
}


/*
 * 加载时执行
 */
int load(ErlNifEnv *env, void **priv_data, ERL_NIF_TERM load_info) {
   LOG("加载erlang_hdfs_driver native库");
   int flags = ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER;
  
   HDFS_TYPE = enif_open_resource_type(env, NULL, "hdfs", freeHdfs, flags, NULL);
   if(HDFS_TYPE == NULL) return -1;


   atom_ok = enif_make_atom(env,"ok");

   return 0;
}

/*
 * 返回ERLANG HDFS驱动程序的元数据信息
 */
static ERL_NIF_TERM meta_info(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    return enif_make_string(env,"Erlang HDFS Driver @powerd by mclaren 2016",ERL_NIF_LATIN1);
}

/**
 * 打开到HDFS的链接并返回操作句柄
 */
static ERL_NIF_TERM connect(ErlNifEnv *env,int argc,const ERL_NIF_TERM argv[]) {

  char *host = malloc(sizeof(char) * 256);
  int port;   
  enif_get_string(env,argv[0],host,256,ERL_NIF_LATIN1);
  enif_get_int(env,argv[1],&port);
 
  char message[255];
  sprintf(message,"connect to is %s:%d",host,port);
 
  LOG(message);   
  hdfsFS fs = hdfsConnect(host,port); 
  free(host);

  HDFS *hdfs = enif_alloc_resource(HDFS_TYPE, sizeof(HDFS));
  hdfs -> fs = fs;
  if(hdfs == NULL) return enif_make_badarg(env);

  ERL_NIF_TERM ret = enif_make_resource(env, hdfs);
  enif_release_resource(hdfs);

  return ret;
}

/**
 * 列举指定目录下的所有文件
 */
static ERL_NIF_TERM list_files(ErlNifEnv *env,int argc,const ERL_NIF_TERM argv[]) {
    HDFS *hdfs;
    if(!enif_get_resource(env, argv[0], HDFS_TYPE, (void**) &hdfs)) {
        return enif_make_badarg(env);
    }

    

    char *path = malloc(256);
    enif_get_string(env,argv[1],path,256,ERL_NIF_LATIN1);
    int number;

    enif_get_int(env,argv[2],&number);


    hdfsFileInfo *subFiles = hdfsListDirectory(hdfs->fs,path,&number);
    free(path);

    /**
     * 列举出目标文件夹下的所有文件
     */
    char buf[255];
    int i=0;
  
    ERL_NIF_TERM Returns[number];

    for(i=0; i< number; i++) {
        memset(buf,0,255);
        hdfsFileInfo subFile = subFiles[i];

      
        ERL_NIF_TERM terms[] = {
             enif_make_tuple2(env,enif_make_string(env,"name",ERL_NIF_LATIN1),mkstring(env,"%s",subFile.mName)),
             enif_make_tuple2(env,enif_make_string(env,"lastMod",ERL_NIF_LATIN1),mkstring(env,"%d",subFile.mLastMod)),
             enif_make_tuple2(env,enif_make_string(env,"size",ERL_NIF_LATIN1),mkstring(env,"%d",subFile.mSize)),
             enif_make_tuple2(env,enif_make_string(env,"replication",ERL_NIF_LATIN1),mkstring(env,"%d",subFile.mReplication)),
             enif_make_tuple2(env,enif_make_string(env,"blockSize",ERL_NIF_LATIN1),mkstring(env,"%d",subFile.mBlockSize)),
             enif_make_tuple2(env,enif_make_string(env,"owner",ERL_NIF_LATIN1),mkstring(env,"%s",subFile.mOwner)),
             enif_make_tuple2(env,enif_make_string(env,"permissions",ERL_NIF_LATIN1),mkstring(env,"%d",subFile.mPermissions)),
             enif_make_tuple2(env,enif_make_string(env,"group",ERL_NIF_LATIN1),mkstring(env,"%s",subFile.mGroup)),
             enif_make_tuple2(env,enif_make_string(env,"lastAccess",ERL_NIF_LATIN1),mkstring(env,"%d",subFile.mLastAccess))
        };

        ERL_NIF_TERM element = enif_make_tuple_from_array(env,terms,9);
        Returns[i] = element;
    }

  
    return enif_make_tuple2(env,atom_ok,enif_make_list_from_array(env,Returns,number));
}


/**
 * 读取HDFS上的文件
 * 参数1: HDFS句柄
 * 参数2: 文件路径
 */
static ERL_NIF_TERM read_file(ErlNifEnv *env,int argc,const ERL_NIF_TERM argv[]) {
    HDFS *hdfs;
    if(!enif_get_resource(env, argv[0], HDFS_TYPE, (void**) &hdfs)) {
        return enif_make_badarg(env);
    }

    char *path = malloc(256);
    enif_get_string(env,argv[1],path,256,ERL_NIF_LATIN1);
    hdfsFile file = hdfsOpenFile(hdfs->fs, path, O_RDONLY, 0, 0, 0);

    if(file == NULL) {
       return enif_make_badarg(env);
    }

    hdfsFileInfo *info = hdfsGetPathInfo(hdfs->fs,path);
    free(path);
    if(info == NULL) {
      return enif_make_badarg(env);
    }
    int size = info->mSize;

    char *buf = malloc(size * sizeof(char));
    hdfsRead(hdfs->fs,file,buf,size);
  

    ErlNifBinary output_binary;
    enif_alloc_binary(size,&output_binary);
    strcpy(output_binary.data, buf);
    free(buf);
    hdfsCloseFile(hdfs->fs,file);
    return enif_make_tuple2(env,atom_ok,enif_make_binary(env, &output_binary));
}

/**
 * 返回API函数映射表
 */
static ErlNifFunc nif_funcs[] = {
   {"meta_info",0,meta_info},
   {"connect",2,connect},
   {"list_files",3,list_files},
   {"read_file",2,read_file}
};


ERL_NIF_INIT(erlang_hdfs_driver,nif_funcs,&load,NULL,NULL,NULL) 

