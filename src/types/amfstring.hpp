#pragma once
#ifndef AMFSTRING_HPP
#define AMFSTRING_HPP

#include <string>

#include "deserializationcontext.hpp"

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"

namespace amf {

class AmfString : public AmfItem {
public:
	AmfString() { }
	AmfString(const char* v) : value(v == nullptr ? "" : v) { }
	AmfString(std::string v) : value(v) { }
	operator std::string() const { return value; }

	bool operator==(const AmfItem& other) const {
		const AmfString* p = dynamic_cast<const AmfString*>(&other);
		return p != nullptr && value == p->value;
	}

	std::vector<u8> serialize() const {
		if (value.empty())
			return std::vector<u8>{ AMF_STRING, 0x01 };

		// AmfString = string-marker UTF-8-vr
		// with UTF-8-vr = U29S-value *(UTF8-char)
		// U29S-value encodes the length of the following string
		std::vector<u8> buf = AmfInteger::asLength(value.size(), AMF_STRING);

		// now, append the actual string.
		buf.insert(buf.end(), value.begin(), value.end());

		return buf;
	}

	static AmfString deserialize(v8::const_iterator& it, v8::const_iterator end, DeserializationContext& ctx) {
		if (it == end || *it++ != AMF_STRING)
			throw std::invalid_argument("AmfString: Invalid type marker");

		return AmfString(deserializeValue(it, end, ctx));
	}

	static std::string deserializeValue(v8::const_iterator& it, v8::const_iterator end, DeserializationContext& ctx) {
		int type = AmfInteger::deserializeValue(it, end);
		if ((type & 0x01) == 0)
			return ctx.getString(type >> 1);

		int length = type >> 1;
		if (end - it < length)
			throw std::out_of_range("Not enough bytes for AmfString");

		std::string val(it, it + length);
		it += length;

		ctx.addString(val);
		return val;
	}

	std::string value;
};

} // namespace amf

#endif
