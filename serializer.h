#include <string>
#include <cstring>
#include <cstdint>
#include <iostream>

class serializer
{
public:
  
	inline void store_bytes(const void* data, size_t len)
	{
		buff.append((char*)data, len);
	}

	inline void load_bytes(void* data, size_t len)
	{
		if(buff.size() < position+len)
			throw std::runtime_error("load_bytes buffer overflow");

		memcpy(data, buff.data()+position, len);
		position += len;
	}

	inline void store_int(size_t data)
	{
		uint8_t tmp_buff[MAX_VARINT_SIZE];
		size_t len = varint_encode(tmp_buff, data);
		buff.append((char*)tmp_buff, len);
	}

	inline size_t load_int()
	{
		size_t data;
		size_t len = varint_decode((uint8_t*)(buff.data()+position), buff.size()-position, data);

		if(len == 0)
			throw std::runtime_error("load_int decode error");
		
		position += len;

		return data;
	}

	template<typename T>
	void store_pod(const T& pod)
	{
		size_t len = sizeof(T);
		buff.append(reinterpret_cast<const char*>(&pod), len);
	}

	template<typename T>
	T load_pod()
	{
		size_t len = sizeof(T);
		if(buff.size() < position+len)
			throw std::runtime_error("read_pod buffer overflow");

		T pod;
		memcpy(&pod, buff.data()+position, len);
		position += len;

		return pod;
	}
private:
	inline size_t varint_encode(uint8_t* buf, uint64_t num)
	{
		if (num > UINT64_MAX / 2)
			return 0;

		size_t i = 0;

		while (num >= 0x80)
		{
			buf[i++] = (uint8_t)(num) | 0x80;
			num >>= 7;
		}

		buf[i++] = (uint8_t)(num);
		return i;
	}

	inline size_t varint_decode(const uint8_t* buf, size_t buflen, uint64_t& num)
	{
		if (buflen == 0)
			return 0;

		if (buflen > 9)
			buflen = 9;

		num = buf[0] & 0x7F;
		size_t i = 0;

		while (buf[i++] & 0x80)
		{
			if (i >= buflen || buf[i] == 0x00)
				return 0;
			num |= (uint64_t)(buf[i] & 0x7F) << (i * 7);
		}

		return i;
	}

	std::string buff;
	size_t position = 0;

	static constexpr size_t MAX_VARINT_SIZE = 9;
};