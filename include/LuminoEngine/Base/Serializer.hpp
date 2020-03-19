﻿
#pragma once
#include <functional>
#include "../Engine/Object.hpp"

namespace ln {
class AssetModel;

/** */
LN_CLASS()
class Serializer
	: public Object
{
	LN_OBJECT;
public:
	
	/** write */
	LN_METHOD()
	void writeBool(const StringRef& name, bool value);

	/** write */
	LN_METHOD()
	void writeInt(const StringRef& name, int value);

	/** write */
	LN_METHOD()
	void writeFloat(const StringRef& name, float value);

	/** write */
	LN_METHOD()
	void writeString(const StringRef& name, const StringRef& value);

	/** write */
	LN_METHOD()
	void writeObject(const StringRef& name, Object* value);

	
	/** read */
	LN_METHOD()
	bool readBool(const StringRef& name);

	/** read */
	LN_METHOD()
	int readInt(const StringRef& name);

	/** read */
	LN_METHOD()
	float readFloat(const StringRef& name);

	/** read */
	LN_METHOD()
	String readString(const StringRef& name);

	/** read */
	LN_METHOD()
	Ref<Object> readObject(const StringRef& name);

	
	/** serialize */
	LN_METHOD()
	static String serialize(Object* value, const String& basePath);
	
	/** serialize */
	LN_METHOD()
	static Ref<Object> deserialize(const String& str, const String& basePath);

LN_CONSTRUCT_ACCESS:
	Serializer();
	virtual ~Serializer() = default;
	void init(/*ArchiveMode mode, const String& basePath*/);

private:
	void processSave();
	void processLoad();

	std::unique_ptr<Archive> m_archive;
};

namespace detail {
class SerializerStore2;
}


#if 1
/** */
LN_CLASS()
class Serializer2
	: public Object
{
	LN_OBJECT;
public:

	bool isSaving() const { return m_mode == ArchiveMode::Save; }

	bool isLoading() const { return m_mode == ArchiveMode::Load; }


	void writeName(const StringRef& name);

	/** write */
	//LN_METHOD()
	void writeBool(bool value);

	void writeInt8(int8_t value);
	void writeInt16(int16_t value);
	void writeInt32(int32_t value);
	void writeInt64(int64_t value);
	void writeUInt8(uint8_t value);
	void writeUInt16(uint16_t value);
	void writeUInt32(uint32_t value);
	void writeUInt64(uint64_t value);
	void writeFloat(float value);
	void writeDouble(double value);

	/** write */
	//LN_METHOD()
	void writeInt(int value);


	/** write */
	//LN_METHOD()
	void writeString(const StringRef& value);



	/** write */
	//LN_METHOD()
	// beginObject, endObject のユーティリティ
	void writeObject(Object* value);

	//template<class T>
	//void writeList(const StringRef& name, const List<T>& value)
	//{
	//	beginWriteList(name);

	//	endWriteList();
	//}


	bool readName(const StringRef& name);

	//
	/** read */
	//LN_METHOD()
	bool readBool();

	int8_t readInt8();
	int16_t readInt16();
	int32_t readInt32();
	int64_t readInt64();
	uint8_t readUInt8();
	uint16_t readUInt16();
	uint32_t readUInt32();
	uint64_t readUInt64();
	float readFloat();
	double readDouble();

	/** read */
	//LN_METHOD()
	int readInt();

	/** read */
	//LN_METHOD()
	float readFloat(const StringRef& name);

	/** read */
	//LN_METHOD()
	String readString();

	/** read */
	//LN_METHOD()
	// beginObject, endObject のユーティリティ
	Ref<Object> readObject();



	/** sec */
	void beginWriteObject();
	void endWriteObject();
	void beginWriteList();
	void endWriteList();


	void beginReadObject();
	void endReadObject();
	bool beginReadList(int* outItemCount);
	void endReadList();

	
	/** serialize */
	//LN_METHOD()
	static String serialize(AssetModel* value, const String& basePath);
	
	/** serialize */
	//LN_METHOD()
	static Ref<AssetModel> deserialize(const String& str, const String& basePath);



	// C++ utils

	template<typename T>
	Serializer2& operator&(T&& value)
	{
		process(std::forward<T>(value));
		return *this;
	}
	

LN_CONSTRUCT_ACCESS:
	Serializer2();
	virtual ~Serializer2() = default;
	void init();

private:
	void processSave();
	void processLoad();

	template<typename T>
	void process(T&& head)
	{
		switch (m_mode)
		{
		case ArchiveMode::Save:
			processSave(head);
			break;
		case ArchiveMode::Load:
			processLoad(head);
			break;
		default:
			LN_UNREACHABLE();
			break;
		}
	}

	template<typename T>
	void processSave(NameValuePair<T>& nvp)
	{
		writeName(nvp.name);
		writeValue(*nvp.value);
	}
	template<typename T>
	void processSave(T& value)
	{
		writeValue(value);
	}
	void writeValue(bool value) { writeBool(value); }
	void writeValue(int8_t value) { writeInt8(value); }
	void writeValue(int16_t value) { writeInt16(value); }
	void writeValue(int32_t value) { writeInt32(value); }
	void writeValue(int64_t value) { writeInt64(value); }
	void writeValue(uint8_t value) { writeUInt8(value); }
	void writeValue(uint16_t value) { writeUInt16(value); }
	void writeValue(uint32_t value) { writeUInt32(value); }
	void writeValue(uint64_t value) { writeUInt64(value); }
	void writeValue(float value) { writeFloat(value); }
	void writeValue(double value) { writeDouble(value); }
	void writeValue(String& value) { writeString(value); }
	template<typename T>
	void writeValue(Ref<T>& value) { writeObject(value); }
	template<typename T>
	//void writeValue(T& value) { value.serialize2(*this); }
	void writeValue(T& value) { ln::serialize2(*this, value); }

	template<typename T>
	void processLoad(NameValuePair<T>& nvp)
	{
		if (readName(nvp.name)) {
			readValue(*nvp.value);
		}
	}
	template<typename T>
	void processLoad(T& value)
	{
		readValue(value);
	}
	void readValue(bool& outValue) { outValue = readBool(); }
	void readValue(int8_t& outValue) { outValue = readInt8(); }
	void readValue(int16_t& outValue) { outValue = readInt16(); }
	void readValue(int32_t& outValue) { outValue = readInt32(); }
	void readValue(int64_t& outValue) { outValue = readInt64(); }
	void readValue(uint8_t& outValue) { outValue = readUInt8(); }
	void readValue(uint16_t& outValue) { outValue = readUInt16(); }
	void readValue(uint32_t& outValue) { outValue = readUInt32(); }
	void readValue(uint64_t& outValue) { outValue = readUInt64(); }
	void readValue(float& outValue) { outValue = readFloat(); }
	void readValue(double& outValue) { outValue = readDouble(); }
	void readValue(String& outValue) { outValue = readString(); }
	template<typename T>
	void readValue(Ref<T>& outValue) { outValue = dynamic_pointer_cast<T>(readObject()); }
	template<typename T>
	//void readValue(T& outValue) { outValue.serialize2(*this); }
	void readValue(T& outValue) { ln::serialize2(*this, outValue); }

	ArchiveMode m_mode;
	Ref<detail::SerializerStore2> m_store;
};
#endif

template<typename T>
void serialize2(Serializer2& sr, List<T>& value)
{
	if (sr.isSaving()) {
		sr.beginWriteList();
		for (auto& v : value) {
			sr & v;
		}
		sr.endWriteList();
	}
	else {
		int size = 0;
		if (sr.beginReadList(&size)) {
			value.resize(size);
			for (auto& v : value) {
				sr & v;
			}
			sr.endReadList();
		}
	}
}
} // namespace ln
