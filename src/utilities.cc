#include "utilities.h"

namespace lastro {

// Replace directory and extension of a path
std::string GenerateFilename(const std::string &path, const std::string &dir,
                             const std::string &ext) {
  if (path.empty())
    return dir + "/" + ext;
  
  std::size_t pos_start = 0;
  std::size_t pos_end   = path.size();
  bool extra_slash = false;
  
  if (!dir.empty()) {
    std::size_t pos = path.find_last_of("/");
    if (pos == std::string::npos)
      pos = 0;
    pos_start = pos;
    
    if (dir.back() != '/' && path[pos] != '/') {
      extra_slash = true;
    }  
  }
  
  if (!ext.empty()) {
    std::size_t pos = path.find_last_of(".");
    if (pos == std::string::npos || pos <= pos_start) {
      pos = path.size();
    }
    pos_end = pos;
  }
  
  std::ostringstream ss;
  ss << dir << (extra_slash ? "/" : "")
    << path.substr(pos_start, pos_end - pos_start)
    << ext;
  return ss.str();
}

}