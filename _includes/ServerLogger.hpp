#pragma once
#include <iostream>
#include <sstream>
#include <ctime>
#include <string>

#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define DIM         "\033[2m"

#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define GRAY        "\033[90m"

#define BG_RED      "\033[41m"
#define BG_GREEN    "\033[42m"
#define BG_YELLOW   "\033[43m"

class ServerLogger {
    public:
        static std::string getCurrentTime();
        static void serverStarted() ;
        static void serverCreated( std::string port);
        static void configSyntaxError(std::string msg) ;
        static void serverError(const std::string& details) ;
        static void portBindError(std::string port);
        static void portListenError(std::string port) ;
        static void clientConnected( std::string serverPort);
        static void clientDisconnected();
        static void responseSent(int statusCode);
};

