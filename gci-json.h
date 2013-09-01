#ifndef GGICCI_JSON_H_
#define GGICCI_JSON_H_ 

#define NDEBUG
#ifdef _DEBUG
 #define TRACK(DESC) do {std::cout << "--> " << DESC << std::endl;} while (0);
#endif
#ifdef NDEBUG
 #define TRACK(DESC) 
#endif

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace ggicci
{
/**
 * \brief A Json Parser and Manipulator.
 * \details This class is used to parse a json structural string. After parsing,
 * 			you can easily access and modify the data in simple statements through
 * 			the Json object. Feel free to contact me to help my developing this
 * 			library. And you can fetch the source code from Github
 * 			<a href="https://github.com/ggicci/ggicci--json" title="ggicci::Json on github">here</a> or
 * 			my personal site <a href="http://ggicci.me/works/json/" title=
 * 			"ggicci::Json on ggicci.me">here</a>. If just for usage, you can ignore
 * 			the inner classes(structs) in Json this document presents.
 * \author Ggicci <ggicci@163.com>
 * \version 1.0
 * \date 2013.08.26
 * \bug Maybe there exists potential memory leaks.
 * 		Please be sure to tell me if you find some bugs.
 * 		And I'll be very appreciated :) After all, this is my \b first repo on Github.
 *   	And please give me some suggestions on optimizing this small library.
 * \warning Some of the functions may throw exceptions if there's a parse error
 * 			or bad opeartion.
 * \copyright GPLv3
 */
class Json
{
public:
	/**
	 * \brief Enumerate all kinds of the <a href="http://www.json.org/">json structure</a>.
	 */
	enum Kind
	{ 
		kNumber,	///< a number, eg. 2013, 12.45, 3.3e+12, -12E-5
		kString, 	///< a string, eg. "hello world"
		kBool,    	///< a boolean, true or false
		kNull,		///< null
		kObject, 	///< an object, eg. {"id": 1931, "name": "Ggicci"}
		kArray		///< an array, eg. [ 1, 2, null, {"love": true} ]
	};

	/**
	 * \brief Parse a json structural string to get a Json object.
	 *
	 * \note It may throw an exception when parsing failed. The std::exception::what() message
	 * tells you the failed reason.
	 * @param  json_string json structural string
	 * @return             a Json instance
	 *
	 * \code{.cpp}
	 * try
	 * {
	 * 	Json json = Json::Parse("{ \"year\": 2013, month: 8 }"); // cause exception
	 * }
	 * catch (exception& e)
	 * {
	 * 	cout << e.what() << endl;
	 * 	// output:
	 * 	// SyntaxError: Unexpected token m at pos 16
	 * }
	 * \endcode
	 */
	static Json Parse(const char* json_string);

	/**
	 * \brief Get a Json object represents null.
	 * 
	 * \code{.cpp}
	 * Json json;
	 * Json another = Json::Parse("null");
	 * json.IsNull(); // true
	 * another.IsNull(); // true
	 * \endcode
	 */
	Json();

	/**
	 * \brief Get a Json object from \b int.
	 */
	explicit Json(int num);

	/**
	 * \brief Get a Json object from \b double.
	 */
	explicit Json(double num);

	/**
	 * \brief Get a Json object from \b string.
	 * \note the string \em str will not be parsed.
	 */
	explicit Json(const std::string& str);

	/**
	 * \brief Get a Json object from \b string.
	 * \note the string \em str will not be parsed.
	 */
	explicit Json(const char* str);

	/**
	 * \brief Get a Json object from \b bool.
	 */
	explicit Json(bool boo);

	/**
	 * \brief Deep copy constructor.
	 * @param rhs another Json object.
	 */
	Json(const Json& rhs);

	/**
	 * \brief Destructor to delete a Json object.
	 */
	virtual ~Json();

	/**
	 * \brief Get the enum value of this Json object, which indicates
	 * the type of the data it holds.
	 * @return an enum value
	 * 
	 * \code{.cpp}
	 * Json json = Json::Parse("[1, 2, 3, 4]");
	 * json.IsArray(); // true
	 * json.DataKind() == Json::kArray; // true
	 * \endcode
	 */
	Kind DataKind() const;

	/**
	 * \brief Test whether this Json object represents a number.
	 * @return true means it's a number
	 */
	bool IsNumber() const;

	/**
	 * \brief Test whether this Json object represents a string.
	 * @return true means it's a string
	 */
	bool IsString() const;

	/**
	 * \brief Test whether this Json object represents a bool.
	 * @return true means it's a bool (its value may be true or false)
	 */
	bool IsBool() const;

	/**
	 * \brief Test whether this Json object represents null.
	 * \note Distinguish with IsEmpty()
	 * \see Json()
	 * @return true means it's null
	 */
	bool IsNull() const;

	/**
	 * \brief Test whether this Json object represents an array.
	 * @return true means it's an array
	 */
	bool IsArray() const;

	/**
	 * \brief Test whether this Json object represents an object.
	 * @return true means it's an object
	 */
	bool IsObject() const;

	/**
	 * \brief Test whether an object or array holds values.
	 * @return true if no values in an object or array
	 * 
	 * \code{.cpp}
	 * Json empty_object = Json::Parse("{}");
	 * Json empty_array = Json::Parse("[]");
	 * empty_object.IsEmpty(); // true
	 * empty_array.IsEmpty(); // true
	 * empty_object.IsNull; // false
	 * \endcode
	 */
	bool IsEmpty() const;

	/**
	 * \brief Test if an object Json object has a KVP(key-value pair) named \em key.
	 * 
	 * An object Json object means this Json object represents an object "{...}".
	 * \note if this Json object does not represent an object (IsObject() == false),
	 * 		 it always returns false.
	 * 
	 * @param  key the key(name) of a KVP
	 * @return \b true if IsObject() == true and it contains that KVP, otherwise \b false
	 * 
	 * \code {.cpp}
	 * Json object = Json::Parse("{\"name\": \"Ggicci\", \"age\": 21}");
	 * object.Contains("name"); // true
	 * object.Contains("sex"); // false
	 * object = 3; // will delete the data of object and 
	 *             // allocate new memory for object to 
	 *             // hold value (3). and now, object is
	 *             // a number
	 * object.Contains("hello"); // false
	 * \endcode
	 */
	bool Contains(const char* key) const;

	/**
	 * Indicate the size of the array.
	 * @return size of the array, or 1 if it is not an array.
	 * 
	 * \code{.cpp}
	 * Json json = Json::Parse("[1, 2, 3, 4]");
	 * json.Size(); // 4
	 * json.Add(5).Add(6).Size(); // 6
	 * json = 0; // 1
	 * (json = "Ggicci").Size(); // 1
	 * \endcode
	 */
	int Size() const; // size of array
	
	/**
	 * \brief Get all the keys of the KVPs the object Json object holds.
	 *
	 * If this Json object represents an object (parsed from "{...}"), it
	 * will have some KVPs (or maybe none), then use this function you can
	 * retrieve all the keys (names) in a std::vector.
	 * \note An exception will be thrown to indicate you that a bad operation,
	 * 	     when you try to retrive keys on an array, a number, or some else
	 * 	     which is not an object Json object.
	 * @return a std::vector contains all the keys
	 * 
	 * <b>Example 1:</b>
	 * \code{.cpp}
	 * Json json = Json::Parse("{\"id\":1234, \"name\": \"Ggicci\", \"birthday\": [1991, 11, 10]}");
	 * vector<string> keys = json.Keys();
	 * for (vector<string>::const_iterator cit = keys.begin(); cit != keys.end(); ++cit)
	 * {
	 * 	cout << *cit << ": " << json[cit->c_str()] << endl;
	 * }
	 * \endcode
	 * \b Output:
	 * \code{.txt}
	 * birthday: [ 1991, 11, 10 ]
	 * id: 1234
	 * name: "Ggicci"
	 * \endcode
	 * <b>Example 2:</b>
	 * \code{.cpp}
	 * try
	 * {
	 * 	Json json = Json::Parse("[1991, 11, 10, \"Ggicci\"]");
	 * 	json.Keys(); // cause exception
	 * }
	 * catch (exception &e)
	 * {
	 * 	cout << e.what() << endl;
	 * 	// output:
	 * 	// OperationError: Illegal retrive keys opeartion on Array
	 * }
	 * \endcode
	 */
	std::vector<std::string> Keys() const; // get all keys of an object

	/**
	 * \brief Push a Json object to the current Json object (finally an array).
	 *
	 * If current Json object is a number, string, bool, null or object, it will be
	 * converted to an array first and then push back \em rhs to this array.
	 * If current Json object is an array, just simply push back \em rhs.
	 * Otherwise, an exception will be thrown.
	 * \note An exception may be thrown to indicate that it's a bad operation.
	 * @param  rhs the Json object to push
	 * @return     The Json object finally got
	 * 
	 * <b>Example 1:</b>
	 * \code{.cpp}
	 * Json json = 10;
	 * cout << json << endl; // 10
	 * json.Push("fuck").Push("{Here will not be parsed}").Push(true);
	 * cout << json << endl;
	 * // [ 10, "fuck", "{Here will not be parsed}", true ]
	 * \endcode
	 */
	Json& Push(const Json& rhs);

	/**
	 * \brief AddProperty a Json object to the current Json object (finally an object).
	 *
	 * If this Json object doesn't represent an object (IsObject() == false), an exception
	 * will be thrown. Otherwise, a KVP(key-value pair) will be insert into this object
	 * Json object, which consists of \em key and \em val. The function returns the 
	 * referece to this object, so you can call AddProperty function in a cascade way.
	 * \note An exception may be thrown to indicate that it's a bad operation.
	 * @param  key the key(name) of the KVP
	 * @param  val the value of the KVP
	 * @return      the Json object finally got
	 *
	 * <b>Example 1:</b>
	 * \code{.cpp}
	 * Json json = Json::Parse("{}");
	 * json.AddProperty("name", "Ggicci");
	 * json.AddProperty("characteristics", Json::Parse("[\"optimitic\", \"sympathetic\"]"));
	 * cout << json << endl;
	 * // output:
	 * // { "characteristics": [ "optimitic", "sympathetic" ], "name": "Ggicci" }
	 * \endcode
	 * 
	 * <b>Example 2:</b>
	 * \code{.cpp}
	 * try
	 * {
	 * 	Json json = Json::Parse("[1, 2, 3]"); 
	 * 	json.AddProperty("name", "Ggicci"); // cause exception
	 * } 
	 * catch (exception &e) 
	 * { 
	 * 	cout << e.what() << endl;
	 * 	// output:
	 * 	// OperationError: Illegal add opeartion on Array
	 * }
	 * \endcode
	 */
	Json& AddProperty(const std::string& key, const Json& val);

	/**
	 * \brief Remove a Json object from an array or object.
	 *
	 * Find the \em json object in current Json object. And if it
	 * is found, it will be removed from this Json object. <b>However,
	 * it will not be deleted in memory.</b>
	 * \note The Json object \em json points to will not be deleted
	 * in memory. And an exception will be thrown when you operate
	 * on a number, a string, a bool or null.
	 * @param  json a pointer to the object which needs to be removed
	 * @return      the final Json object of current
	 *
	 * <b>Example</b>
	 * \code{.cpp}
	 * Json json = Json::Parse("{\"id\":1234, \"name\": \"Ggicci\", \"birthday\": [1991, 11, 10]}");
	 * Json &birthday = json["birthday"];
	 * cout << "json = " << json << endl;
	 * cout << "birthday = " << birthday << endl;
	 * json.Remove(&birthday);
	 * cout << "json(after) = " << json << endl;
	 * cout << "birthday(after) = " << birthday << endl;
	 * delete &birthday; // DO NOT forget to delete the memory
	 * \endcode
	 * <b>Output:</b>
	 * \code{.txt}
	 * json = { "birthday": [ 1991, 11, 10 ], "id": 1234, "name": "Ggicci" }
	 * birthday = [ 1991, 11, 10 ]
	 * json(after) = { "id": 1234, "name": "Ggicci" }
	 * birthday(after) = [ 1991, 11, 10 ]
	 * \endcode
	 */
	Json& Remove(Json* json);

	/**
	 * \brief Remove a specefied KVP(key-value pair) from an object Json object by key(name).
	 *
	 * Find the KVP named \em key and remove it from this current Json object. Simultaneously,
	 * the memory will be deallocated. And nothing will happen if not found.
	 * \note An exception will be thrown when you try to operate on non-object Json object.
	 * @param  key the key(name) of the KVP
	 * @return     the Json object after removing the specified KVP
	 *
	 * \code{.cpp}
	 * Json json = Json::Parse("{\"color\": \"green\", \"weight\": 12.4}");
	 * cout << "json = " << json << endl;
	 * // output:
	 * // json = { "color": "green", "weight": 12.4 }
	 * cout << "json(after) = " << json.Remove("color").Remove("not found") << endl;
	 * // output:
	 * // json(after) = { "weight": 12.4 }
	 * \endcode
	 */
	Json& Remove(const std::string& key);

	/**
	 * \brief Remove an item from an array by the specified index
	 *
	 * If the index out of range [0, Size()-1], the specified item of this array
	 * will be removed and deleted in memory.
	 * \note This function will not return the Json object.
	 * @param index the index of the array
	 *
	 * \code{.cpp}
	 * Json json = Json::Parse("[1, 2, true, \"Hello World\"]");
	 * json.Remove(1);
	 * cout << json << endl;
	 * // output:
	 * // [ 1, true, "Hello World" ]
	 * \endcode
	 */
	void Remove(int index);
	
	/**
	 * \brief Extract the data from number Json object and return it as \b int.
	 * \note Exception when Json object is not a number.
	 */
	operator int() const;

	/**
	 * \brief Extract the data from number Json object and return it as \b double.
	 * \note Exception when Json object is not a number.
	 */
	operator double() const;

	/**
	 * \brief Extract the data from string Json object and return it as \b std::string.
	 * \note Exception when Json object is not a string.
	 */
	operator std::string() const;

	/**
	 * \brief Extract the data from string Json object and return it as \b const char*.
	 * \note Exception when Json object is not a string.
	 */
	operator const char*() const;

	/**
	 * \brief Extract the data from bool Json object and return it as \b bool.
	 * \note Exception when Json object is not a bool.
	 */
	operator bool() const;

	/**
	 * \brief Extract the item data from an array. Return the reference.
	 * \note Exception when Json object is not an array.
	 */
	const Json& operator [] (int index) const;

	/**
	 * \brief Extract the item data from an array. Return the reference.
	 * \note Exception when Json object is not an array.
	 * \code{.cpp}
	 * Json json = Json::Parse("[1, 2, 3 ,4]");
	 * json[1] = Json("hello"); // [1, "hello", 3, 4]
	 * \endcode
	 */
	Json& operator [] (int index);

	/**
	 * \brief Extract the item data from an object by specified a key(name).
	 * Return the reference.
	 * \note Exception when Json object is not an object.
	 */
	const Json& operator [] (const char* key) const;

	/**
	 * \brief Extract the item data from an object by specified a key(name).
	 * Return the reference.
	 * \note Exception when Json object is not an object.
	 * \code{.cpp}
	 * Json json = Json::Parse("{\"author\": \"Ggicci\", \"sex\": 0}");
	 * json["sex"] = 1; // { "author": "Ggicci", "sex": 1 }
	 * \endcode
	 */
	Json& operator [] (const char* key);

	/**
	 * \brief Assignment, DO deep copy.
	 *
	 * Copy everything from \em json to this Json object. And the old memory will be
	 * changed or just deallocated. The current Json object will hold a copy of the
	 * values of \em json.
	 */
	Json& operator = (const Json& json);

	/**
	 * \brief Assignment from \b int, finally become a number.
	 */
	Json& operator = (int num);

	/**
	 * \brief Assignment from \b double, finally become a number.
	 */
	Json& operator = (double num);

	/**
	 * \brief Assignment from \b std::string, finally become a string.
	 * \note \en str will not be parsed.
	 */
	Json& operator = (const std::string& str);

	/**
	 * \brief Assignment from \b const char*, finally become a string.
	 * \note \em str will not be parsed.
	 */
	Json& operator = (const char* str);

	/**
	 * \brief Assignment from \b bool, finally become a bool.
	 */
	Json& operator = (bool boo);

	/**
	 * \brief Get the json structural string of this Json object.
	 *
	 * For example, if this Json object represents an object, and it has
	 * a KVP named "id" and its value is 194024, then you will get a json
	 * string like this: <b>{ "id": 194024 }</b>.
	 * @return the json structural string
	 */
	const char* ToString() const;

private:
	#define CAST_JSON_OBJ(DATA) (static_cast<map<string, Json*>*>(DATA))
	#define CAST_JSON_ARR(DATA) (static_cast<vector<Json*>*>(DATA))
	typedef std::vector<Json*> ArrayData;
	typedef std::map<std::string, Json*> ObjectData;
	typedef std::pair<std::string, Json*> Pair;

	/**
	 * \brief A nested struct who does the real parsing job.
	 * 
	 * Firstly, I want to use static functions for doing
	 * the same job. However, it is annoying to pass arguments
	 * between different functions in order to keep the library
	 * \b thread-safe.
	 */
	struct Assist
	{
		const char* source; 	///< the json structural string need to be parsed
		int 		pos; 		///< current position(index) of the character in \em source
		char 		character;	///< current character scanned at
		std::string token;		///< appear as a word, finally it will be parsed to correspoding
								///< data structure
								///< ~~~
								///< +---------+---+----------+---+---------+---+
								///< | source  | [ | 112.4e+3 | , | "hello" | ] |
								///< +---------+---+----------+---+---------+---+
								///< | tokens  |   |  token   |   |  token  |   |
								///< +---------+---+----------+---+---------+---+
								///< ~~~

		/**
		 * Constructor
		 */
		Assist(const char* json_string);

		/**
		 * \brief Parse a \b Value.
		 *
		 * Value is a general term. A value maybe a string, number, object, array, bool, null.
		 * ![value](value.gif "value")
		 * \note All the consume functions will throw an exception when syntax error occurs
		 * 		 in the \en source string.
		 * @param  section whether global, it will check the end of the input when global
		 * @return         the Json object parsed from \em source (maybe from substring)
		 */
		Json ConsumeValue(bool section = true);

		/**
		 * \brief Parse a \b number.
		 * 
		 * ![number](number.gif "number")
		 * @return the number Json object parsed
		 */
		Json ConsumeNumber();

		/**
		 * \brief Parse a \b string.
		 * 
		 * ![string](string.gif "string")
		 * @return the string Json object parsed
		 */
		Json ConsumeString();

		/**
		 * \brief Parse a \b bool(true or false).
		 * @return the bool Json object parsed
		 */
		Json ConsumeBool();

		/**
		 * \brief Parse a \b null.
		 * @return the null Json object parsed
		 */
		Json ConsumeNull();

		/**
		 * \brief Parse an \b object.
		 * 
		 * ![object](object.gif "object")
		 * @return the object Json object parsed
		 */
		Json ConsumeObject();

		/**
		 * \brief Parse an \b array.
		 * 
		 * ![array](array.gif "array")
		 * @return the array Json object parsed
		 */
		Json ConsumeArray();

		/**
		 * \brief Parse a \b pair of an \b object.
		 * 
		 * Object of a Json consists of zero or more pairs. Ref :
		 * <a href="www.json.org">json.org</a>
		 * ![object](object.gif "object")
		 * @return the pair parsed
		 */
		Json::Pair ConsumePair();

		/**
		 * \brief Consume a specified string in the \em source.
		 * \note If the string not found in \em source, it will throw an exception
		 * 		 to indicate a parse error.
		 * @param str the spcified string to be cosumed in \em source
		 */
		void ConsumeSpecific(const char* str);

		/**
		 * \brief Detect the whole \em source string represents what, a string or a number or something else.
		 * @return the Kind enum value indicates the data type
		 */
		Json::Kind KindDetect(); 

		/**
		 * \brief Concat the current \em character to \em token.
		 */
		void Concat();

		/**
		 * \brief Scan backward a step(one character distance).
		 */
		void Retract();

		/**
		 * \brief Whether it is the end-of-line.
		 * @return true if EOL.
		 */
		bool EOL() const;

		/**
		 * \brief Scan forward a step(one character distance).
		 * @return the character after current \em position, i.e. the character after steping forward
		 */
		char NextCharacter();

		/**
		 * \brief Skip all the white spcaes(' ', \t, \r, \n) from \em position.
		 */
		void SkipWhitespaces();

		/**
		 * \brief Throw UnexpectedTokenException to indicate an unexpected token in \em source.
		 */
		void UnexpectedToken();

		// Updated: 2013.09.01 - by Ggicci
		void DestroyArrayData(ArrayData& arr);
		void DestroyObjectData(ObjectData& obj);
	};

	/**
	 * \brief Exception indicates syntax error of \em source.
	 */
	struct UnexpectedTokenException : std::exception
	{
	public:
		UnexpectedTokenException(char ch, int pos);
		const char* what() const throw();
	private:
		char 	ch_;  ///< which character cause syntax error
		int 	pos_; ///< where this character locates
	};
	
	/**
	 * \brief Exception indicates illegal operation on a wrong type of Json object.
	 */
	struct IllegalOperationException : std::exception
	{
	public:
		enum OperationKind
		{
			kExtract, 		///< Extract data from a Json object through keywords(or type) overloading functions.
			kViolateAccess,	///< Maybe array out of bounds.
			kPush,			///< Push a Json object to current array, or make an array together with single Json object.
			kAddProperty,  	///< Add a KVP to current object Json object.
			kRemove,		///< Remove a Json object in an array or Remove a Pair in object
			kRetriveKeys	///< Retrive all the keys an object Json object holds
		};
		IllegalOperationException(OperationKind kind, const char* to, const char* from = 0);
		const char* what() const throw();
	private:
		OperationKind kind_;  	///< what kind operation is being operated
		const char* from_;		///< from which kind Json object
		const char* to_;		///< works on(to) which kind Json object
	};

	/**
	 * \brief Used by std::find_if algorithm to find in std::map structure by values.
	 */
	struct ObjectDataValuePred
	{
		ObjectDataValuePred(Json* json) : json_(json) { }
		bool operator () (const Json::Pair& pair) { return pair.second == json_; }
	private:
		Json* json_;
	};

	/**
	 * \brief Construct an array Json object from std::vector.
	 */
	Json(const ArrayData& arr); // Array

	/**
	 * \brief Construct an object Json object from std::map.
	 */
	Json(const ObjectData& obj); // Object

	/**
	 * \brief The string representation of enum Kind.
	 */
	const char* DataTypeName() const;

	/**
	 * \brief Do deep copy procedure.
	 * @param rhs the other Json object to be copied
	 */
	void CopyData(const Json& rhs);

	/**
	 * \brief Deallocate memory for a Json object.
	 */
	void DestroyData();

	Kind  kind_;	///< which kind of data this Json object represents
	void* data_;	///< the real data held by the Json object

/**
 * \brief Simple standard output stream overloading function to easily output a Json object.
 */
friend std::ostream& operator << (std::ostream& outs, const Json& rhs);
};

}

#endif // GGICCI_JSON_H_