#include "stddef.h"

/**
 * An opaque struct that holds the results of the melting operatation
 */
typedef struct MeltedBuffer MeltedBuffer;

typedef struct MeltedBufferResult MeltedBufferResult;

typedef struct PdsError PdsError;

typedef struct PdsFile PdsFile;

typedef struct PdsFileResult PdsFileResult;

typedef struct PdsMeta PdsMeta;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Destroys a `MeltedBuffer` once you are done with it.
 *
 * # Safety
 *
 * Must pass in a valid pointer to a `MeltedBuffer`
 */
void rakaly_free_melt(struct MeltedBuffer *res);

/**
 * Returns the length of the melted data in bytes.
 *
 * # Safety
 *
 * Must pass in a valid pointer to a `MeltedBuffer`
 */
size_t rakaly_melt_data_length(const struct MeltedBuffer *res);

/**
 * Returns true if the melter performed no work on the input
 *
 * # Safety
 *
 * Must pass in a valid pointer to a `MeltedBuffer`
 */
bool rakaly_melt_is_verbatim(const struct MeltedBuffer *res);

/**
 * Returns true if the melter encountered unknown tokens in the binary input
 *
 * # Safety
 *
 * Must pass in a valid pointer to a `MeltedBuffer`
 */
bool rakaly_melt_binary_unknown_tokens(const struct MeltedBuffer *res);

/**
 * Writes plaintext data into a provided buffer that is a given length.
 *
 * The encoding of the written data is dependant on the game. For instance, EU4
 * will fill the provided buffer with Windows-1252 encoded data, while CK3 uses
 * UTF-8.
 *
 * Returns the number of bytes copied from the melted data to the provided
 * buffer.
 *
 * If the buffer is not long enough for the melted data, then 0 is returned.
 *
 * If the melted data or provided buffer are null, then 0 is returned.
 *
 * # Safety
 *
 * - Must pass in a valid pointer to a `MeltedBuffer`
 * - Given buffer must be at least the given length in size
 */
size_t rakaly_melt_write_data(const struct MeltedBuffer *res, char *buffer, size_t length);

/**
 * Consume a result and return the underlying error. If the result does not
 * encompass an error, the result is not consumed.
 *
 * # Safety
 *
 * - Must pass in a valid pointer to a `PdsFileResult`
 */
struct PdsError *rakaly_file_error(struct PdsFileResult *ptr);

/**
 * Calculate the number of bytes in the for the melted output's error message.
 * The length excludes null termination
 *
 * # Safety
 *
 * Must pass in a valid pointer to a `PdsError`
 */
int rakaly_error_length(const struct PdsError *res);

/**
 * Write the most recent error message into a caller-provided buffer as a UTF-8
 * string, returning the number of bytes written.
 *
 * # Note
 *
 * This writes a **UTF-8** string into the buffer. Windows users may need to
 * convert it to a UTF-16 "unicode" afterwards.
 *
 * `-1` is returned if there are any errors, for example when passed a
 * null pointer or a buffer of insufficient size.
 *
 * The buffer will not be null terminated.
 *
 * # Safety
 *
 * - Must pass in a valid pointer to a `PdsError`
 * - Given buffer must be at least the given length in size
 */
int rakaly_error_write_data(const struct PdsError *res, char *buffer, int length);

/**
 * Destroys a `PdsError`
 *
 * # Safety
 *
 * Must pass in a valid pointer to a `MeltedBuffer`
 */
void rakaly_free_error(struct PdsError *res);

/**
 * Destroys a `PdsFile`
 *
 * # Safety
 *
 * Must pass in a valid pointer to a `PdsFile`
 */
void rakaly_free_file(struct PdsFile *res);

/**
 * Consume a result and return the underlying value. If the result does not
 * encompass a value, the result is not consumed.
 *
 * # Safety
 *
 * - Must pass in a valid pointer to a `PdsFileResult`
 */
