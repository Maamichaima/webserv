// #include <iostream>
// #include <string>
// #include <ctime>
// #include <iomanip>
// #include <sstream>

// class ServerLogger {
// public:
//     // ANSI Color Codes
//     static const std::string RESET;
//     static const std::string BOLD;
//     static const std::string DIM;
    
//     // Colors
//     static const std::string RED;
//     static const std::string GREEN;
//     static const std::string YELLOW;
//     static const std::string BLUE;
//     static const std::string MAGENTA;
//     static const std::string CYAN;
//     static const std::string WHITE;
//     static const std::string GRAY;
    
//     // Background Colors
//     static const std::string BG_RED;
//     static const std::string BG_GREEN;
//     static const std::string BG_YELLOW;
    
//     // Get current timestamp
//     static std::string getCurrentTime() {
//         std::time_t now = std::time(0);
//         std::tm* timeinfo = std::localtime(&now);
//         std::stringstream ss;
//         ss << std::put_time(timeinfo, "%H:%M:%S");
//         return ss.str();
//     }
    
//     // Server startup messages
//     static void serverStarted(int serverNum, const std::string& ip, int port) {
//         std::cout << BOLD << CYAN << "╭─────────────────────────────────────╮" << RESET << std::endl;
//         std::cout << BOLD << CYAN << "│" << RESET << "         " << BOLD << GREEN << "🚀 WebServ Started" << RESET << "         " << BOLD << CYAN << "│" << RESET << std::endl;
//         std::cout << BOLD << CYAN << "╰─────────────────────────────────────╯" << RESET << std::endl;
//         std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
//                   << " " << BOLD << CYAN << "[WebServ]" << RESET 
//                   << " " << BOLD << GREEN << "✓ Server " << serverNum 
//                   << " [" << ip << ":" << port << "] started successfully" << RESET << std::endl;
//         std::cout << GRAY << "    └─ Ready to accept connections..." << RESET << std::endl;
//         std::cout << std::endl;
//     }
    
//     // Multiple servers started
//     static void allServersStarted(int count) {
//         std::cout << BOLD << GREEN << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << RESET << std::endl;
//         std::cout << BOLD << GREEN << "🎉 All " << count << " servers initialized successfully!" << RESET << std::endl;
//         std::cout << BOLD << GREEN << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << RESET << std::endl;
//         std::cout << std::endl;
//     }
    
//     // Error messages
//     static void serverError(const std::string& error, const std::string& details = "") {
//         std::cout << BOLD << RED << "╭─────────────────────────────────────╮" << RESET << std::endl;
//         std::cout << BOLD << RED << "│" << RESET << "           " << BOLD << BG_RED << WHITE << " ERROR " << RESET << "            " << BOLD << RED << "│" << RESET << std::endl;
//         std::cout << BOLD << RED << "╰─────────────────────────────────────╯" << RESET << std::endl;
//         std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
//                   << " " << BOLD << CYAN << "[WebServ]" << RESET 
//                   << " " << BOLD << RED << "✗ " << error << RESET << std::endl;
//         if (!details.empty()) {
//             std::cout << GRAY << "    └─ " << details << RESET << std::endl;
//         }
//         std::cout << std::endl;
//     }
    
//     // Port binding error
//     static void portBindError(int port, const std::string& reason) {
//         std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
//                   << " " << BOLD << CYAN << "[WebServ]" << RESET 
//                   << " " << BOLD << RED << "✗ Failed to bind port " << port << RESET << std::endl;
//         std::cout << GRAY << "    └─ Reason: " << reason << RESET << std::endl;
//         std::cout << YELLOW << "    💡 Tip: Try a different port or check if another service is using it" << RESET << std::endl;
//         std::cout << std::endl;
//     }
    
//     // Client connection messages
//     static void clientConnected(const std::string& clientIP, int clientPort, int serverPort) {
//         std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
//                   << " " << BOLD << CYAN << "[WebServ]" << RESET 
//                   << " " << BOLD << GREEN << "🔗 New connection" << RESET 
//                   << " from " << BOLD << YELLOW << clientIP << ":" << clientPort << RESET 
//                   << " → " << BOLD << BLUE << "server:" << serverPort << RESET << std::endl;
//     }
    
