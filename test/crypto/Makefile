BINARY			:=		crypto

SRCS			:=		Test.cc				

OBJS			:=		$(SRCS:.cc=.o)

CXXFLAGS		:=		-I$$HOME/local/include		\
					-I$$HOME/local-ssldev/include	\
					-I.				\
					-L$$HOME/local/lib		\
					-L$$HOME/local-ssldev/lib	

LIBRARIES		:=		-lelle				\
					-lcrypto			\
					-lssl				\
					-ldl				

RM			:=		rm -f

main:			$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(BINARY) $(OBJS) $(LIBRARIES)

clean:
	$(RM) $(BINARY)
	$(RM) $(OBJS)
	$(RM) *~
