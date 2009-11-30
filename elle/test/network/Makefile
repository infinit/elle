BINARY			:=		network

SRCS			:=		Test.cc				

OBJS			:=		$(SRCS:.cc=.o)

CXXFLAGS		:=		-I.				

LIBRARIES		:=		-lelle				

RM			:=		rm -f

main:			$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(BINARY) $(OBJS) $(LIBRARIES)

clean:
	$(RM) $(BINARY)
	$(RM) $(OBJS)
	$(RM) *~
