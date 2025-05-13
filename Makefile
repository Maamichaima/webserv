SRC = chaima_/mainRegEx.cpp /chaima_/client.cpp /chaima
OBJ = ${SRC:.cpp=.o}
CXXFLAGS = -Wall -Wextra -Werror -std=c++98  
CXX = c++
NAME = webserv

all : ${NAME}

${NAME} : ${OBJ}
	${CXX} -o ${NAME} ${OBJ} ${CXXFLAGS}

clean :
	rm -rf ${OBJ}

fclean : clean
	${RM} ${NAME}

re: fclean all