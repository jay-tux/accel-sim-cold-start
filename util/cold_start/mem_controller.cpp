#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <tuple>

using vec = std::vector<std::vector<float>>;

std::tuple<float, float> active(const vec &rd, const vec &wrt) {
  std::vector<float> averaged{};

  if(rd.size() == 0) return {-1.0f,-1.0f};

  for(size_t i = 0; i < rd.size(); i++) {
    float a1 = 0.0f;
    for(const auto &v: rd[i]) a1 += v;
    a1 /= rd[i].size();

    float a2 = 0.0f;
    for(const auto &v: wrt[i]) a2 += v;
    a2 /= wrt[i].size();

    averaged.push_back((a1 + a2) / 2);
  }

  float max = *std::max_element(averaged.begin(), averaged.end());
  float count = 0.0f;
  for(const auto &x: averaged) {
    count += x / max;
  }

  if(max < 1e-6f || count < 1e-6f) return {1.0f, 0.0f};
  return {count, max};
}

int main () {
  std::string line;
  std::string kernel;
  vec dram_read{}, dram_write{};
  int state = 0;
  int index = 0;

  std::cout << "kernel name,active memory controllers\n";
  while(std::getline(std::cin, line)) {
    if(line[0] == '@') {
      if(kernel != "") {
        const auto &[c, m] = active(dram_read, dram_write);
        std::cout << kernel << "," << c << "\n";
        
        dram_read.clear();
        dram_write.clear();
        state = 0;
        index = 0;
      }
      kernel = line.substr(1);
      state = 0;
    }
    else if(line.starts_with("number of total read")) {
      state = 1;
      index = 0;
    }
    else if(line.starts_with("number of total write")) {
      state = 2;
      index = 0;
    }
    else if(line.starts_with("dram[")) {
      size_t pos;
      auto sub = line.substr(5);
      int idx = stoi(sub, &pos);
      if(idx < index - 1) state = 0;
      index = idx;

      if(state == 0) continue;
      float v;
      std::istringstream strm(sub.substr(pos + 3));
      std::vector<float> temp{};
      while(strm >> v) {
        temp.push_back(v);
      }
      if(state == 1) dram_read.push_back(temp);
      else dram_write.push_back(temp);
    }
  }

  const auto &[c, m] = active(dram_read, dram_write);
  std::cout << kernel << "," << c << "\n";

  return 0;
}
















