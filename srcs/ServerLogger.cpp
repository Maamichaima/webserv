#include "../_includes/ServerLogger.hpp"

std::string ServerLogger::getCurrentTime() {
    std::time_t now = std::time(0);
    std::tm* timeinfo = std::localtime(&now);
    std::stringstream ss;
    ss << (timeinfo->tm_hour < 10 ? "0" : "") << timeinfo->tm_hour << ":"
       << (timeinfo->tm_min < 10 ? "0" : "") << timeinfo->tm_min << ":"
       << (timeinfo->tm_sec < 10 ? "0" : "") << timeinfo->tm_sec;
    return ss.str();
}

 void ServerLogger::serverStarted() {
    std::cout << BOLD << CYAN << "╭─────────────────────────────────────╮" << RESET << std::endl;
    std::cout << BOLD << CYAN << "│" << RESET << "         " << BOLD << GREEN << "Server Started" << RESET << "         " << BOLD << CYAN << "│" << RESET << std::endl;
    std::cout << BOLD << CYAN << "╰─────────────────────────────────────╯" << RESET << std::endl;
    std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
              << " " << BOLD << CYAN << "[WebServ]" << RESET 
              << " " << BOLD << GREEN << "Server "  << std::endl;
}

void ServerLogger::serverCreated( std::string port) {
    std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
              << " " << BOLD << CYAN << "[WebServ]" << RESET 
              << " " << BOLD << GREEN << "🚀 Server  created successfully!!! Listening on  port " 
              << BOLD << YELLOW  << ":" << port << RESET << std::endl;
}

void ServerLogger::configSyntaxError(std::string msg) {
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

void ServerLogger::serverError(const std::string& details = "") {
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

void ServerLogger::portBindError(std::string port) {
    std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
              << " " << BOLD << CYAN << "[WebServ]" << RESET 
              << " " << BOLD << RED << "✗ Failed to bind port " << port << RESET << std::endl;
    std::cout << YELLOW << "    💡 Tip: Try a different port or check if another service is using it" << RESET << std::endl;
    std::cout << std::endl;
}

void ServerLogger::portListenError(std::string port) {
    std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
              << " " << BOLD << CYAN << "[WebServ]" << RESET 
              << " " << BOLD << RED << "✗ Failed to listen port " << port << RESET << std::endl;
    std::cout << YELLOW << "    💡 Tip: Try a different port or check if another service is using it" << RESET << std::endl;
    std::cout << std::endl;
}

void ServerLogger::clientConnected( std::string serverPort) {
    std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
              << " " << BOLD << CYAN << "[WebServ]" << RESET 
              << " " << BOLD << GREEN << "🔗 New connection " << RESET 
              << " → " << BOLD << BLUE << "server:" << serverPort << RESET << std::endl;
}


void ServerLogger::clientDisconnected() {
    std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
              << " " << BOLD << CYAN << "[WebServ]" << RESET 
              << " " << BOLD << YELLOW << "🔌 Client disconnected" << RESET ;
    
    std::cout << std::endl;
}

void ServerLogger::responseSent(int statusCode) {
    std::string statusColor = (statusCode >= 200 && statusCode < 300) ? GREEN : 
                             (statusCode >= 400) ? RED : YELLOW;
    std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
              << " " << BOLD << CYAN << "[WebServ]" << RESET 
              << " " << BOLD <<  GREEN << "📤 Response  send with " << BOLD  <<  YELLOW  << statusCode  
              << BOLD << GREEN <<  " status code" << GREEN <<  std::endl;
}


// static void requestReceived(const std::string& method, const std::string& path, const std::string& clientIP) {
//     std::cout << BOLD << BLUE << "[" << getCurrentTime() << "]" << RESET 
//               << " " << BOLD << CYAN << "[WebServ]" << RESET 
//               << " " << BOLD << MAGENTA << "📥 " << method << RESET 
//               << " " << BOLD << WHITE << path << RESET 
//               << " from " << BOLD << GRAY << clientIP << RESET << std::endl;
// }
