BINARY			:=		archive

SRCS			:=		Test.cc				\
					Pack.cc				\
					Referee.cc			\
					Generator.cc			

OBJS			:=		$(SRCS:.cc=.o)

CXXFLAGS		:=		-I$$HOME/local/include		\
					-I.				\
					-L$$HOME/local/lib		

LIBRARIES		:=		-lelle				\
					-lssl

RM			:=		rm -f

main:			$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(BINARY) $(OBJS) $(LIBRARIES)

clean:
	$(RM) $(BINARY)
	$(RM) $(OBJS)
	$(RM) *~
