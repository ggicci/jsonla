#ifndef GGICCI_JSON_H_
#define GGICCI_JSON_H_ 

#define _DEBUG
#ifdef _DEBUG
#define TRACK(DESC) do {std::cout << "--> " << DESC << std::endl;} while (0);
#endif
#ifdef NDEBUG
#define TRACK(DESC) 
#endif

#include <iostream>
#include <typeinfo>
#include <string>
#include <vector>
#include <map>

namespace ggicci
{

	class Json
	{
	public:
		static Json Parse(const char* json_string);
		enum Kind
		{
			kNumber,
			kString,
			kBool,
			kNull,
			kObject,
			kArray
		};
		Json();
		Json(int num);
		Json(double num);
		Json(const std::string& str);
		Json(const char* str);
		Json(bool boo);
		Json(const Json& rhs);
		Json& operator = (const Json& rhs);

		virtual ~Json();

		Kind DataKind() const { return kind_; }
		bool IsNumber() const { return kind_ == kNumber; }
		bool IsString() const { return kind_ == kString; }
		bool IsBool() const { return kind_ == kBool; }
		bool IsNull() const { return !data_; }
		bool IsArray() const { return kind_ == kArray; }
		bool IsObject() const { return kind_ == kObject; }

		bool IsEmpty() const;
		bool Contains(const char* key) const;
		int Size() const;
		std::vector<std::string> Keys() const;
		Json& Push(const Json& rhs);
		Json& AddProperty(const std::string& key, const Json& value);
		Json& Remove(const std::string& key);
		void Remove(int index);

		operator int () const;
		operator double() const;
		operator bool() const;
		operator const char* () const;

		const Json& operator[] (int index) const;
		Json& operator[] (int index);

		const Json& operator[] (const char* key) const;
		Json& operator[] (const char* key);

		Json& operator = (int num);
		Json& operator = (double num);
		Json& operator = (const std::string& str);
		Json& operator = (const char* str);
		Json& operator = (bool boo);

		std::string ToString() const;

	private:
		#define CAST_JSON_OBJ(DATA) (static_cast<ObjectData*>(DATA))
		#define CAST_JSON_ARR(DATA) (static_cast<ArrayData*>(DATA))

		typedef std::vector<Json*> ArrayData;
		typedef std::map<std::string, Json*> ObjectData;
		typedef std::pair<std::string, Json*> Pair;

		struct Parser
		{
			const char* source;
			int			pos;
			unsigned char		character;
			std::string	token;

			Parser(const char* json_string);

			Json* ConsumeValue(bool section = true);
			Json* ConsumeNumber();
			Json* ConsumeString();
			Json* ConsumeBool();
			Json* ConsumeNull();
			Json* ConsumeObject();
			Json* ConsumeArray();
			Json::Pair ConsumePair();
			void ConsumeSpecific(const char* str);

			Json::Kind KindDetect();

			void Concat() { token += character; }
			void Retract() { pos--; character = ' '; }
			bool EOL() const { return '\0' == character; }
			char NextCharacter()
			{
				if (pos >= 0 && '\0' == *(source + pos)) { UnexpectedToken(); }
				character = *(source + (++pos));
				return character;
			}
			void SkipWhitespaces();
			void UnexpectedToken();
		};

		struct UnexpectedTokenException : std::exception
		{
		public:
			UnexpectedTokenException(char ch, int pos);
			const char* what() const throw();
		private:
			char 	ch_;
			int 	pos_;
		};

		struct BadConversionException : std::exception
		{
		public:
			const char* what() const throw() { return "a bad conversion"; }
		};

		static void DestroyArrayData(ArrayData& arr);
		static void DestroyObjectData(ObjectData& obj);

		// Construct from a pointer
		Json(Json* rhs);
		Json(ObjectData* obj);
		Json(ArrayData* arr);

		void DoDeepCopy(const Json& rhs);
		void Release();

		template <typename ToType>
		ToType& Data()
		{
			return const_cast<ToType&>(static_cast<const Json&>(*this).Data<ToType>());
		}

		template <typename ToType>
		const ToType& Data() const
		{
			bool okay;
			switch(kind_)
			{
			case kNumber: okay = (typeid(double) == typeid(ToType)); break;
			case kString: okay = (typeid(std::string) == typeid(ToType)); break;
			case kBool: okay = (typeid(bool) == typeid(ToType)); break;
			case kArray: okay = (typeid(ArrayData) == typeid(ToType)); break;
			case kObject: okay = (typeid(ObjectData) == typeid(ToType)); break;
			default: okay = false;
			}
			if (!okay) throw BadConversionException();
			return *static_cast<ToType*>(data_);
		}
		
		Kind kind_;
		void *data_;
	};

}

#endif // GGICCI_JSON_H_