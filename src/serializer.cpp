#include "serializer.hpp"

namespace amf {

Serializer& Serializer::operator<<(const AmfItem& item) {
	std::vector<u8> serialized = item.serialize();
	buf.insert(buf.end(), serialized.begin(), serialized.end());

	return *this;
}

} // namespace amf
