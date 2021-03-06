#pragma once
#ifndef AMFARRAY_HPP
#define AMFARRAY_HPP

#include <map>

#include "deserializationcontext.hpp"
#include "deserializer.hpp"

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"
#include "types/amfstring.hpp"

#include "utils/amfitemptr.hpp"

namespace amf {

class AmfArray : public AmfItem {
public:
	AmfArray() { }

	template<class V>
	AmfArray(std::vector<V> densePart) {
		for (const V& it : densePart)
			push_back(it);
	}

	template<class V, class A>
	AmfArray(std::vector<V> densePart, std::map<std::string, A> associativePart) {
		for (const V& it : densePart)
			push_back(it);

		for (const auto& it : associativePart)
			insert(it.first, it.second);
	}

	bool operator==(const AmfItem& other) const {
		const AmfArray* p = dynamic_cast<const AmfArray*>(&other);
		return p != nullptr && dense == p->dense && associative == p->associative;
	}

	template<class T>
	void push_back(const T& item) {
		static_assert(std::is_base_of<AmfItem, T>::value, "Elements must extend AmfItem");

		dense.emplace_back(new T(item));
	}

	template<class T>
	void insert(const std::string key, const T& item) {
		static_assert(std::is_base_of<AmfItem, T>::value, "Elements must extend AmfItem");

		associative[key] = AmfItemPtr(new T(item));
	}

	template<class T>
	T& at(int index) {
		return dense.at(index).as<T>();
	}

	template<class T>
	T& at(std::string key) {
		return associative.at(key).as<T>();
	}

	std::vector<u8> serialize() const {
		/*
		 * array-marker
		 * (
		 * 	U29O-ref |
		 * 	(U29A-value *(assoc-value) UTF-8-empty *(value-type))
		 * )
		 */
		// U29A-value
		std::vector<u8> buf = AmfInteger::asLength(dense.size(), AMF_ARRAY);

		for (const auto& it : associative) {
			auto name = AmfString(it.first).serialize();
			auto s = it.second->serialize();

			// skip AmfString marker
			buf.insert(buf.end(), name.begin() + 1, name.end());
			buf.insert(buf.end(), s.begin(), s.end());
		}

		// UTF-8-empty
		buf.push_back(0x01);

		for (const auto& it : dense) {
			auto s = it->serialize();
			buf.insert(buf.end(), s.begin(), s.end());
		}

		return buf;
	}

	static AmfArray deserialize(v8::const_iterator& it, v8::const_iterator end, DeserializationContext& ctx) {
		if (it == end || *it++ != AMF_ARRAY)
			throw std::invalid_argument("AmfArray: Invalid type marker");

		int type = AmfInteger::deserializeValue(it, end);
		if ((type & 0x01) == 0)
			return ctx.getObject<AmfArray>(type >> 1);

		AmfArray ret;
		size_t contextIndex = ctx.addObject<AmfArray>(ret);

		// associative until UTF-8-empty
		while (true) {
			std::string name = AmfString::deserializeValue(it, end, ctx);
			if (name == "") break;

			AmfItemPtr val = Deserializer::deserialize(it, end, ctx);
			ret.associative[name] = val;
		}

		// dense
		int length = type >> 1;
		for (int i = 0; i < length; ++i)
			ret.dense.push_back(Deserializer::deserialize(it, end, ctx));

		ctx.setObject<AmfArray>(contextIndex, ret);
		return ret;
	}


	std::vector<AmfItemPtr> dense;
	std::map<std::string, AmfItemPtr> associative;
};

} // namespace amf

#endif