struct PdsFile *rakaly_file_value(struct PdsFileResult *ptr);

/**
 * Returns a pointer to data that can decode a save's metadata. If a save does
 * not have easily extractable metadata, then a null pointer is returned.
 *
 * # Safety
 *
 * - Must pass in a valid pointer to a `PdsFile`
 */
struct PdsMeta *rakaly_file_meta(const struct PdsFile *ptr);

/**
 * Return the result of converting the metadata of a save to plaintext
 *
 * # Safety
 *
 * - Must pass in a valid pointer to a `PdsMeta`
 */
struct MeltedBufferResult *rakaly_file_meta_melt(const struct PdsMeta *ptr);

/**
 * Return the result of converting the save to plaintext
 *
 * # Safety
 *
 * - Must pass in a valid pointer to a `PdsFile`
 */
struct MeltedBufferResult *rakaly_file_melt(const struct PdsFile *ptr);

/**
 * Returns true if the melter needed to convert the binary input
 *
 * # Safety
 *
 * Must pass in a valid pointer to a `MeltedBuffer`
 */
bool rakaly_file_is_binary(const struct PdsFile *res);

/**
 * Consume a result and return the underlying error. If the result does not
 * encompass an error, the result is not consumed.
 *
 * # Safety
 *
 * - Must pass in a valid pointer to a `MeltedBufferResult`
 */
struct PdsError *rakaly_melt_error(struct MeltedBufferResult *ptr);

/**
 * Consume a result and return the underlying value. If the result does not
 * encompass a value, the result is not consumed.
 *
 * # Safety
 *
 * - Must pass in a valid pointer to a `MeltedBufferResult`
 */
struct MeltedBuffer *rakaly_melt_value(struct MeltedBufferResult *ptr);

/**
 * Initializes an EU4 save from a pointer the save data bytes and a number of
 * those bytes.
 *
 * # Safety
 *
 * The data is assumed to exist for the duration while the result of this
 * function lives.
 */
struct PdsFileResult *rakaly_eu4_file(const char *data_ptr, size_t data_len);

/**
 * Initializes an CK3 save from a pointer the save data bytes and a number of
 * those bytes.
 *
 * # Safety
 *
 * The data is assumed to exist for the duration while the result of this
 * function lives.
 */
struct PdsFileResult *rakaly_ck3_file(const char *data_ptr, size_t data_len);

/**
 * Initializes an Imperator save from a pointer the save data bytes and a number of
 * those bytes.
 *
 * # Safety
 *
 * The data is assumed to exist for the duration while the result of this
 * function lives.
 */
struct PdsFileResult *rakaly_imperator_file(const char *data_ptr, size_t data_len);

/**
 * Initializes an HOI4 save from a pointer the save data bytes and a number of
 * those bytes.
 *
 * # Safety
 *
 * The data is assumed to exist for the duration while the result of this
 * function lives.
 */
struct PdsFileResult *rakaly_hoi4_file(const char *data_ptr, size_t data_len);

/**
 * Initializes a Vic3 save from a pointer the save data bytes and a number of
 * those bytes.
 *
 * # Safety
 *
 * The data is assumed to exist for the duration while the result of this
 * function lives.
 */
struct PdsFileResult *rakaly_vic3_file(const char *data_ptr, size_t data_len);

/**
 * Initializes an EU5 save from a pointer the save data bytes and a number of
 * those bytes.
 *
 * # Safety
 *
 * The data is assumed to exist for the duration while the result of this
 * function lives.
 */
struct PdsFileResult *rakaly_eu5_file(const char *data_ptr, size_t data_len);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#ifdef __cplusplus
#ifndef RAKALY_WRAPPER_H
#define RAKALY_WRAPPER_H

#include <optional>
#include <stdexcept>
#include <string>

