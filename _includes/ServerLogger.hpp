#pragma once
#include <iostream>
#include <sstream>
#include <ctime>
#include <string>

#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define DIM         "\033[2m"

// Colors
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define GRAY        "\033[90m"

// Background Colors
#define BG_RED      "\033[41m"
#define BG_GREEN    "\033[42m"
#define BG_YELLOW   "\033[43m"

class ServerLogger {
public:

    static std::string getCurrentTime() {
        std::time_t now = std::time(0);
        std::tm* timeinfo = std::localtime(&now);
        std::stringstream ss;
        ss << (timeinfo->tm_hour < 10 ? "0" : "") << timeinfo->tm_hour << ":"
           << (timeinfo->tm_min < 10 ? "0" : "") << timeinfo->tm_min << ":"
           << (timeinfo->tm_sec < 10 ? "0" : "") << timeinfo->tm_sec;
        return ss.str();
    }
    
    // Server startup messages
    static void serverStarted() {
        std::cout << BOLD << CYAN << "╭─────────────────────────────────────╮" << RESET << std::endl;
        std::cout << BOLD << CYAN << "│" << RESET << "         " << BOLD << GREEN << "Server Started" << RESET << "         " << BOLD << CYAN << "│" << RESET << std::endl;
        std::cout << BOLD << CYAN << "╰─────────────────────────────────────╯" << RESET << std::endl;
        std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
                  << " " << BOLD << CYAN << "[WebServ]" << RESET 
                  << " " << BOLD << GREEN << "Server "  << std::endl;
    }
    static void serverCreated( std::string port) {
        std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
                  << " " << BOLD << CYAN << "[WebServ]" << RESET 
                  << " " << BOLD << GREEN << "🚀 Server  created successfully!listening on  port " 
                  << BOLD << YELLOW  << ":" << port << RESET << std::endl;
    }
    
    // Multiple servers started
    static void allServersStarted(int count) {
        std::cout << BOLD << GREEN << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << RESET << std::endl;
        std::cout << BOLD << GREEN << "🎉 All " << count << " servers initialized successfully!" << RESET << std::endl;
        std::cout << BOLD << GREEN << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << RESET << std::endl;
        std::cout << std::endl;
    }
    
    static void configSyntaxError(std::string msg) {
        std::cout << BOLD << RED << "╭─────────────────────────────────────╮" << RESET << std::endl;
        std::cout << BOLD << RED << "│" << RESET << "       " << BOLD << BG_RED << WHITE << " CONFIG ERROR " << RESET << "        " << BOLD << RED << "│" << RESET << std::endl;
        std::cout << BOLD << RED << "╰─────────────────────────────────────╯" << RESET << std::endl;
        std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
                  << " " << BOLD << CYAN << "[WebServ]" << RESET 
                  << " " << BOLD << RED << "✗ Syntax error in  your configuration file " << RESET << std::endl;
                  if (!msg.empty()) {
                    std::cout << GRAY << "    └─ Context: " <<  msg << RESET << std::endl;
                }
        std::cout << YELLOW << "    💡 Tip: Check your configuration file syntax" << RESET << std::endl;
        std::cout << std::endl;
    }

