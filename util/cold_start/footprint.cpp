#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unordered_set>

#define MEM_TYPE_UNCOMPRESSED 0

int main(const int argc, const char **argv) {
  if(argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <*.traceg file>\n";
    return -1;
  }

  std::ifstream strm(argv[1]);
  if(!strm.good()) {
    std::cerr << "Can't open '" << argv[1] << "' for reading\n";
    return -2;
  }

  std::string line;
  size_t lineno = 0;
  std::unordered_set<size_t> footprint;

  while(std::getline(strm, line)) {
    lineno++;
    // ignore non-instruction lines
    if(line.empty() || line[0] == '-' || line[0] == '#' ||
      line.starts_with("thread block = ") || line.starts_with("warp = ") ||
      line.starts_with("insts = ")
    ) continue;

    size_t count;
    size_t type;
    size_t ignore[2];
    size_t addr;
    std::string ignore_str;

    std::istringstream line_strm(line);
    // ignore PC and mask
    line_strm >> std::hex >> ignore[0] >> std::hex >> ignore[1];

    // read & ignore destination registers
    line_strm >> std::dec >> count;
    for(size_t i = 0; i < count; i++) {
      line_strm >> ignore_str;
    }

    // read & ignore opcde
    line_strm >> ignore_str;

    // read & ignore source registers
    line_strm >> std::dec >> count;
    for(size_t i = 0; i < count; i++) {
      line_strm >> ignore_str;
    }

    // read memory count & address type
    line_strm >> std::dec >> count >> std::dec >> type;
    if(type != MEM_TYPE_UNCOMPRESSED) {
      std::cerr << "Line " << lineno << ": only supports uncompressed memory addresses (got " << type << " instead of " << MEM_TYPE_UNCOMPRESSED << ").\n";
    }

    for(size_t i = 0; i < count; i++) {
      line_strm >> std::hex >> addr;
      footprint.insert(addr);
    }
  }

  const std::string fp_name = std::string(argv[1]) + ".ftprnt";
  std::ofstream fp_strm(fp_name);
  if(!fp_strm.good()) {
    std::cerr << "Can't write footprint (can't open '" << fp_name << "' for writing).\n";
  }

  for(const auto &addr: footprint) {
    fp_strm << std::hex << std::showbase << std::setw(16) << std::setfill('0') << addr << "\n";
  }

  std::cout << footprint.size() << " unique addresses.\n";

  return 0;
}
