CXXFLAGS		:=		-I$$HOME/local/include		\
					-L$$HOME/local/lib		
RM			:=		rm -f


all: sdht

sdht:
	$(CXX) $(CXXFLAGS) -o test-sdht test-sdht.cc  -g -lhole -lelle -lviewer -lssl -lpthread -lubigraphclient  -lxmlrpc_client -lxmlrpc -lxmlrpc_util -lxmlrpc_xmlparse -lxmlrpc_xmltok -lboost_serialization -lboost_system -lboost_thread

clean:
	$(RM) test-sdht
	$(RM) *~
