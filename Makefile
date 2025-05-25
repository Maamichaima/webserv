CXX = c++

# CXXFLAGS = -fsanitize=address -g3 #-Wall -Wextra -Werror -std=c++98

CPPFILES =  main.cpp srcs/Socket.cpp srcs/ServerManager.cpp srcs/Server.cpp srcs/parsing/tokenizer.cpp \
srcs/parsing/check_syntaxe.cpp srcs/parsing/parser.cpp srcs/parsing/parce_http_rq.cpp srcs/client.cpp \
RegEx/implRegEX.cpp RegEx/RegEx.cpp srcs/methods/GetMethod.cpp srcs/methods/PostMethod.cpp

OBJDIR = obj

OFILES = $(addprefix $(OBJDIR)/, $(notdir $(CPPFILES:.cpp=.o)))

EXECUTABLE = Server

all: $(OBJDIR) $(EXECUTABLE)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

vpath %.cpp $(sort $(dir $(CPPFILES)))

$(EXECUTABLE): $(OFILES)
	@$(CXX) $(OFILES) -o $(EXECUTABLE) $(CXXFLAGS)
	@echo "✅ \033[1;32mCompilation successful!\033[0m"

clean:
	@rm -rf $(OBJDIR)
	@echo "🧹 \033[1;33mObject files removed.\033[0m"

fclean: clean
	@rm -f $(EXECUTABLE)
	@echo "🗑️ \033[1;31mExecutable removed.\033[0m"

re: fclean all
