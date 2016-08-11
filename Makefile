HADOOP_HOME=/root/hadoop-2.6.4
ERLANG_HOME=/usr/local/erlang
cc=gcc

out:erlang_hdfs_driver.erl erlang_hdfs_driver.so
	erlc -o bin $< *.erl
erlang_hdfs_driver.so:erlang_hdfs_driver.c
	$(cc) -shared -L $(HADOOP_HOME)/lib/native  -I $(HADOOP_HOME)/include -I $(ERLANG_HOME)/lib/erlang/usr/include -fPIC -o bin/$@ $< -lhdfs
test:test.c
	$(cc) -L $(HADOOP_HOME)/lib/native  -I $(HADOOP_HOME)/include -I $(ERLANG_HOME)/lib/erlang/usr/include -fPIC -o bin/$@ $< -lhdfs
ut:test
	@echo '开始集成测试'
	@cp bin/*.so .
	@erl -noshell -pa bin -s erlang_hdfs_driver init -s test start -s init stop
	@rm *.so
clean:
	@rm -f bin/*
