
CXX = c++

CXXFLAGS = -g 
#-Wall -Wextra -Werror -std=c++98 

CPPFILES = parcere.cpp parce_http_rq.cpp client.cpp implRegEX.cpp   \
           RegEx_utils.cpp RegEx.cpp server.cpp
OFILES = ${CPPFILES:.cpp=.o}

EXECUTABLE = Server

all : $(EXECUTABLE)

$(EXECUTABLE) : $(OFILES)
				$(CXX) $(OFILES)  -o $(EXECUTABLE) $(CXXFLAGS)

clean :
			rm -rf  ${OFILES}

fclean : clean
			rm  -rf ${EXECUTABLE}
	
re: fclean all