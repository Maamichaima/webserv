
CXX = c++

CXXFLAGS = -g 
#-Wall -Wextra -Werror -std=c++98 

CPPFILES =  server.cpp Socket.cpp

OFILES = ${CPPFILES:.cpp=.o}

EXECUTABLE = Socket

all : $(EXECUTABLE)

$(EXECUTABLE) : $(OFILES)
				$(CXX) $(OFILES)  -o $(EXECUTABLE) $(CXXFLAGS)

clean :
			rm -rf  ${OFILES}

fclean : clean
			rm  -rf ${EXECUTABLE}
	
re: fclean all