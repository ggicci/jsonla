ggicci::json
============

A cute C++ library for easily parsing and manipulating JSON data structure.

Parser
------
Convert a json string to a plain C++ object. Then you can retrieve data or modify them by operating on the C++ object.

Now you have a json structural string like **{ "year": 2013, "month": 8, "day": 29 }**, and just use
the static function `ggicci::Json::Parse(const char*)` to get a `ggicci::Json` object. Then you can do what you want on it. See below.

Files to Include
----------------
There're only two files: `gci-json.h` and `gci-json.cc`. You have to include `gci-json.h`. And the
`namespace` is `ggicci`. We assume that `using namespace ggicci;` already declared.


Manipulation
------------
### Construct a Json Object

	// from int
	Json json(3);
	// from double
	Json json(4.23);
	// from bool
	Json json(true);
	// from string, no parse
	Json json("hello world");
	// from a json string, parse
	Json json = Json::Parse("[1, false, null, {\"name\": \"Ggicci\", \"age\": 21}");
	// null
	Json json; // json.IsNull(); --> true

### Data Retrieving

	Json json = Json::Parse("{				\
		\"id\": 20130192,					\
		\"user\": \"ggicci\",				\
		\"mail\": \"ggicci@163.com\",		\
		\"password\": \"md5 code here\",	\
		\"birthday\": 19911110,				\
		\"gender\": 1, 						\
		\"tags\": [							\
			\"program fans\",				\
			\"dog\",						\
			\"nightbird\",					\
			\"anime fans\",					\
			\"almost perfectionist\"		\
		],									\
		\"motto\": null						\
	}");
	cout << json.ToString() << endl;
	// output: 
	// { "birthday": 1.99111e+007, "gender": 1, "id": 2.01302e+010, "mail": "ggicci@163.com", "motto": null, "password": "md5 code here", "tags": [ "program fans", "dog", "nightbird", "anime fans", "almost perfectionist" ], "user": "ggicci" }

	cout << json["id"].AsInt() << endl; // output: 20130192
	cout << json["user"].AsString() << endl; // output: ggicci
	cout << (json["motto"].IsNull() ? "lazy man leaves nothing..."
		: json["motto"].AsString()) << endl; // output: lazy man leaves nothing...

	// Traverse Array
	Json &tags = json["tags"];
	for (int i = 0; i < tags.Size(); ++i)
	{
		cout << tags[i].AsString() << endl;
	}
	// output:
	// program fans
	// dog
	// nightbird
	// anime fans
	// almost perfectionist

	// Traverse Object
	vector<string> keys = json.Keys();
	for (vector<string>::const_iterator cit = keys.begin();
		cit != keys.end(); ++cit)
	{
		cout << *cit << ": " << json[cit->c_str()].ToString() << endl;
	}
	// output:
	// birthday: 1.99111e+007
	// gender: 1
	// id: 2.01302e+007
	// mail: "ggicci@163.com"
	// motto: null
	// password: "md5 code here"
	// tags: [ "program fans", "dog", "nightbird", "anime fans", "almost perfectionist" ]
	// user: "ggicci"
	
### Data Modification

	// Assignment
	Json json;		// null
	json = 1;		// 1
	json = 3.234;	// 3.234
	json = true;	// true
	json = "hello";	// "hello"
	json = Json::Parse("[1, 2, 3, 4]"); // array
	json = Json();	// null

	// Reference
	Json json = Json::Parse("[1, 2, 3, 4]");
	json[0] = 0; // [0, 2, 3, 4]
	Json &second = json[1];
	second = "hello"; // [ 0, "hello", 3, 4 ]

	// Copy
	Json json = Json::Parse("[1, 2, 3, 4]");
	Json co_third = json[2];
	co_third = true; // [1, 2, 3, 4]

	// Push Values to Array
	Json json(1);
	json.Push(Json("hello")).Push(Json()).Push(Json::Parse("{}"));
	// [ 1, "hello", null, {  } ]

	// Add Properties to an Object
	Json json = Json::Parse("{}");
	json.AddProperty("id", Json(1))
		.AddProperty("name", Json("ggicci"))
		.AddProperty("motto", Json());
	// { "id": 1, "motto": null, "name": "ggicci" }
	// Exception(a bad conversion) will be thrown if you apply
	// AddProperty() on a non-object Json object.

	// Remove Item from an Array, no cascade
	Json json = Json::Parse("[1, 2, 3, 4]");
	json.Remove(0);
	json.Remove(1);
	json.Remove(8); // invalid index do noting
	cout << json.ToString() << endl;
	// [ 2, 4 ]

	// Remove Item from an Object, can cascade
	Json json = Json::Parse("{ \"id\": 123, \"name\": \"ggicci\", \"gender\": 1 }");
	json.Remove("id").Remove("gender").Remove("age");
	cout << json.ToString() << endl;
	// { "name": "ggicci" }

### Exception Handling
	
	// Parse Exception
	try
	{
		Json json = Json::Parse("[1, 2, 3, 4, ]");
	}
	catch (exception& e)
	{
		cout << e.what() << endl; // SyntaxError: Unexpected token ] at pos 13
	}

	// Convert Exception
	try
	{
		Json json = Json::Parse("[1, 2, 3, 4, 5]");
		json[0].AsString(); // should use AsInt() or AsDouble().
	}
	catch (exception& e)
	{
		cout << e.what() << endl; // a bad conversion
	}	

More
----
### Note
There may exist potential bugs, if you found some bugs, please be sure to let me know.

### Document
The document is in **doc.zip**.
