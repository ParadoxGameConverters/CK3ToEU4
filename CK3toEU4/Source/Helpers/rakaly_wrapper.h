#ifndef RAKALY_WRAPPER_H
#define RAKALY_WRAPPER_H

#include "rakaly.h"
#include <stdexcept>
#include <string>

namespace rakaly {

inline std::string meltFinish(MeltedBuffer *melt) {
  int result = rakaly_melt_error_code(melt);
  if (result) {
    rakaly_free_melt(melt);
    throw std::runtime_error("librakaly returned an error: " +
                             std::to_string(result));
  }

  size_t len = rakaly_melt_data_length(melt);
  std::string str(len, ' ');

  if (rakaly_melt_write_data(melt, str.data(), len) != len) {
    rakaly_free_melt(melt);
    throw std::runtime_error("librakaly failed to copy data.");
  }

  rakaly_free_melt(melt);
  return str;
}

inline std::string meltEU4(const std::string &encrypted) {
  return meltFinish(rakaly_eu4_melt(encrypted.c_str(), encrypted.length()));
}

inline std::string meltCK3(const std::string &encrypted) {
  return meltFinish(rakaly_ck3_melt(encrypted.c_str(), encrypted.length()));
}

} // namespace rakaly

#endif
