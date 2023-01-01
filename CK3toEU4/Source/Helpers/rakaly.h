#include "stddef.h"

/**
 * An opaque struct that holds the results of the melting operatation
 */
typedef struct MeltedBuffer MeltedBuffer;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

	/**
	 * A non-zero return value indicates an error with the melted buffer
	 *
	 * A non-zero status code can occur from the following:
	 *
	 *  - An early EOF
	 *  - Invalid format encountered
	 *  - Too many close delimiters
	 *
	 * # Safety
	 *
	 * Must pass in a valid pointer to a `MeltedBuffer`
	 */
	int rakaly_melt_error_code(const struct MeltedBuffer* res);

	/**
	 * Calculate the number of bytes in the for the melted output's error message.
	 * The length excludes null termination
	 *
	 * # Safety
	 *
	 * Must pass in a valid pointer to a `MeltedBuffer`
	 */
	int rakaly_melt_error_length(const struct MeltedBuffer* res);

	/**
	 * Write the most recent error message into a caller-provided buffer as a UTF-8
	 * string, returning the number of bytes written.
	 *
	 * # Note
	 *
	 * This writes a **UTF-8** string into the buffer. Windows users may need to
	 * convert it to a UTF-16 "unicode" afterwards.
	 *
	 * If there are no recent errors then this returns `0` (because we wrote 0
	 * bytes). `-1` is returned if there are any errors, for example when passed a
	 * null pointer or a buffer of insufficient size.
	 *
	 * The buffer will not be null terminated.
	 *
	 * # Safety
	 *
	 * - Must pass in a valid pointer to a `MeltedBuffer`
	 * - Given buffer must be at least the given length in size
	 */
	int rakaly_melt_error_write_data(const struct MeltedBuffer* res, char* buffer, int length);

	/**
	 * Destroys a `MeltedBuffer` once you are done with it.
	 *
	 * # Safety
	 *
	 * Must pass in a valid pointer to a `MeltedBuffer`
	 */
	void rakaly_free_melt(struct MeltedBuffer* res);

	/**
	 * Returns the length of the melted data in bytes.
	 *
	 * # Safety
	 *
	 * Must pass in a valid pointer to a `MeltedBuffer`
	 */
	size_t rakaly_melt_data_length(const struct MeltedBuffer* res);

	/**
	 * Returns true if the melter performed no work on the input
	 *
	 * # Safety
	 *
	 * Must pass in a valid pointer to a `MeltedBuffer`
	 */
	bool rakaly_melt_is_verbatim(const struct MeltedBuffer* res);

	/**
	 * Returns true if the melter needed to convert the binary input
	 *
	 * # Safety
	 *
	 * Must pass in a valid pointer to a `MeltedBuffer`
	 */
	bool rakaly_melt_binary_translated(const struct MeltedBuffer* res);

	/**
	 * Returns true if the melter encountered unknown tokens in the binary input
	 *
	 * # Safety
	 *
	 * Must pass in a valid pointer to a `MeltedBuffer`
	 */
	bool rakaly_melt_binary_unknown_tokens(const struct MeltedBuffer* res);

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
	size_t rakaly_melt_write_data(const struct MeltedBuffer* res, char* buffer, size_t length);

	/**
	 * Converts a save into uncompressed plaintext data.
	 *
	 * Parameters:
	 *
	 *  - data: Pointer to the save data to convert. It is valid for this data to
	 *    be uncompressed plaintext data, compressed plaintext data, or binary data
	 *  - data_len: Length of the data indicated by the data pointer. It is
	 *    undefined behavior if the given length does not match the actual length
	 *    of the data
	 *
	 * If an unknown binary token is encountered there are two possible outcomes:
	 *
	 *  - If the token is part of an object's key, then key and value will not
	 *    appear in the plaintext output
	 *  - Else the object value (or array value) will be string of "__unknown_x0$z"
	 *    where z is the hexadecimal representation of the unknown token.
	 */
	struct MeltedBuffer* rakaly_eu4_melt(const char* data_ptr, size_t data_len);

	/**
	 * See `rakaly_eu4_melt` for more information
	 */
	struct MeltedBuffer* rakaly_ck3_melt(const char* data_ptr, size_t data_len);

	/**
	 * See `rakaly_eu4_melt` for more information
	 */
	struct MeltedBuffer* rakaly_imperator_melt(const char* data_ptr, size_t data_len);

	/**
	 * See `rakaly_eu4_melt` for more information
	 */
	struct MeltedBuffer* rakaly_hoi4_melt(const char* data_ptr, size_t data_len);

	/**
	 * See `rakaly_eu4_melt` for more information
	 */
	struct MeltedBuffer* rakaly_vic3_melt(const char* data_ptr, size_t data_len);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#ifdef __cplusplus
#ifndef RAKALY_WRAPPER_H
#define RAKALY_WRAPPER_H

#include <stdexcept>
#include <string>

namespace rakaly
{

class MeltedOutput
{
	MeltedBuffer* melt;

  public:
	MeltedOutput(MeltedBuffer* melt) { this->melt = melt; }

	/**
	 * Updates the given string with the melted output. The string is assumed to
	 * contain the data that was requested to be melted, as if the melter required
	 * no work, the string won't be written to (as it is already melted)
	 */
	void writeData(std::string& data) const
	{
		int result = rakaly_melt_error_code(melt);
		if (result)
		{
			int error_len = rakaly_melt_error_length(melt);
			std::string error(error_len, ' ');
			rakaly_melt_error_write_data(melt, error.data(), error_len);
			auto msg = std::string("librakaly returned an error ") + error;
			throw std::runtime_error(msg);
		}

		// The passed in data is already uncompressed plaintext
		if (rakaly_melt_is_verbatim(melt))
		{
			return;
		}

		size_t len = rakaly_melt_data_length(melt);
		data.resize(len);
		if (rakaly_melt_write_data(melt, data.data(), len) != len)
		{
			throw std::runtime_error("librakaly failed to copy data.");
		}
	}

	bool was_binary() const { return rakaly_melt_binary_translated(melt); }
	bool has_unknown_tokens() const { return rakaly_melt_binary_unknown_tokens(melt); }

	virtual ~MeltedOutput() { rakaly_free_melt(melt); }
};

MeltedOutput meltEu4(const std::string& data)
{
	return MeltedOutput(rakaly_eu4_melt(data.c_str(), data.length()));
}

MeltedOutput meltCk3(const std::string& data)
{
	return MeltedOutput(rakaly_ck3_melt(data.c_str(), data.length()));
}

MeltedOutput meltImperator(const std::string& data)
{
	return MeltedOutput(rakaly_imperator_melt(data.c_str(), data.length()));
}

MeltedOutput meltHoi4(const std::string& data)
{
	return MeltedOutput(rakaly_hoi4_melt(data.c_str(), data.length()));
}

MeltedOutput meltVic3(const std::string& data)
{
	return MeltedOutput(rakaly_vic3_melt(data.c_str(), data.length()));
}

} // namespace rakaly

#endif
#endif // __cplusplus
