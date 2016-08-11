%
% 集成测试
%
-module(test).

-export([start/0]).

start() ->
   Con = erlang_hdfs_driver:connect("master",9000),
   io:format("Connection is ~p~n",[Con]),
   R  =  erlang_hdfs_driver:list_files(Con,"/",10),
   io:format("Returns is ~p~n",[R]),


   {ok,Content} = erlang_hdfs_driver:read_file(Con,"/README"),
   
   Re = io_lib:format("Returns is ~s~n",[binary_to_list(Content)]),
   io:format("~p",[Re]).
