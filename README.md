ggicci::json
============

A cute C++ library for easily parsing and manipulating JSON data structure.

Parser
----------------------
Convert a json structural string to a C++ object. And you can do things you want on this C++ object.
You might just want to extract the data you want a json string represents. Or you might also want to
modify the data in this json string. With this library, you can do all of these things on a C++
object parsed from your json string through simple statements. It's convienent.

Now you have a json structural string like **{ "year": 2013, "month": 8, "day": 29 }**, and just use
the static function `ggicci::Json::Parse(const char*)` to get a `ggicci::Json` object. Then you can
do whatever you want on this object. See below.

Files to Include
---------------------
There're only two files: `gci-json.h` and `gci-json.cc`. You have to include `gci-json.h`. And the
`namespace` is `ggicci`. We assume that `using namespace ggicci;` already exists in the code below.

Data Extraction
------------------------
###Number, true, false, null

	Json json = Json::Parse("124.8");
	int number = json;
	
	bool isRight = Json::Parse("true");

	bool isNull = Json::Parse("null").IsNull();

###String
	
	Json json = Json::Parse("\"Hello World!\"");
	const char* hello = json;
	string hello2 = json;

###Object

	Json json = Json::Parse("{\"id\": 1991, \"name\": \"Ggicci\"}");
	// extract a value specified by a key(name)
	const Json &ref = json["id"];  // get the reference
	Json copy = json["id"];  // get a copy
	const Json *pid = &json["id"]; // get a pointer to this value

###Array

	Json json = Json::Parse("[1, 2, 3, 4]");
	// extract a value specified by the index
	int first = json[0];
	int second = json[1];

Data Manipulation
-----------------------------------
	Json json = Json::Parse("{ \"author\": \"Ggicci\", \"version\": \"1.0 Beta\", \"keywords\": [\"ggicci\", \"json\", \"java\"], \"cute\": true }");

	// change value of the version
	json["version"] = "1.0";

	// change the value "java" in "keywords" to "c++"
	json["keywords"][2] = "c++";

	// add two keywords "parser" and "manipulator" to "keywords"
	json["keywords"].Add("parser").Add("manipulator");

	// remove keyword "ggicci"
	json["keywords"].Remove(0); // cant cascade remove an array item

	// remove "version" and "cute"
	json.Remove("version").Remove("cute"); // cascade for object

	// add a KVP named "since", valued [2013, 8, 26] to json
	json.Add("since", Json::Parse("[2013, 8, 26]"));

	// you can change the value, even from number to string
	Json number = 1991;
	number = "Hello World";
	number = Json::Parse("[1, 2, 3]");

	// you can add values to a number, a string, a bool, a null to make an array
	Json val = true;
	val.Add("hello").Add(null).Add(Json::Parse("{\"ack\": 1845}"));
	// --> Got [ true, "hello", null, { "ack" : 1845 } ]

Get Json String through Json Object
----------------------------------------

	// Organize a json structure
	Json json = Json::Parse("{}");
	json.Add("name", "Ggicci").Add("birthday", Json::Parse("[1991, 11, 10]")).Add("characteritics", null);

	Json &traits = json["characteritics"];
	(traits = "compassionate").Add("independent");

	// Get the json string
	cout << json.ToString() << endl;

More
-------------------
### Note
It's a beta version now, so there may exist potential bugs, if you find some bugs, please be sure
to tell me.

### Project Url
- Github: [https://github.com/ggicci/ggicci--json](https://github.com/ggicci/ggicci--json)
- GGICCI: [http://ggicci.me/works/json](http://ggicci.me/works/json)

### Document
There is a document [here](http://ggicci.me/works/json/doc) on my personal blog.


