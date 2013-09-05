ggicci::Json version 2
======================

This is the second version. Because of no document and lack of test, I put it here.

Update Log
----------
1. Use pointers to organize data in memory.
2. Bug fix --> 56 Expression: (unsigned)(c + 1) <= 256
3. Remove function: Json& Json::Remove(Json* json); 

Others
---------
考虑了 `void* data;` 的更优雅的实现方式，考虑了访问者模式、内部的继承体系，可是还是没有采用。因为用 C++ 去模拟动态语言的表达的确不好做。我本身就想把所有的对 `Json` 的处理集中在一个类中。避免不了运行时类型的检测。所以就还是采用 `void* data;` 来保存不同类型的数据。其实有一个方案还不错，就是 `JsonArray`, `JsonObject`, `JsonValue` 这样的，通过组合来做。不过我觉得，C++ 解析 Json 用在应用中，很多时候文件格式是确定的，而且就算不确定可以通过 `IsNumber()`, `IsObject()` 这样的函数动态检测再决定做什么，所以就还是坚持原来的想法这样做，暴露给用户的就一个 `Json` 类，没有什么 `JsonArray`, `JsonObject`, `JsonTrue`, `JsonFalse` 什么的。