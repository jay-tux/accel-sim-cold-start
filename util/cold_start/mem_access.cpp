#include <iostream>
#include <unordered_set>
#include <string>

int main () {
  std::string line;
  std::unordered_set<std::string> addr{};
  size_t kernel = 0;
  std::string name{};

  std::cout << "kernel name,unique DRAM accesses\n";

  while(std::getline(std::cin, line)) {
    if(line[0] == '@') {
      // kernel name
      if(kernel++ != 0) {
        std::cout << name << "," << addr.size() << "\n";
        addr.clear();
      }
      name = line.substr(1);
    }
    else {
      // memory address
      addr.insert(line);
    }
  }
  std::cout << name << "," << addr.size() << "\n";

  return 0;
}
