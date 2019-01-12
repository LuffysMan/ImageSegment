#pragma once
#include <ostream>
#include <istream>
#include <sstream>
#include <string>
#include "assert.h"

typedef unsigned char byte;
typedef long long int64;

namespace persistent {
	const long true_pattern = 0xF00DF00D;
	const long false_pattern = 0xBAD0BAD0;
	class serializer
	{
	public:
		serializer(std::ostream& os) : _ostream(os) {}
		void put_char(char value) { return put_basic_t<char>(value); }
		void put_wchar(wchar_t value) { return put_basic_t<wchar_t>(value); }
		void put_bool(bool value) { return put_basic_t<bool>(value); }
		void put_short(short value) { return put_basic_t<short>(value); }
		void put_long(long value) { return put_basic_t<long>(value); }
		void put_llong(int64 value) { return put_basic_t<int64>(value); }
		void put_double(double value) { return put_basic_t<double>(value); }
		void put_string(const std::string& value) { return put_string_t<std::string>(value); }
		void put_wstring(const std::wstring& value) { return put_string_t<std::wstring>(value); }
		void put_raw(size_t length, const byte* buff)
		{
			put_long(static_cast<long>(length));
			_ostream.write(reinterpret_cast<const char*>(buff),
				static_cast<std::streamsize>(length));
			if (_ostream.bad())
				throw std::exception("stream_write");
		}
		//void put_raw(const util::raw_buffer& buff)
		//{
		//	_ostream.write(reinterpret_cast<const char*>(buff.operator const byte*()),
		//		static_cast<std::streamsize>(buff.size()));
		//	if (_ostream.bad())
		//		throw std::exception("stream_write");
		//}
	protected:
		template<typename T> void put_basic_t(T value)
		{
			_ostream.write(reinterpret_cast<char*>(&value), sizeof(T));
			if (_ostream.bad())
				throw std::exception("stream_write");
		}
		template<> void put_basic_t(bool value)
		{
			long pattern = value ? true_pattern : false_pattern;
			put_long(pattern);
			if (_ostream.bad())
				throw std::exception("stream_write");
		}
		template<typename T> void put_string_t(const T& value)
		{
			long len = static_cast<long>(value.length() * sizeof(T::value_type));
			put_long(len);
			_ostream.write(reinterpret_cast<const char*>(value.data()), len);
			if (_ostream.bad())
				throw std::exception("stream_write");
		}
	private:
		std::ostream& _ostream;
		//serializer& operator =(const serializer&);
	};
	class deserializer
	{
	public:
		deserializer(std::istream& is) : _istream(is) {}
		char get_char() { return get_basic_t<char>(); }
		wchar_t get_wchar() { return get_basic_t<wchar_t>(); }
		bool get_bool() { return get_basic_t<bool>(); }
		short get_short() { return get_basic_t<short>(); }
		long get_long() { return get_basic_t<long>(); }
		int64 get_llong() { return get_basic_t<int64>(); }
		double get_double() { return get_basic_t<double>(); }
		std::string get_string() { return get_string_t<std::string>(); }
		std::wstring get_wstring() { return get_string_t<std::wstring>(); }
		//util::raw_buffer get_raw()
		//{
		//	size_t length = static_cast<size_t>(get_long());
		//	if (_istream.eof())
		//		throw std::exception("unexpected_eof");
		//	util::raw_buffer value(length);
		//	_istream.read(value.force_to<char*>(), static_cast<std::streamsize>(length));
		//	if (_istream.bad())
		//		throw std::exception("stream_read");
		//	return value;
		//}
		//void get_raw(util::raw_buffer& buff)
		//{
		//	if (_istream.eof())
		//		throw std::exception("unexpected_eof");
		//	_istream.read(buff.force_to<char*>(), static_cast<std::streamsize>(buff.size()));
		//	if (_istream.bad())
		//		throw std::exception("stream_read");
		//}
	protected:
		template <typename T> T get_basic_t()
		{
			if (_istream.eof())
				throw std::exception("unexpected_eof");
			T value;
			_istream.read(reinterpret_cast<char *>(&value), sizeof(T));
			if (_istream.bad())
				throw std::exception("stream_read");
			return value;
		}
		template<> bool get_basic_t()
		{
			long value = get_long();
			if (_istream.bad())
				throw std::exception("stream_read");
			if (value == true_pattern)
				return true;
			else if (value == false_pattern)
				return false;
			else
				throw std::exception("data_corrupt");
		}
		template<typename T> T get_string_t()
		{
			long len = get_long();
			T value;
			value.resize(len / sizeof(T::value_type));
			if (_istream.eof())
				throw std::exception("unexpected_eof");
			_istream.read(reinterpret_cast<char*>(&value[0]), len);
			if (_istream.bad())
				throw std::exception("stream_read");
			return value;
		}
	private:
		std::istream& _istream;
		//deserializer& operator =(const deserializer&);
	};
}


namespace persistent {
	class serializable
	{
	public:
		virtual void serialize(serializer& out) const = 0;
		virtual void deserialize(deserializer& in) = 0;
	};
}

