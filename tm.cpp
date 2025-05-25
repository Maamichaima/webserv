#include <iostream>
#include <string>

// int main() {
//     std::string fullPath = "example/test/image.png/asddasdasdasd";
//     std::string extension;

//     size_t extPos = fullPath.find_last_of('.');
//     if (extPos != std::string::npos)
//         extension = fullPath.substr(extPos);

//     std::cout << "Extension: " << extension << std::endl;

//     return 0;
// }


#include <iostream>
#include <string>

int main() {
    std::string root = "/var/www/html";
    std::string fullPath;
    struct Request {
        std::string path;
    } req;

    req.path = "/index.html?name=John&age=30";

    std::string query;
    size_t pos = req.path.find('?');
    if (pos != std::string::npos) {
        query = req.path.substr(pos + 1);
        fullPath = root + req.path.substr(0, pos);
    } else {
        fullPath = root + req.path;
    }

    std::cout << "Query string: " << query << std::endl;
    std::cout << "Full path: " << fullPath << std::endl;

    return 0;
}
