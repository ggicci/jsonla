#include "gci-json.h"
#include <ctype.h>
#include <stdlib.h>
#include <sstream>
#include <algorithm>

namespace ggicci
{
using namespace std;

/* Static Members */
Json Json::Parse(const char* json_string)
{
	// Construct an Assist object to do the parse job.
	Assist assist(json_string);
	// Return the Json object parsed from \em json_string.
	return assist.ConsumeValue(false);
}

/* Public Members */
Json::Json()
	:kind_(Json::kNull) { data_ = 0; }

Json::Json(int num) : Json((double)num) {}

Json::Json(double num)
	:kind_(Json::kNumber) { data_ = new double(num); }

Json::Json(const string& str) : Json(str.c_str()) { }

Json::Json(const char* str)
	:kind_(Json::kString) { data_ = new string(str); }

Json::Json(bool boo)
	:kind_(Json::kBool) { data_ = new bool(boo); }

Json::Json(const Json& rhs)
{
	// Do deep copy from \em rhs.
	CopyData(rhs);
}

Json::~Json()
{
	TRACK("Json::~Json() -----------------------------------> " << *this);
	DestroyData();
}

Json::Kind Json::DataKind() const
{
	return kind_;
}

/* Data Type Detector */
bool Json::IsNumber() const { return kind_ == Json::kNumber; }
bool Json::IsString() const { return kind_ == Json::kString; }
bool Json::IsBool() const { return kind_ == Json::kBool; }
bool Json::IsNull() const { return kind_ == Json::kNull; }
bool Json::IsArray() const { return kind_ == Json::kArray; }
bool Json::IsObject() const { return kind_ == Json::kObject; }

/* Utilities for Object or Array */
bool Json::IsEmpty() const
{
	if (IsObject()) { return CAST_JSON_OBJ(data_)->size() == 0; }
	if (IsArray()) { return CAST_JSON_ARR(data_)->size() == 0; }
	return false;
}

bool Json::Contains(const char* key) const 
{
	if (!IsObject()) { return false; }
	const ObjectData &data = *CAST_JSON_OBJ(data_);
	// Find the key in the \b map, return true if found and false otherwise.
	return data.find(key) != data.end();
}

int Json::Size() const
{
	// Which means the size of a Json object at least is 1,
	// even it's null (IsNull() == true).
	if (!IsArray()) { return 1; } 
	return CAST_JSON_ARR(data_)->size();
}

vector<string> Json::Keys() const
{
	// You cant retrieve keys on a non-object Json object,
	// because an array or a number has no keys(names).
	if (!IsObject())
	{ throw IllegalOperationException(IllegalOperationException::kRetriveKeys, DataTypeName()); }
	vector<string> keys;
	ObjectData *data = CAST_JSON_OBJ(data_);
	for (ObjectData::const_iterator cit = data->begin(); cit != data->end(); ++cit)
	{
		keys.push_back(cit->first);
	}
	// Return a vector<string> object which contains all the keys.
	return keys;
}

Json& Json::Push(const Json& rhs)
{
	TRACK("Json& Json::Push(const Json& rhs)");
	switch (kind_)
	{
		case kArray:
		{
			// Simply copy it and push it to the array.
			ArrayData *data = CAST_JSON_ARR(data_);
			data->push_back(new Json(rhs));
			break;
		}
		case kNumber: case kString: case kBool: case kNull: case kObject:
		{
			// Copy the old one, and together with the new one,
			// both are to pushed into an empty array. Finally
			// got an array of size 2.
			Json *old = new Json(*this);
			DestroyData();
			kind_ = Json::kArray;
			ArrayData *tmp = new ArrayData();
			tmp->push_back(old);
			tmp->push_back(new Json(rhs));
			data_ = tmp;
			break;
		}
		default:
		{
			throw IllegalOperationException(IllegalOperationException::kPush, DataTypeName());
			break;
		}
	}
	return *this;
}

Json& Json::AddProperty(const string& key, const Json& val)
{
	TRACK("Json& Json::Add(const string& key, const Json& val)");
	if (!IsObject())
	{ throw IllegalOperationException(IllegalOperationException::kAddProperty, DataTypeName()); }
	ObjectData *data = CAST_JSON_OBJ(data_);
	// to call opeartor [] function to get a reference to the newly created Json object
	// which is mapped to the \em pair named \em key.
	(*this)[key.c_str()] = val;
	return *this;
}

Json& Json::Remove(Json* json)
{
	TRACK("Json& Json::Remove(Json* json)");
	if (IsObject())
	{
		// O(n) find operation
		ObjectData *data = CAST_JSON_OBJ(data_);
		ObjectData::iterator it = find_if(data->begin(), data->end(), ObjectDataValuePred(json));
		if (it != data->end()) { data->erase(it); } // DO NOT delete
	}
	else if (IsArray())
	{
		ArrayData *data = CAST_JSON_ARR(data_);
		ArrayData::iterator it = find(data->begin(), data->end(), json);
		if (it != data->end()) { data->erase(it); } // DO NOT delete
	}
	else
	{ throw IllegalOperationException(IllegalOperationException::kRemove, DataTypeName()); }
	return *this;
}

Json& Json::Remove(const string& key)
{
	TRACK("Json& Json::Remove(const string& key)");
	if (!IsObject())
	{ throw IllegalOperationException(IllegalOperationException::kRemove, DataTypeName()); }
	ObjectData *data = CAST_JSON_OBJ(data_);
	ObjectData::iterator it = data->find(key);
	if (it != data->end())
	{
		delete it->second;
		data->erase(it);
	}
	return *this;
}

void Json::Remove(int index)
{
	TRACK("void Json::Remove(int index)");
	if (!IsArray())
	{ throw IllegalOperationException(IllegalOperationException::kRemove, DataTypeName()); }
	ArrayData *data = CAST_JSON_ARR(data_);
	if (index >= 0 && index < Size())
	{
		ArrayData::iterator it = data->begin() + index;
		delete (*it);
		data->erase(it);
	}
}

/* Data Extractors */
Json::operator int() const
{
	if (!IsNumber())
	{ throw IllegalOperationException(IllegalOperationException::kExtract, "Number(int)", DataTypeName()); }
	return (int)*static_cast<double*>(data_);
}

Json::operator double() const
{
	if (!IsNumber())
	{ throw IllegalOperationException(IllegalOperationException::kExtract, "Number(double)", DataTypeName()); }
	return *static_cast<double*>(data_);
}

Json::operator string() const
{
	if (!IsString())
	{ throw IllegalOperationException(IllegalOperationException::kExtract, "String", DataTypeName()); }
	return *static_cast<string*>(data_);
}

Json::operator const char*() const
{
	return (operator string()).c_str();
}

Json::operator bool() const
{
	if (!IsBool())
	{ throw IllegalOperationException(IllegalOperationException::kExtract, "Bool", DataTypeName()); }
	return *static_cast<bool*>(data_);
}

const Json& Json::operator [] (int index) const
{
	if (!IsArray())
	{ throw IllegalOperationException(IllegalOperationException::kExtract, "Array Item", DataTypeName()); }
	if (index >= Size() || index < 0)
	{ throw IllegalOperationException(IllegalOperationException::kViolateAccess, "Array Item", DataTypeName()); }
	return *(CAST_JSON_ARR(data_)->at(index));
}

Json& Json::operator [] (int index)
{
	return const_cast<Json&>(static_cast<const Json&>(*this)[index]);
}

const Json& Json::operator [] (const char* key) const
{
	if (!IsObject())
	{ throw IllegalOperationException(IllegalOperationException::kExtract, "Object Item", DataTypeName()); }
	ObjectData &data = *CAST_JSON_OBJ(data_);
	if (data.find(key) == data.end()) { data[key] = new Json(); }
	return *(data[key]);
}

Json& Json::operator [] (const char* key)
{
	TRACK("Json& Json::operator [] (const char* key)");
	return const_cast<Json&>(static_cast<const Json&>(*this)[key]);
}

/* Data Setters */
Json& Json::operator = (const Json& json)
{
	TRACK("Json& Json::operator = (const Json& json)");
	if (this != &json)
	{
		DestroyData();
		CopyData(json);
	}
	return *this;
}

Json& Json::operator = (int num)
{
	return operator = ((double)num);
}

Json& Json::operator = (double num)
{
	TRACK("Json& Json::operator = (double num)");
	if (!IsNumber())
	{
		DestroyData();
		kind_ = Json::kNumber;
		data_ = new double(num);
	}
	else
	{
		*static_cast<double*>(data_) = num;
	}
	return *this;
}

Json& Json::operator = (const string& str)
{
	return (operator = (str.c_str()));
}

Json& Json::operator = (const char* str)
{
	TRACK("Json& Json::operator = (const char* str)");
	if (!IsString())
	{
		DestroyData();
		kind_ = Json::kString;
		data_ = new string(str);
	}
	else
	{
		*static_cast<string*>(data_) = str;
	}
	return *this;
}

Json& Json::operator = (bool boo)
{
	TRACK("Json& Json::operator = (bool boo)");
	if (!IsBool())
	{
		DestroyData();
		kind_ = Json::kBool;
		data_ = new bool(boo);
	}
	else
	{
		*static_cast<bool*>(data_) = boo;
	}
	return *this;
}

const char* Json::ToString() const
{
	ostringstream oss;
	switch (kind_)
	{
		case Json::kString: { oss << "\"" << *static_cast<const string*>(data_) << "\""; break; }
		case Json::kNumber: { oss << *static_cast<const double*>(data_); break; }
		case Json::kBool: { oss << boolalpha << *static_cast<bool*>(data_); break; }
		case Json::kNull: { oss << "null"; break; }
		case Json::kObject:
		{
			const Json::ObjectData& data = *CAST_JSON_OBJ(data_);
			oss << "{ ";
			bool unique = true;
			for (Json::ObjectData::const_iterator cit = data.begin(); cit != data.end(); ++cit)
			{
				oss << (unique ? "" : ", ") << "\"" << cit->first << "\": " << *cit->second;
				unique = false;
			}
			oss << " }";
			break;
		}
		case Json::kArray:
		{
			const Json::ArrayData& data = *CAST_JSON_ARR(data_);
			oss << "[ ";
			bool unique = true;
			for (Json::ArrayData::const_iterator cit = data.begin(); cit != data.end(); ++cit)
			{
				oss << (unique ? "" : ", ") << *(*cit);
				unique = false; 
			}
			oss << " ]";
			break;
		}
		default:
			break;
	}
	return oss.str().c_str();
}

/* Private Members */
Json::Json(const ArrayData& arr)
	:kind_(Json::kArray)
{
	TRACK("Json::Json(const ArrayData& arr)");
	data_ = new ArrayData(arr);
}

Json::Json(const ObjectData& obj)
	:kind_(Json::kObject)
{
	TRACK("Json::Json(const ObjectData& obj)");
	data_ = new ObjectData(obj);
}

const char* Json::DataTypeName() const
{
	switch (kind_)
	{
		case kNumber: { return "Number"; }
		case kString: { return "String"; }
		case kBool: { return "Bool"; }
		case kNull: { return "Null"; }
		case kObject: { return "Object"; }
		case kArray: { return "Array"; }
		default : { return "Unknown"; }
	}
}

void Json::CopyData(const Json& rhs)
{
	TRACK("void Json::CopyData(const Json& rhs)");
	kind_ = rhs.kind_;
	TRACK("Copying " << kind_ << ": " << rhs);
	switch (kind_)
	{
		case kNull: { data_ = 0; break; }
		case kNumber: { data_ = new double(*static_cast<double*>(rhs.data_)); break; }
		case kString: { data_ = new string(*static_cast<string*>(rhs.data_)); break; }
		case kBool: { data_ = new bool(*static_cast<bool*>(rhs.data_)); break; }
		case kObject:
		{
			ObjectData *tmp = new ObjectData();
			const ObjectData *data = CAST_JSON_OBJ(rhs.data_);
			for (ObjectData::const_iterator cit = data->begin(); cit != data->end(); ++cit)
			{
				TRACK("Copying: " << *cit->second);
				tmp->insert(make_pair(cit->first, new Json(*cit->second)));
			}
			data_ = tmp;
			break;
		}
		case kArray:
		{
			ArrayData *tmp = new ArrayData();
			const ArrayData *data = CAST_JSON_ARR(rhs.data_);
			for (ArrayData::const_iterator cit = data->begin(); cit != data->end(); ++cit)
			{
				TRACK("Copying: " << *(*cit));
				tmp->push_back(new Json(*(*cit)));
			}
			data_ = tmp;
			break;
		}
		default: { break; }
	}
	TRACK("Copy end!");
}

void Json::DestroyData()
{
	TRACK("void Json::DestroyData()");
	if (0 == data_) { return; }
	switch (kind_)
	{
		case kNumber: { delete static_cast<double*>(data_); break; }
		case kString: { delete static_cast<string*>(data_); break; }
		case kBool: { delete static_cast<bool*>(data_); break; }
		case kArray:
		{
			TRACK("delete: array");
			ArrayData *data = CAST_JSON_ARR(data_);
			for (ArrayData::iterator it = data->begin(); it != data->end(); ++it)
			{ delete *it; *it = 0; }
			delete data;
			break;
		}
		case kObject:
		{
			TRACK("delete: object");
			ObjectData *data = CAST_JSON_OBJ(data_);
			for (ObjectData::iterator it = data->begin(); it != data->end(); ++it)
			{ delete it->second; it->second = 0; }
			delete data;
			break;
		}
		default: break;
	}
	data_ = 0;
}

/* Struct Assist */
Json::Assist::Assist(const char* json_string)
{
	source = json_string;
	pos = -1;
	character = ' ';
	token = "";
}

Json Json::Assist::ConsumeValue(bool section/* = true */)
{
	TRACK("Json Json::Assist::ConsumeValue()");
	SkipWhitespaces();
	Kind kind = KindDetect();
	Json (Json::Assist::*consumer)();
	switch (kind)
	{
		case kNumber: { consumer = &Json::Assist::ConsumeNumber; break; }
		case kString: { consumer = &Json::Assist::ConsumeString; break; }
		case kBool: { consumer = &Json::Assist::ConsumeBool; break; }
		case kNull: { consumer = &Json::Assist::ConsumeNull; break; }
		case kObject: { consumer = &Json::Assist::ConsumeObject; break; }
		case kArray: { consumer = &Json::Assist::ConsumeArray; break; }
		default: { throw; break; }
	}
	Json json = (this->*consumer)();
	if (!section && !EOL())
	{
		SkipWhitespaces();
		NextCharacter();
		if (!EOL()) { UnexpectedToken(); }
	}
	if (section) { SkipWhitespaces(); }
	return json;
} // end fn:ConsumeValue

Json Json::Assist::ConsumeNumber()
{
	TRACK("Json Json::Assist::ConsumeNumber()");
	token = "";
	NextCharacter();
	// negative
	if ('-' == character) { Concat(); NextCharacter(); }
	bool loop = true;
	bool dot = false;
	if (!isdigit(character)) { UnexpectedToken(); }
	Concat(); // the first digit or after '-'
	if ('0' == character) {  loop = false; }
	while (loop) // * loop
	{
		if (!isdigit(NextCharacter())) { Retract(); break; }
		Concat();
	}
	if (isdigit(NextCharacter())) { UnexpectedToken(); } // fix 000.3
	if ('.' == character) { Concat(); dot = true; }
	if (dot) // met '.', at least need one digit
	{
		if (!isdigit(NextCharacter())) { UnexpectedToken(); }
		Concat();
	}
	while (dot) // * loop
	{
		NextCharacter();
		if (!isdigit(character)) { break; }
		Concat();
	}
	// confront with scientific notation
	if ('e' == character || 'E' == character)
	{
		Concat();
		NextCharacter();
		if ('+' == character || '-' == character) { Concat(); }
		else if (isdigit(character)) { Retract(); }
		else { UnexpectedToken(); }
		// at least need one digit after '+' or '-' or 'E' or 'e'
		if (!isdigit(NextCharacter())) { UnexpectedToken(); }
		Concat();
		while (true)
		{
			NextCharacter();
			// if (EOL() || isspace(character)) { break; }
			if (!isdigit(character)) { Retract(); break; }
			Concat();
		}
	}
	else
	{
		Retract();
	}
	// else if (EOL() || isspace(character)) { ; } 
	// else { UnexpectedToken(); } // fix -23.0s
	return Json(atof(token.c_str()));
} // end fn:ConsumeNumber

Json Json::Assist::ConsumeString()
{
	TRACK("Json Json::Assist::ConsumeString()");
	SkipWhitespaces();
	// consume the open quote
	if ('\"' != NextCharacter()) { UnexpectedToken(); }
	token = "";
	while (true)
	{
		// meet the close quote, end loop
		if ('\"' == NextCharacter()) { break; }
		// the escape characters
		if ('\\' == character)
		{
			Concat();
			NextCharacter();
			switch (character)
			{
				case '\"': case '\\': case '/': case 'b':
				case 'f': case 'n': case 'r': case 't':
				{
					Concat();
					break;
				}
				case 'u':
				{
					Concat();
					for (int i = 0; i < 4; ++i)
					{
						if (!isxdigit(NextCharacter()))
						{
							UnexpectedToken();
						}
						Concat();
					}
					break;
				}
				default:
				{
					UnexpectedToken();
					break;
				}
			}
		}
		else // if not a escape character
		{
			if (iscntrl(character)) { UnexpectedToken(); }
			Concat();
		}
	}
	return Json(token);
} // end fn:ConsumeString

Json Json::Assist::ConsumeBool()
{
	TRACK("Json Json::Assist::ConsumeBool()");
	char ch = NextCharacter();
	Retract();
	if ('t' == ch) { ConsumeSpecific("true"); return Json(true); }
	else { ConsumeSpecific("false"); return Json(false); }
} // end fn:ConsumeBool

Json Json::Assist::ConsumeNull()
{
	TRACK("Json Json::Assist::ConsumeNull()");
	ConsumeSpecific("null");
	return Json();
} // end fn:ConsumeNull

Json Json::Assist::ConsumeObject()
{
	TRACK("Json Json::Assist::ConsumeObject()");
	ObjectData obj;
	if ('{' != NextCharacter()) { UnexpectedToken(); }
	SkipWhitespaces();
	if ('}' == NextCharacter()) { ; } // empty object
	else // at least need one pair
	{
		Retract();
		try { obj.insert(ConsumePair()); }
		catch (exception& e) { DestroyObjectData(obj); UnexpectedToken(); }
		while (true) // * loop
		{
			if ('}' == NextCharacter()) { break; }
			else if (',' == character)
			{ 
				try { obj.insert(ConsumePair()); }
				catch (exception& e) { DestroyObjectData(obj); UnexpectedToken(); }
			}
			else
			{
				DestroyObjectData(obj);
				UnexpectedToken();
			}
		}
	}
	return Json(obj);
} // end fn:ConsumeObject

Json Json::Assist::ConsumeArray()
{
	TRACK("Json Json::Assist::ConsumeArray()");
	ArrayData arr;
	if ('[' != NextCharacter()) { UnexpectedToken(); }
	SkipWhitespaces();
	if (']' == NextCharacter()) { ; } // empty array
	else // at least one value
	{
		Retract();
		try { arr.push_back(new Json(ConsumeValue())); }
		catch (exception& e) { DestroyArrayData(arr); UnexpectedToken(); }
		while (true) // * loop
		{
			if (']' == NextCharacter()) { break; }
			else if (',' == character) {
				try { arr.push_back(new Json(ConsumeValue())); }
				catch(exception& e) { DestroyArrayData(arr); UnexpectedToken(); }
			}
			else
			{
				DestroyArrayData(arr);
				UnexpectedToken();
			}
		}
	}
	return Json(arr);
} // end fn: Consume Array

Json::Pair Json::Assist::ConsumePair()
{
	TRACK("Json::Assist::Pair Json::Assist::ConsumePair()");
	string key = ConsumeString();
	SkipWhitespaces();
	ConsumeSpecific(":");
	Json *value = new Json(ConsumeValue());
	return make_pair(key, value);
} // end fn:ConsumePair

void Json::Assist::ConsumeSpecific(const char* str)
{
	TRACK("void Json::Assist::ConsumeSpecific(const char* str)");
	int len = char_traits<char>::length(str);
	for (int i = 0; i < len; ++i)
	{
		if (str[i] != NextCharacter()) { UnexpectedToken(); }
	}
} // end fn:ConsumeSpeciffic

Json::Kind Json::Assist::KindDetect()
{
	NextCharacter();
	Json::Kind kind = Json::kNumber;
	switch (character)
	{
		case '\"': { kind = Json::kString; break; }
		case 't': case 'f': { kind = Json::kBool; break; }
		case 'n': { kind = Json::kNull; break; }
		case '{': { kind = Json::kObject; break; }
		case '[': { kind = Json::kArray; break; }
		default: break; // treat as number (will cause exception)
	}
	Retract();
	return kind;
}

void Json::Assist::Concat() { token += character; }
void Json::Assist::Retract() { pos--; character = ' '; }

char Json::Assist::NextCharacter()
{
	if (pos >= 0 && '\0' == *(source + pos)) { UnexpectedToken(); }
	character = *(source + (++pos));
	return character;
}

void Json::Assist::SkipWhitespaces()
{
	char ch;
	while (true)
	{
		ch = NextCharacter();
		if (' ' == ch || '\t' == ch || '\n' == ch || '\r' == ch) { ; }
		else { pos--; break; }
	}
}

bool Json::Assist::EOL() const { return '\0' == character; }
void Json::Assist::UnexpectedToken() { throw Json::UnexpectedTokenException(character, pos); }

void Json::Assist:: DestroyArrayData(ArrayData& arr)
{
	TRACK("void Json::Assist:: DestroyArrayData(ArrayData& arr)");
	for (ArrayData::iterator it = arr.begin(); it != arr.end(); ++it)
	{
		delete *it;
		*it = 0;
	}
}

void Json::Assist::DestroyObjectData(ObjectData& obj)
{
	TRACK("void Json::Assist::DestroyObjectData(ObjectData& obj)");
	for (ObjectData::iterator it = obj.begin(); it != obj.end(); ++it)
	{
		delete it->second;
		it->second = 0;
	}
}

/* Struct UnexpectedTokenException */
Json::UnexpectedTokenException::UnexpectedTokenException(char ch, int pos)
	:exception(), ch_(ch), pos_(pos) { }

const char* Json::UnexpectedTokenException::what() const throw()
{
	ostringstream oss;
	if ('\0' == ch_)
	{
		oss << "SyntaxError: Unexpected end of input";
	}
	else
	{
		oss << "SyntaxError: Unexpected token ";
		if (isgraph(ch_)) { oss << ch_; }
		else { oss << (int)ch_ << "(ASCII)"; }
		oss << " at pos " << pos_;
	}
	return oss.str().c_str();
}

/* Struct IllegalOperationException */
Json::IllegalOperationException::IllegalOperationException(OperationKind kind, const char* to, const char* from /* = 0 */)
	:exception(), kind_(kind), from_(from), to_(to) { }

const char* Json::IllegalOperationException::what() const throw()
{
	ostringstream oss;
	oss << "OperationError: Illegal ";
	switch (kind_)
	{
		case kExtract: { oss << "extract"; break; }
		case kViolateAccess: { oss << "violate access"; break; }
		case kPush: { oss << "push"; break; }
		case kAddProperty: { oss << "add property"; break; }
		case kRemove: { oss << "remove"; break; }
		case kRetriveKeys: { oss << "retrive keys"; break; }
		default : { oss << "unknown"; break; }
	}
	oss << " opeartion";
	if (kind_ == kExtract) { oss << " from " << from_ << " to " << to_; }
	else { oss << " on " << to_; }
	return oss.str().c_str();
}

/* Friend Functions */
ostream& operator << (ostream& outs, const Json& rhs)
{
	outs << rhs.ToString();
	return outs;
}

}