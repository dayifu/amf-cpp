#include "amftest.hpp"

#include "amf.hpp"
#include "serializer.hpp"


TEST(SerializerTest, SingleValue) {
	Serializer s;
	s << AmfInteger(0xffffffe);

	v8 expected { 0x04, 0xBF, 0xFF, 0xFF, 0xFE };
	ASSERT_EQ(expected, s.data());
}

TEST(SerializerTest, Clear) {
	Serializer s;
	s << AmfDouble(0.5);
	s.clear();

	ASSERT_EQ(v8 { }, s.data());

	s << AmfDouble(-1.2);
	v8 expected { 0x05, 0xBF, 0xF3, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33 };
	ASSERT_EQ(expected, s.data());
}

TEST(SerializerTest, MultipleDoubleValues) {
	Serializer s;
	s << AmfDouble(0.5) << AmfDouble(2.5e+51);

	v8 expected {
		0x05, 0x3F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x05, 0x4A, 0x9A, 0xBA, 0x47, 0x14, 0x95, 0x7D, 0x30
	};
	ASSERT_EQ(expected, s.data());

	s << AmfDouble(-1.2);
	expected = {
		0x05, 0x3F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x05, 0x4A, 0x9A, 0xBA, 0x47, 0x14, 0x95, 0x7D, 0x30,
		0x05, 0xBF, 0xF3, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33
	};
	ASSERT_EQ(expected, s.data());
}

TEST(SerializerTest, MultipleStringValues) {
	Serializer s;
	s << AmfString("bar") << AmfString("boofar");

	v8 expected {
		0x06, 0x07, 0x62, 0x61, 0x72,
		0x06, 0x0D, 0x62, 0x6F, 0x6F, 0x66, 0x61, 0x72
	};
	ASSERT_EQ(expected, s.data());
}

TEST(SerializerTest, MultipleMixedValues) {
	Serializer s;
	s << AmfDouble(0.5) << AmfInteger(0x3ff) << AmfString("bar") << AmfNull();

	v8 expected {
		0x05, 0x3F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x04, 0x87, 0x7F,
		0x06, 0x07, 0x62, 0x61, 0x72,
		0x01
	};
	ASSERT_EQ(expected, s.data());
}
