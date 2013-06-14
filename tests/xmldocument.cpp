#include "gtest/gtest.h"

#include "amf.hpp"
#include "types/amfxmldocument.hpp"

static void isEqual(const std::vector<u8>& expected, AmfXmlDocument value) {
	std::vector<u8> serialized = value.serialize();
	ASSERT_EQ(expected, serialized) << "Expected length " << expected.size()
	                                << ", got " << serialized.size();;
}

TEST(XmlDocumentSerializationTest, EmptyXmlString) {
	AmfXmlDocument val;

	v8 expected { 0x07, 0x01 };
	ASSERT_EQ(expected, val.serialize());

	val = AmfXmlDocument("");
	ASSERT_EQ(expected, val.serialize());
}

TEST(XmlDocumentSerializationTest, XmlString) {
	AmfXmlDocument val("<asd><foo bar='asd'>qux</foo><boo/></asd>");

	isEqual(v8 {
		0x07, // AMF_XMLDOCUMENT
		0x53, // U29X-value, length = 41
    0x3c, 0x61, 0x73, 0x64, 0x3e, 0x3c, 0x66, 0x6f, 0x6f, 0x20, 0x62, 0x61,
    0x72, 0x3d, 0x27, 0x61, 0x73, 0x64, 0x27, 0x3e, 0x71, 0x75, 0x78, 0x3c,
    0x2f, 0x66, 0x6f, 0x6f, 0x3e, 0x3c, 0x62, 0x6f, 0x6f, 0x2f, 0x3e, 0x3c,
    0x2f, 0x61, 0x73, 0x64, 0x3e
	}, val);
}

TEST(XmlDocumentSerializationTest, UnicodeXmlString) {
	AmfXmlDocument val("<foo>ħĸð@þæĸſ“ð</foo>");

	isEqual(v8 {
		0x07,
		0x3f,
		0x3c, 0x66, 0x6f, 0x6f, 0x3e,
		0xc4, 0xa7, 0xc4, 0xb8, 0xc3, 0xb0, 0x40, 0xc3, 0xbe, 0xc3, 0xa6, 0xc4,
		0xb8, 0xc5, 0xbf, 0xe2, 0x80, 0x9c, 0xc3, 0xb0,
		0x3c, 0x2f, 0x66, 0x6f, 0x6f, 0x3e
 }, val);
}