    // Error messages
    static void serverError(const std::string& details = "") {
        std::cout << BOLD << RED << "╭─────────────────────────────────────╮" << RESET << std::endl;
        std::cout << BOLD << RED << "│" << RESET << "           " << BOLD << BG_RED << WHITE << " ERROR " << RESET << "            " << BOLD << RED << "│" << RESET << std::endl;
        std::cout << BOLD << RED << "╰─────────────────────────────────────╯" << RESET << std::endl;
        std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
                  << " " << BOLD << CYAN << "[WebServ]" << RESET << std::endl;
        if (!details.empty()) {
            std::cout << GRAY << "    └─ " << details << RESET << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Port binding error
     
    static void portBindError(std::string port) {
        std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
                  << " " << BOLD << CYAN << "[WebServ]" << RESET 
                  << " " << BOLD << RED << "✗ Failed to bind port " << port << RESET << std::endl;
        std::cout << YELLOW << "    💡 Tip: Try a different port or check if another service is using it" << RESET << std::endl;
        std::cout << std::endl;
    }
    static void portListenError(std::string port) {
        std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
                  << " " << BOLD << CYAN << "[WebServ]" << RESET 
                  << " " << BOLD << RED << "✗ Failed to listen port " << port << RESET << std::endl;
        std::cout << YELLOW << "    💡 Tip: Try a different port or check if another service is using it" << RESET << std::endl;
        std::cout << std::endl;
    }
    
    // Client connection messages
    static void clientConnected( int serverPort) {
        std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
                  << " " << BOLD << CYAN << "[WebServ]" << RESET 
                  << " " << BOLD << GREEN << "🔗 New connection " << RESET 
                  << " → " << BOLD << BLUE << "server:" << serverPort << RESET << std::endl;
    }
    
    // Client disconnection messages
    static void clientDisconnected() {
        std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
                  << " " << BOLD << CYAN << "[WebServ]" << RESET 
                  << " " << BOLD << YELLOW << "🔌 Client disconnected" << RESET ;
        
        std::cout << std::endl;
    }
    
    // Request handling
    static void requestReceived(const std::string& method, const std::string& path, const std::string& clientIP) {
        std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
                  << " " << BOLD << CYAN << "[WebServ]" << RESET 
                  << " " << BOLD << MAGENTA << "📥 " << method << RESET 
                  << " " << BOLD << WHITE << path << RESET 
                  << " from " << BOLD << GRAY << clientIP << RESET << std::endl;
    }
    
    // Response sent
    static void responseSent(int statusCode) {
        std::string statusColor = (statusCode >= 200 && statusCode < 300) ? GREEN : 
                                 (statusCode >= 400) ? RED : YELLOW;
        std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
                  << " " << BOLD << CYAN << "[WebServ]" << RESET 
                  << " " << BOLD <<  GREEN << "📤 Response  send with " << BOLD  <<  GREEN  << statusCode  
                  << BOLD << GREEN <<  "status code" << GREEN <<  std::endl;
    }
    
    // Server shutdown
    static void serverShutdown() {
        std::cout << std::endl;
        std::cout << BOLD << YELLOW << "╭─────────────────────────────────────╮" << RESET << std::endl;
        std::cout << BOLD << YELLOW << "│" << RESET << "        " << BOLD << YELLOW << "🛑 Server Shutdown" << RESET << "        " << BOLD << YELLOW << "│" << RESET << std::endl;
        std::cout << BOLD << YELLOW << "╰─────────────────────────────────────╯" << RESET << std::endl;
        std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
                  << " " << BOLD << CYAN << "[WebServ]" << RESET 
                  << " " << BOLD << YELLOW << "Gracefully shutting down..." << RESET << std::endl;
        std::cout << GRAY << "    └─ Thank you for using WebServ! 👋" << RESET << std::endl;
        std::cout << std::endl;
    }
    
    // Configuration loaded
    static void configLoaded(const std::string& configFile) {
        std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
                  << " " << BOLD << CYAN << "[WebServ]" << RESET 
                  << " " << BOLD << GREEN << "📋 Configuration loaded" << RESET 
                  << " from " << BOLD << WHITE << configFile << RESET << std::endl;
    }
    
    // Warning messages
    static void warning(const std::string& message) {
        std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
                  << " " << BOLD << CYAN << "[WebServ]" << RESET 
                  << " " << BOLD << YELLOW << "⚠️  Warning: " << message << RESET << std::endl;
    }
    
    // Info messages
    static void info(const std::string& message) {
        std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
                  << " " << BOLD << CYAN << "[WebServ]" << RESET 
                  << " " << BOLD << BLUE << "ℹ️  " << message << RESET << std::endl;
    }
};

// Define static color constants

// Example usage in your server code:
// void exampleUsage() {
//     // Server startup
//     ServerLogger::configLoaded("webserv.conf");
//     ServerLogger::serverStarted(1, "127.0.0.1", 8080);
//     ServerLogger::serverStarted(2, "0.0.0.0", 8081);
//     ServerLogger::allServersStarted(2);
    
//     // Client interactions
//     ServerLogger::clientConnected("192.168.1.100", 54321, 8080);
//     ServerLogger::requestReceived("GET", "/index.html", "192.168.1.100");
//     ServerLogger::responseSent(200, "192.168.1.100");
//     ServerLogger::clientDisconnected("192.168.1.100", 54321, "connection closed");
    
//     // Errors
//     ServerLogger::portBindError(80, "Permission denied");
//     ServerLogger::serverError("Failed to initialize epoll", "epoll_create1() returned -1");
    
//     // Warnings and info
//     ServerLogger::warning("Large request body detected");
//     ServerLogger::info("Using default configuration");
    
//     // Shutdown
//     ServerLogger::serverShutdown();
// }