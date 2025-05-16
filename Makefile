
CXX = c++

# CXXFLAGS = -g -std=c++98 #-fsanitize=address  
# -Wall -Wextra -Werror -std=c++98 

CPPFILES =  main.cpp srcs/Socket.cpp srcs/ServerManager.cpp srcs/Server.cpp srcs/parsing/tokenizer.cpp \
srcs/parsing/check_syntaxe.cpp srcs/parsing/parser.cpp srcs/parsing/parce_http_rq.cpp srcs/client.cpp RegEx/implRegEX.cpp \
RegEx/RegEx.cpp
OFILES = ${CPPFILES:.cpp=.o}

EXECUTABLE = Server

all : $(EXECUTABLE)

$(EXECUTABLE): $(OFILES)
	@$(CXX) $(OFILES) -o $(EXECUTABLE) $(CXXFLAGS)
	@echo "✅ \033[1;32mCompilation successful!\033[0m "

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf ${OFILES}
	@echo "🧹 \033[1;33mObject files removed.\033[0m"

fclean: clean
	@rm -rf ${EXECUTABLE}
	@echo "🗑️ \033[1;31mExecutable removed.\033[0m​"

	
re: fclean all