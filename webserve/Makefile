
CXX = c++

CXXFLAGS = -g 
#-Wall -Wextra -Werror -std=c++98 

CPPFILES =  main.cpp Socket.cpp ServerManager.cpp ./parsing/Server.cpp ./parsing/tokenizer.cpp ./parsing/check_syntaxe.cpp

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