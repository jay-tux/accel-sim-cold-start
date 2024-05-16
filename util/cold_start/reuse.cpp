#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <string>
#include <iomanip>

// Usage: ./compute_reuse <input file 1> <input file 2>
using set_t = std::unordered_set<size_t>;

set_t load(std::ifstream &strm) {
  set_t res{};
  std::string line;
  while(std::getline(strm, line)) {
    if(line.empty() || line[0] == '-' || line[0] == '#' || 
        line.starts_with("thread block") || line.starts_with("warp") || line.starts_with("insts")) continue;

    size_t count, ignore, addr;
    std::string ignore_str;
    std::istringstream lstrm(line);

    // PC, mask
    lstrm >> std::hex >> ignore >> std::hex >> ignore;
    // dest regs
    lstrm >> std::dec >> count;
    for(size_t i = 0; i < count; i++) lstrm >> ignore_str;
    // opcode
    lstrm >> ignore_str;
    // src regs
    lstrm >> std::dec >> count;
    for(size_t i = 0; i < count; i++) lstrm >> ignore_str;
    // mem count, addr type
    lstrm >> std::dec >> count >> std::dec >> ignore;
    // addrs
    for(size_t i = 0; i < count; i++) {
      lstrm >> std::hex >> addr;
      res.insert(addr);
    }
  }

  return res;
}

double compute_reuse(const set_t &from, const set_t &to) {
  size_t matches = 0;
  for(const auto &v: from) {
    if(to.contains(v)) matches++;
  }

  return (double)matches / (double)from.size();
}

int main(int argc, const char **argv) {
  if(argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <trace from> <trace to>\n";
    return -1;
  }
  std::ifstream from{argv[1]}, to{argv[2]};
  if(!from.good()) {
    std::cerr << "Can't open file " << argv[1] << ".\n";
    return -2;
  }
  else if(!to.good()) {
    std::cerr << "Can't open file " << argv[2] << ".\n";
    return -3;
  }

  set_t s_from = load(from);
  set_t s_to = load(to);
  double reuse = compute_reuse(s_from, s_to);
  std::cout << reuse << "\n";
  return 0;
}
