
CXX = c++

# CXXFLAGS = -g -std=c++98 #-fsanitize=address  
# -Wall -Wextra -Werror -std=c++98 

CPPFILES =  rihab_/main.cpp rihab_/Socket.cpp rihab_/ServerManager.cpp rihab_/parsing/Server.cpp rihab_/parsing/tokenizer.cpp \
rihab_/parsing/check_syntaxe.cpp chaima_/parcere.cpp chaima_/parce_http_rq.cpp chaima_/client.cpp chaima_/implRegEX.cpp \
 chaima_/RegEx_utils.cpp chaima_/RegEx.cpp
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