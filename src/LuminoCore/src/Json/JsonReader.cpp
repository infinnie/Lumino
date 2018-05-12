﻿
#include "Internal.hpp"
#include <float.h>
#include <Lumino/Base/String.hpp>
#include <Lumino/IO/StringReader.hpp>
#include <Lumino/Json/JsonReader.h>

namespace ln {
namespace tr {
namespace detail {

//==============================================================================
// PositioningTextReader

PositioningTextReader::PositioningTextReader(TextReader* innter)
{
	m_innter = innter;
	m_pos = 0;
	m_line = 0;
	m_column = 0;
	m_lastCR = false;
	//m_matched = 0;
}

int PositioningTextReader::peek()
{
	return m_innter->peek();
}

int PositioningTextReader::read()
{
	int c = m_innter->read();
	if (c >= 0) {
		advancePosition((Char)c);
	}
	return c;
}

bool PositioningTextReader::readLine(String* line)
{
	LN_NOTIMPLEMENTED();
	return false;
}

String PositioningTextReader::readToEnd()
{
	LN_NOTIMPLEMENTED();
	return String();
}

bool PositioningTextReader::isEOF()
{
	return m_innter->isEOF();
}

int PositioningTextReader::getPosition() const
{
	return m_pos;
}

int PositioningTextReader::getLineNumber() const
{
	return m_line;
}

int PositioningTextReader::getColumnNumber() const
{
	return m_column;
}

void PositioningTextReader::advancePosition(Char ch)
{
	++m_pos;
	if (m_lastCR)
	{
		m_lastCR = false;
		if (ch == '\n')
		{
			return;
		}
	}

	if (ch == '\r' || ch == '\n')
	{
		++m_line;
		m_column = 0;
		if (ch == '\r') {
			m_lastCR = true;
		}
		return;
	}

	++m_column;
}

} // namespace detail

//==============================================================================
// JsonReader

//------------------------------------------------------------------------------
JsonReader::JsonReader()
	: m_reader()
{
}

//------------------------------------------------------------------------------
JsonReader::JsonReader(const String& text)
	: JsonReader()
{
	Ref<StringReader> r(LN_NEW StringReader(text), false);
	m_reader.attach(LN_NEW detail::PositioningTextReader(r));
}

//------------------------------------------------------------------------------
JsonReader::JsonReader(TextReader* textReader)
	: JsonReader()
{
	m_reader.attach(LN_NEW detail::PositioningTextReader(textReader));
}

//------------------------------------------------------------------------------
JsonReader::~JsonReader()
{
}

//------------------------------------------------------------------------------
bool JsonReader::read()
{
	m_textCache.clear();

	bool skip;
	do
	{
		skip = false;
		switch (m_currentState.state)
		{
		case State::Start:
		case State::Property:
		case State::ArrayStart:
		case State::Array:
			return parseValue();
		case State::ObjectStart:
		case State::Object:
			return parseObject();
			break;
		case State::PostValue:
		{
			bool r = ParsePostValue(&skip);
			if (!skip) return r;
		}
		}

	} while (skip);

	return false;
}


//------------------------------------------------------------------------------
JsonToken JsonReader::getTokenType() const
{
	return m_currentToken.type;
}

//------------------------------------------------------------------------------
const String& JsonReader::getValue() const
{
	return m_value;
}

//------------------------------------------------------------------------------
const String& JsonReader::getPropertyName() const
{
	return m_currentState.propertyName;
}

//------------------------------------------------------------------------------
bool JsonReader::getBoolValue() const
{
	if (LN_REQUIRE(m_currentToken.type == JsonToken::Boolean)) return false;
	return m_valueData.m_bool;
}

//------------------------------------------------------------------------------
int32_t JsonReader::getInt32Value() const
{
	if (LN_REQUIRE(m_currentToken.type == JsonToken::Int32)) return 0;
	return m_valueData.m_int32;
}

//------------------------------------------------------------------------------
int64_t JsonReader::getInt64Value() const
{
	if (LN_REQUIRE(m_currentToken.type == JsonToken::Int64)) return 0;
	return m_valueData.m_int64;
}

//------------------------------------------------------------------------------
float JsonReader::getFloatValue() const
{
	if (LN_REQUIRE(m_currentToken.type == JsonToken::Float)) return 0;
	return m_valueData.m_float;
}

//------------------------------------------------------------------------------
double JsonReader::getDoubleValue() const
{
	if (LN_REQUIRE(m_currentToken.type == JsonToken::Double)) return 0;
	return m_valueData.m_double;
}

//------------------------------------------------------------------------------
const JsonError2& JsonReader::getError() const
{
	return m_error;
}

void JsonReader::readAsStartObject()
{
	if (!read() || getTokenType() != JsonToken::StartObject)
	{
		LN_ENSURE(0);
	}
}

void JsonReader::readAsEndObject()
{
	if (!read() || getTokenType() != JsonToken::EndObject)
	{
		LN_ENSURE(0);
	}
}

void JsonReader::readAsStartArray()
{
	if (!read() || getTokenType() != JsonToken::StartArray)
	{
		LN_ENSURE(0);
	}
}

void JsonReader::readAsEndArray()
{
	if (!read() || getTokenType() != JsonToken::EndArray)
	{
		LN_ENSURE(0);
	}
}

bool JsonReader::readAsBool()
{
	if (!read() || getTokenType() != JsonToken::Boolean)
	{
		LN_ENSURE(0);
		return false;
	}
	return getValue()[0] == 't';
}

const String& JsonReader::readAsPropertyName()
{
	if (!read() || getTokenType() != JsonToken::PropertyName)
	{
		LN_ENSURE(0);
		return String::getEmpty();
	}
	return getValue();
}

const String& JsonReader::readAsString()
{
	if (!read() || getTokenType() != JsonToken::String)
	{
		LN_ENSURE(0);
		return String::getEmpty();
	}
	return getValue();
}

//------------------------------------------------------------------------------
bool JsonReader::skipWhitespace()
{
	while (m_reader->peek() == ' ' || m_reader->peek() == '\n' || m_reader->peek() == '\r' || m_reader->peek() == '\t') {
		m_reader->read();
	}
	return !m_reader->isEOF();	// スキップした後に EOF にたどり着いたら false
}

//------------------------------------------------------------------------------
bool JsonReader::parseValue()
{
	if (!skipWhitespace()) return false;

	//while (true)
	{
		//if (m_reader->IsEOF())
		//{
		//	return false;
		//}

		Char ch = m_reader->peek();
		switch (ch)
		{
			//case ' ':
			//case '\t':
			//	m_reader->Read();	// 空白は消費するだけ
			//	break;
			case '{':
				setToken(JsonToken::StartObject);
				m_reader->read();
				return true;
			case '[':
				setToken(JsonToken::StartArray);
				m_reader->read();
				return true;
			case ']':	// 空配列
				setToken(JsonToken::EndArray);
				m_reader->read();
				return true;
			case '"':
				return parseString(false);

			case 'n':
				return parseNull();
			case 't':
				return parseTrue();
			case 'f':
				return parseFalse();
				//case 't': return ParseTrue();			// true かもしれない
				//case 'f': return ParseFalse();			// false かもしれない
				//case '"': return ParseString(false);	// 文字列かもしれない
				//case '[': return ParseArray();			// 配列かもしれない
				//case '{': return ParseObject();			// オブジェクトかもしれない
				//default: return ParseNumber();			// 数値かもしれない

			default:
				if (isdigit(ch) || ch == '-' || ch == '.')
				{
					return parseNumber();
				}
				return false;	// TODO

			
		}
	}
}

//------------------------------------------------------------------------------
bool JsonReader::parseNull()
{
	m_reader->read();	// skip 'n'
	if (m_reader->read() == 'u' &&
		m_reader->read() == 'l' &&
		m_reader->read() == 'l')
	{
		return setToken(JsonToken::Null);
	}
	else
	{
		// Error: "null" ではなかった
		setError(JsonParseError2::ValueInvalid);
		return false;
	}
}

//------------------------------------------------------------------------------
bool JsonReader::parseTrue()
{
	m_reader->read();	// skip 't'
	if (m_reader->read() == 'r' &&
		m_reader->read() == 'u' &&
		m_reader->read() == 'e')
	{
		m_valueData.m_bool = true;
		return setToken(JsonToken::Boolean, _TT("true"), 4);
	}
	else
	{
		// Error: "true" ではなかった
		setError(JsonParseError2::ValueInvalid);
		return false;
	}
}

//------------------------------------------------------------------------------
bool JsonReader::parseFalse()
{
	m_reader->read();	// skip 'f'
	if (m_reader->read() == 'a' &&
		m_reader->read() == 'l' &&
		m_reader->read() == 's' &&
		m_reader->read() == 'e')
	{
		m_valueData.m_bool = false;
		return setToken(JsonToken::Boolean, _TT("false"), 5);
	}
	else
	{
		// Error: "false" ではなかった
		setError(JsonParseError2::ValueInvalid);
		return false;
	}
}

//------------------------------------------------------------------------------
bool JsonReader::parseNumber()
{
	// 数値として使える文字を m_textCache に入れていく
	int len = 0;
	bool isDecimal = false;
	Char ch;
	while (true)
	{
		ch = m_reader->peek();	// 読むだけ。ポインタは進めない
		if (ch == '.' ||
			('0' <= ch && ch <= '9') ||
			(ch == 'e' || ch == 'E') ||
			(ch == '+' || ch == '-'))
		{
			m_textCache.add(ch);
			++len;
			m_reader->read();	// ここで1つ進める

			// 小数か？
			if (!isDecimal) isDecimal = (ch == '.');
		}
		else {
			break;				// 一致しなければポインタは進めない
		}
	}
	if (len == 0)
	{
		// Error: 数値っぽい文字が見つからなかった
		setError(JsonParseError2::UnexpectedToken);
		return false;
	}

	if (isDecimal)
	{
		const Char* str = &m_textCache[0];
		const Char* endptr = nullptr;
		NumberConversionResult result;
		double value = StringHelper::toDouble(str, len, &endptr, &result);

		if ((endptr - str) != len)	// 正常に変換できていれば、読み取った文字数が全て消費されるはず
		{
			// Error: 構文が正しくない
			setError(JsonParseError2::InvalidNumber);
			return false;
		}
		if (result == NumberConversionResult::Overflow)
		{
			// Error: オーバーフローが発生した
			setError(JsonParseError2::NumberOverflow);
			return false;
		}

		if (-FLT_MAX <= value && value <= FLT_MAX)
		{
			m_valueData.m_float = (float)value;
			return setToken(JsonToken::Float, str, len);
		}
		else
		{
			m_valueData.m_double = value;
			return setToken(JsonToken::Double, str, len);
		}
	}
	else
	{
		const Char* str = &m_textCache[0];
		const Char* endptr = nullptr;
		NumberConversionResult result;
		int64_t value = StringHelper::toInt64(str, len, 0, &endptr, &result);

		if ((endptr - str) != len)	// 正常に変換できていれば、読み取った文字数が全て消費されるはず
		{
			// Error: 構文が正しくない
			setError(JsonParseError2::InvalidNumber);
			return false;
		}
		if (result == NumberConversionResult::Overflow)
		{
			// Error: オーバーフローが発生した
			setError(JsonParseError2::NumberOverflow);
			return false;
		}

		if (INT32_MIN <= value && value <= INT32_MAX)
		{
			m_valueData.m_int32 = (int32_t)value;
			return setToken(JsonToken::Int32, str, len);
		}
		else
		{
			m_valueData.m_int64 = value;
			return setToken(JsonToken::Int64, str, len);
		}
	}

#if 0

	// 数値への変換には strtod を使用する。そのため、まずは数値扱いできる文字を全て読み取る
	m_tmpStream.Seek(0, SeekOrigin_Begin);
	int len = 0;
	Char ch;
	while (true)
	{
		ch = m_reader->Peek();	// 読むだけ。ポインタは進めない
		if (ch == '.' ||
			('0' <= ch && ch <= '9') ||
			(ch == 'e' || ch == 'E') ||
			(ch == '+' || ch == '-'))
		{
			m_tmpStream.Write(&ch, sizeof(Char));
			++len;
			m_reader->Read();	// ここで1つ進める
		}
		else {
			break;				// 一致しなければポインタは進めない
		}
	}
	if (len == 0)
	{
		// Error: 数値っぽい文字が見つからなかった
		m_error.SetError(JsonParseError::NumberInvalid, m_currentCharCount);
		return false;
	}
	ch = '\0';
	m_tmpStream.Write(&ch, sizeof(Char));	// 終端 \0

	// double へ変換する
	Char* str = (TCHAR*)m_tmpStream.GetBuffer();
	const Char* endptr = NULL;
	NumberConversionResult result;
	double value = StringHelper::ToDouble(str, len, &endptr, &result);
	if ((endptr - str) != len)	// 正常に変換できていれば、読み取った文字数が全て消費されるはず
	{
		// Error: 構文が正しくない
		m_error.SetError(JsonParseError::NumberInvalid, m_currentCharCount);
		return false;
	}
	if (result == NumberConversionResult::Overflow) {
		// Error: オーバーフローが発生した
		m_error.SetError(JsonParseError::NumberOverflow, m_currentCharCount);
		return false;
	}

	// Handler に通知する
	if (!m_handler->OnDouble(value))
	{
		// 中断
		m_error.SetError(JsonParseError::Termination, m_currentCharCount);
		return false;
	}
	return true;
#endif
}

//------------------------------------------------------------------------------
bool JsonReader::parseObject()
{
	if (!skipWhitespace()) return false;

	switch (m_reader->peek())
	{
	case '}':
		setToken(JsonToken::EndObject);
		m_reader->read();
		return true;
	default:
		return tryParsePropertyName();
	}
}

//------------------------------------------------------------------------------
bool JsonReader::tryParsePropertyName()
{
	/*
		::member = string name-separator value
		パース時点ではプロパティ名の空文字を許している。
	*/
	if (m_reader->peek() == '"')
	{
		if (!parseString(true))
		{
			// Error: " から始まる文字列ではなかった
			// Invalid property identifier character:
			return false;
		}

		if (!skipWhitespace()) {
			// Error: EOF した
			return false;
		}

		if (m_reader->peek() == ':') {
			// Invalid character after parsing property name. Expected ':'
		}
		m_reader->read();

		if (!m_textCache.isEmpty()) {
			setToken(JsonToken::PropertyName, &m_textCache[0], m_textCache.size());
		}
		else {
			setToken(JsonToken::PropertyName);
		}

		return true;
	}
	else
	{
		return false;
	}

}

//------------------------------------------------------------------------------
bool JsonReader::parseString(bool isKey)
{
	/*
		 string = quotation-mark *char quotation-mark

         char = unescaped /
                escape (
                    %x22 /          ; "    quotation mark  U+0022
                    %x5C /          ; \    reverse solidus U+005C
                    %x2F /          ; /    solidus         U+002F
                    %x62 /          ; b    backspace       U+0008
                    %x66 /          ; f    form feed       U+000C
                    %x6E /          ; n    line feed       U+000A
                    %x72 /          ; r    carriage return U+000D
                    %x74 /          ; t    tab             U+0009
                    %x75 4HEXDIG )  ; uXXXX                U+XXXX

         escape = %x5C              ; \

         quotation-mark = %x22      ; "

         unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
	*/
	// http://json.org/json-ja.html
	static const Char escapeTable[256] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, '\"', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '/',
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '\\', 0, 0, 0,
		0, 0, '\b', 0, 0, 0, '\f', 0, 0, 0, 0, 0, 0, 0, '\n', 0,
		0, 0, '\r', 0, '\t', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

	m_reader->read();	// skip '"'
	while (true)
	{
		Char c = m_reader->peek();

		// エスケープシーケンス
		if (c == '\\')
		{
			m_reader->read();	// skip '\'
			Char esc = m_reader->read();
			// 基本的なエスケープ
			if (unsigned(esc) < 256 && escapeTable[(unsigned char)esc])
			{
				m_textCache.add(escapeTable[(unsigned char)esc]);
			}
			// Unicode エスケープ
			else if (esc == 'u')
			{
				// TODO: 未実装
				LN_NOTIMPLEMENTED();
				return false;
			}
			else
			{
				// 無効なエスケープ
				setError(JsonParseError2::InvalidStringEscape);
				return false;
			}
		}
		// 文字列終端
		else if (c == '"')
		{
			m_reader->read();	// skip '"'
			break;
		}
		// 文字列の途中でバッファが切れた
		else if (m_reader->isEOF() || c == '\0')
		{
			// " が一致しなかった
			setError(JsonParseError2::UnterminatedString);
			return false;
		}
		// 0x20 未満の制御文字は使えない
		else if ((unsigned)c < 0x20)
		{
			setError(JsonParseError2::InvalidStringChar);
			return false;
		}
		// 普通の文字
		else
		{
			m_textCache.add(c);
			m_reader->read();
		}
	}

	if (isKey)
	{
	}
	else
	{
		if (!m_textCache.isEmpty()) {
			setToken(JsonToken::String, &m_textCache[0], m_textCache.size());
		}
		else {
			setToken(JsonToken::String);
		}
	}

	//// Handler に通知
	//bool cont = false;
	//if (isKey) {
	//	cont = m_handler->OnKey((Char*)m_tmpStream.getBuffer(), ((int)m_tmpStream.GetPosition()) / sizeof(TCHAR));
	//}
	//else {
	//	cont = m_handler->OnString((Char*)m_tmpStream.getBuffer(), ((int)m_tmpStream.GetPosition()) / sizeof(TCHAR));
	//}
	//if (!cont)
	//{
	//	// 中断
	//	m_error.SetError(JsonParseError::Termination, m_currentCharCount);
	//	return false;
	//}
	return true;
}

//------------------------------------------------------------------------------
bool JsonReader::ParsePostValue(bool* outSkip)
{
	*outSkip = false;
	if (!skipWhitespace()) return false;

	switch (m_reader->peek())
	{
	case '}':
		setToken(JsonToken::EndObject);
		m_reader->read();
		return true;
	case ']':
		setToken(JsonToken::EndArray);
		m_reader->read();
		return true;
	case ',':
		*outSkip = true;	// 今回の , は Read() で返さず、次のトークンを読みに行く
		if (m_currentState.containerType == ContainerType::Object) {
			m_currentState.state = State::Object;
		}
		else if (m_currentState.containerType == ContainerType::Array) {
			m_currentState.state = State::Array;
		}
		m_reader->read();
		return true;
	default:
		return false;	// Error
	}
}

//------------------------------------------------------------------------------
// 現在位置の状態を newToken にする
//------------------------------------------------------------------------------
bool JsonReader::setToken(JsonToken newToken, const Char* value, int valueLen)
{
	m_currentToken.type = newToken;
	//m_currentToken.valuePos = valuePos;
	//m_currentToken.valueLen = valueLen;
	if (value != nullptr && valueLen > 0)
	{
		m_value = String(value, valueLen/*&m_textCache[0], m_textCache.GetCount()*/);
	}
	else
	{
		m_value = String::getEmpty();
	}

	switch (m_currentToken.type)
	{
		//case State::Start:
		//	break;
		case JsonToken::StartObject:
			m_currentState.state = State::ObjectStart;
			pushState();
			m_currentState.containerType = ContainerType::Object;
			break;
		case JsonToken::EndObject:
			if (m_currentState.state == State::ObjectStart ||
				m_currentState.state == State::PostValue)
			{
				popState();
				m_currentState.state = State::PostValue;
			}
			else
			{
				setError(JsonParseError2::InvalidObjectClosing);
				return false;
			}
			break;
		case JsonToken::StartArray:
			m_currentState.state = State::ArrayStart;
			pushState();
			m_currentState.containerType = ContainerType::Array;
			break;
		case JsonToken::EndArray:
			// ↓ EndObject と同じ処理だから関数化してもいいかも。Json.NET では ValidateEnd()
			if (m_currentState.state == State::ArrayStart ||
				m_currentState.state == State::PostValue)
			{
				popState();
				m_currentState.state = State::PostValue;
			}
			else
			{
				setError(JsonParseError2::ArrayInvalidClosing);
				return false;
			}
			break;
		case JsonToken::PropertyName:
			m_currentState.state = State::Property;	// : まで読んでいる。次は値がほしい
			m_currentState.propertyName = m_value;
			break;
		case JsonToken::Int32:
		case JsonToken::Int64:
		case JsonToken::Float:
		case JsonToken::Double:
		case JsonToken::Null:
		case JsonToken::Boolean:
		case JsonToken::String:
			m_currentState.state = State::PostValue;
			break;
		default:
			setError(JsonParseError2::UnexpectedToken);
			return false;
	}
	return true;
}

//------------------------------------------------------------------------------
void JsonReader::pushState(/*ContainerType containerType*/)
{
	m_stateStack.push(m_currentState);
}

//------------------------------------------------------------------------------
void JsonReader::popState()
{
	m_currentState = m_stateStack.top();
	m_stateStack.pop();
}

//------------------------------------------------------------------------------
void JsonReader::setError(JsonParseError2 code, const String& message)
{
	m_error.code = code;
	m_error.message = message;
	m_error.line = m_reader->getLineNumber();
	m_error.column = m_reader->getColumnNumber();
}

} // namespace tr
} // namespace ln