//     // Client disconnection messages
//     static void clientDisconnected(const std::string& clientIP, int clientPort, const std::string& reason = "") {
//         std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
//                   << " " << BOLD << CYAN << "[WebServ]" << RESET 
//                   << " " << BOLD << YELLOW << "🔌 Client disconnected" << RESET 
//                   << " " << BOLD << GRAY << clientIP << ":" << clientPort << RESET;
//         if (!reason.empty()) {
//             std::cout << " " << GRAY << "(" << reason << ")" << RESET;
//         }
//         std::cout << std::endl;
//     }
    
//     // Request handling
//     static void requestReceived(const std::string& method, const std::string& path, const std::string& clientIP) {
//         std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
//                   << " " << BOLD << CYAN << "[WebServ]" << RESET 
//                   << " " << BOLD << MAGENTA << "📥 " << method << RESET 
//                   << " " << BOLD << WHITE << path << RESET 
//                   << " from " << BOLD << GRAY << clientIP << RESET << std::endl;
//     }
    
//     // Response sent
//     static void responseSent(int statusCode, const std::string& clientIP) {
//         std::string statusColor = (statusCode >= 200 && statusCode < 300) ? GREEN : 
//                                  (statusCode >= 400) ? RED : YELLOW;
//         std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
//                   << " " << BOLD << CYAN << "[WebServ]" << RESET 
//                   << " " << BOLD << "📤 Response " << RESET 
//                   << BOLD << statusColor << statusCode << RESET 
//                   << " to " << BOLD << GRAY << clientIP << RESET << std::endl;
//     }
    
//     // Server shutdown
//     static void serverShutdown() {
//         std::cout << std::endl;
//         std::cout << BOLD << YELLOW << "╭─────────────────────────────────────╮" << RESET << std::endl;
//         std::cout << BOLD << YELLOW << "│" << RESET << "        " << BOLD << YELLOW << "🛑 Server Shutdown" << RESET << "        " << BOLD << YELLOW << "│" << RESET << std::endl;
//         std::cout << BOLD << YELLOW << "╰─────────────────────────────────────╯" << RESET << std::endl;
//         std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
//                   << " " << BOLD << CYAN << "[WebServ]" << RESET 
//                   << " " << BOLD << YELLOW << "Gracefully shutting down..." << RESET << std::endl;
//         std::cout << GRAY << "    └─ Thank you for using WebServ! 👋" << RESET << std::endl;
//         std::cout << std::endl;
//     }
    
//     // Configuration loaded
//     static void configLoaded(const std::string& configFile) {
//         std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
//                   << " " << BOLD << CYAN << "[WebServ]" << RESET 
//                   << " " << BOLD << GREEN << "📋 Configuration loaded" << RESET 
//                   << " from " << BOLD << WHITE << configFile << RESET << std::endl;
//     }
    
//     // Warning messages
//     static void warning(const std::string& message) {
//         std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
//                   << " " << BOLD << CYAN << "[WebServ]" << RESET 
//                   << " " << BOLD << YELLOW << "⚠️  Warning: " << message << RESET << std::endl;
//     }
    
//     // Info messages
//     static void info(const std::string& message) {
//         std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
//                   << " " << BOLD << CYAN << "[WebServ]" << RESET 
//                   << " " << BOLD << BLUE << "ℹ️  " << message << RESET << std::endl;
//     }
// };

// // Define static color constants
// const std::string ServerLogger::RESET = "\033[0m";
// const std::string ServerLogger::BOLD = "\033[1m";
// const std::string ServerLogger::DIM = "\033[2m";

// const std::string ServerLogger::RED = "\033[31m";
// const std::string ServerLogger::GREEN = "\033[32m";
// const std::string ServerLogger::YELLOW = "\033[33m";
// const std::string ServerLogger::BLUE = "\033[34m";
// const std::string ServerLogger::MAGENTA = "\033[35m";
// const std::string ServerLogger::CYAN = "\033[36m";
// const std::string ServerLogger::WHITE = "\033[37m";
// const std::string ServerLogger::GRAY = "\033[90m";

// const std::string ServerLogger::BG_RED = "\033[41m";
// const std::string ServerLogger::BG_GREEN = "\033[42m";
// const std::string ServerLogger::BG_YELLOW = "\033[43m";

// // Example usage in your server code:
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