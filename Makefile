CXX = c++

CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -g3 -fsanitize=address

CPPFILES =  main.cpp srcs/Socket.cpp srcs/ServerManager.cpp srcs/Server.cpp srcs/parsing/Tokenizer.cpp \
			srcs/parsing/ConfigSyntaxe.cpp srcs/parsing/Parser.cpp srcs/Client.cpp \
			RegEx/implRegEX.cpp RegEx/RegEx.cpp srcs/methods/GetMethod.cpp srcs/methods/PostMethod.cpp\
			errorPages/HttpErrorPages.cpp srcs/methods/DeleteMethod.cpp sres/ServerLogger.cpp srcs/Utils.cpp \
			srcs/Methods/executeCgi.cpp

OBJDIR = obj

OFILES = $(addprefix $(OBJDIR)/, $(notdir $(CPPFILES:.cpp=.o)))

EXECUTABLE = webserv

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
