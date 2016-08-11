-module(erlang_hdfs_driver).

-export([init/0,meta_info/0]).
-export([connect/2]).
-export([list_files/3]).
-export([read_file/2]).

init() ->
   erlang:load_nif("./erlang_hdfs_driver",0).


meta_info() ->
    "NIF erlang_hdfs_driver not loaded".


connect(_Host,_Port) ->
    "NIF erlang_hdfs_driver not loaded".

list_files(_Conn,_Path,_Number) ->
    "NIF erlang_hdfs_driver not loaded".

read_file(_Conn,_Path) ->
    "NIF erlang_hdfs_driver not loaded".