namespace rakaly {

void unwrapError(PdsError *err) {
  if (err != nullptr) {
    int error_len = rakaly_error_length(err);
    std::string error(error_len, ' ');
    rakaly_error_write_data(err, error.data(), error_len);
    rakaly_free_error(err);
    auto msg = std::string("librakaly returned an error ") + error;
    throw std::runtime_error(msg);
  }
}

class MeltedOutput {
  MeltedBuffer *melt;

  MeltedOutput(const MeltedOutput &) = delete;

public:
  MeltedOutput(MeltedBuffer *melt) { this->melt = melt; }

  /**
   * Updates the given string with the melted output. The string is assumed to
   * contain the data that was requested to be melted, as if the melter required
   * no work, the string won't be written to (as it is already melted)
   */
  void writeData(std::string &data) const {
    // The passed in data is already uncompressed plaintext
    if (rakaly_melt_is_verbatim(melt)) {
      return;
    }

    size_t len = rakaly_melt_data_length(melt);
    data.resize(len);
    if (rakaly_melt_write_data(melt, data.data(), len) != len) {
      throw std::runtime_error("librakaly failed to copy data.");
    }
  }

  bool has_unknown_tokens() const {
    return rakaly_melt_binary_unknown_tokens(melt);
  }

  virtual ~MeltedOutput() { rakaly_free_melt(melt); }
};

class GameFile {
  PdsFile *file;

  GameFile(const GameFile &) = delete;

public:
  GameFile(PdsFile *file) { this->file = file; }

  bool is_binary() const { return rakaly_file_is_binary(file); }

  std::optional<MeltedOutput> meltMeta() const {
    PdsMeta *meta = rakaly_file_meta(file);
    if (meta == nullptr) {
      return std::nullopt;
    }

    MeltedBufferResult *melt_result = rakaly_file_meta_melt(meta);
    unwrapError(rakaly_melt_error(melt_result));
    return std::make_optional(rakaly_melt_value(melt_result));
  }

  MeltedOutput melt() const {
    MeltedBufferResult *melt_result = rakaly_file_melt(file);
    unwrapError(rakaly_melt_error(melt_result));
    return MeltedOutput(rakaly_melt_value(melt_result));
  }

  virtual ~GameFile() { rakaly_free_file(file); }
};

GameFile parseEu4(const std::string &data) {
  PdsFileResult *file_result = rakaly_eu4_file(data.c_str(), data.length());
  unwrapError(rakaly_file_error(file_result));
  PdsFile *file = rakaly_file_value(file_result);
  return GameFile(file);
}

GameFile parseCk3(const std::string &data) {
  PdsFileResult *file_result = rakaly_ck3_file(data.c_str(), data.length());
  unwrapError(rakaly_file_error(file_result));
  PdsFile *file = rakaly_file_value(file_result);
  return GameFile(file);
}

GameFile parseImperator(const std::string &data) {
  PdsFileResult *file_result =
      rakaly_imperator_file(data.c_str(), data.length());
  unwrapError(rakaly_file_error(file_result));
  PdsFile *file = rakaly_file_value(file_result);
  return GameFile(file);
}

GameFile parseHoi4(const std::string &data) {
  PdsFileResult *file_result = rakaly_hoi4_file(data.c_str(), data.length());
  unwrapError(rakaly_file_error(file_result));
  PdsFile *file = rakaly_file_value(file_result);
  return GameFile(file);
}

GameFile parseVic3(const std::string &data) {
  PdsFileResult *file_result = rakaly_vic3_file(data.c_str(), data.length());
  unwrapError(rakaly_file_error(file_result));
  PdsFile *file = rakaly_file_value(file_result);
  return GameFile(file);
}

GameFile parseEu5(const std::string &data) {
  PdsFileResult *file_result = rakaly_eu5_file(data.c_str(), data.length());
  unwrapError(rakaly_file_error(file_result));
  PdsFile *file = rakaly_file_value(file_result);
  return GameFile(file);
}

} // namespace rakaly

#endif
#endif // __cplusplus
