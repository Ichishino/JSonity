JSonity
=======

JSonity is JSON utility for C++

## Features
* JSON Parsing and Serializing.
* Serializing User Objects into JSON string.
* Serializing STL (map, vector, list, ...) into JSON string directly.
* Support Unicode (wchar_t, char16_t, char32_t).
* Support I/O stream interface.
* One header file only.
* Simple easy interface.

## Examples

```c++

// If using <std::list, set, deque, array, forward_list, ...>
// Include them before "jsonity.hpp"
//
// #include <list>
// ...

#include "jsonity.hpp"

using namespace jsonity;
```

#### Parse JSON string

```c++
// example1_1

std::string jsonStr =
    "{"
        "\"name1\": 100,"
        "\"name2\": true,"
        "\"name3\": ["
            "10,"
            "20,"
            "30"
        "],"
        "\"name4\": \"data\""
    "}";

Json::Value v;
Json::decode(jsonStr, v);   // parse

size_t size = v.getSize();  // 4

bool check = v.hasName("name1");  // true

int n1 = (int)v["name1"].getNumber(); // 100
int n2 = v["name1"];                  // 100

bool b1 = v["name2"].getBoolean(); // true
bool b2 = v["name2"];              // true

Json::Array& arr1 = v["name3"].getArray();
Json::Array& arr2 = v["name3"];

size_t array_size = v["name3"].getSize();  // 3

int arr_n1 = arr1[0];   // 10
int arr_n2 = arr1[1];   // 20
int arr_n3 = arr1[2];   // 30

const std::string& str1 = v["name4"].getString();   // "data"
const std::string& str2 = v["name4"];               // "data"

try
{
    b1 = v["name4"].getBoolean(); // exception
}
catch (const Json::TypeMismatchException&)
{
    // type mismatch
}
```

```c++
// example1_2

std::string jsonStr = "{ ... }";

Json::Error err;	// error info
Json::Value v;

if (!Json::decode(jsonStr, v, &err))	// parse
{
	// error

	printf("Index: %d\n",
		err.getCursor().getPos() + 1);

	printf("Line: %d Col: %d\n",
		err.getCursor().getRow() + 1,
		err.getCursor().getCol() + 1);
}

```

```c++
// example1_3

std::ifstream ifs("json.dat", std::ios::in | std::ios::binary);

Json::Value v;
Json::decode(ifs, v);   // parse
```

```c++
// example1_4

std::ifstream ifs("json.dat", std::ios::in | std::ios::binary);

Json::Value v;
ifs >> v;         // parse

if (!ifs)
{
	// error
}

```

```c++
// example1_5

std::string jsonStr =
    "{"
        "\"name1\": {"
            "\"data1\": ["
                "-3.14,"
                "\"aaaa\","
                "true,"
                "{"
                    "\"subdata1\": ["
                        "600"
                    "]"
                "}"
            "]"
        "}"
    "}";

Json::Value v;
Json::decode(jsonStr, v);   // parse

double d = v["name1"]["data1"][0];    // -3.14
const std::string& str = v["name1"]["data1"][1]; // "aaaa"
bool b = v["name1"]["data1"][2];      // "true"
int n = v["name1"]["data1"][3]["subdata1"][0];  // 600

std::list<Json::Value> listVal;
v.findRecursive("subdata1", listVal);	// find recursively

Json::Value& v2 = *listVal.begin();

n = v2[0]; // 600

```

#### Serialize object to JSON string

```c++
// example2_1

Json::Object root_obj;

root_obj["name1"] = 100;
root_obj["name2"] = true;
root_obj["name3"] = "data_string";
root_obj["name4"] = Json::null();

Json::Array arr(3);
arr[0] = "test";
arr[1] = -400;
arr[2] = false;

root_obj["name5"] = arr;

Json::Object obj;
obj["xxx"] = -1.5;
obj["yyy"] = true;
obj["zzz"] = "test_test";

root_obj["name6"] = obj;

std::list<int> list;  // any STL type (map, vector, list, set, ...)
list.push_back(444);
list.push_back(777);

root_obj["name7"] = list;

std::string jsonStr;
Json::encode(root_obj, jsonStr);  // serialize

// jsonStr ==
//  {"name1":100,"name2":true,"name3":"data_string","name4":null,
//  "name5":["test",-400,false],"name6":{"xxx":-1.5,"yyy":true,"zzz":"test_test"},
//  "name7":[444,777]}"
```

```c++
// example2_2

Json::Object root_obj;

...

std::ofstream ofs("json.dat",
	std::ios::out | std::ios::binary | std::ios::trunc);

Json::encode(root_obj, ofs);  // serialize (redirect)
```

```c++
// example2_3

Json::Object root_obj;

...

std::cout << root_obj << std::endl;  // serialize (redirect)
```

```c++
// example2_4

// User Object
class MyData : public Json::UserValue<MyData>
{
public:
    MyData(int data1, const std::string& data2)
    {
        data1_ = data1;
        data2_ = data2;
    }
    MyData(const MyData& other)
    {
        data1_ = other.data1_;
        data2_ = other.data2_;
    }
    ~MyData()
    {
    }

    int getData1() const
    {   return data1_;    }
    const std::string& getData2() const
    {   return data2_;    }

protected:

    // Encode
    virtual void encode(Json::EncodeContext& ctx) const
    {
        std::ostringstream oss;
        oss << data2_ << "-" << data1_;

        Json::encodeString(ctx, oss.str());
    }

private:
    int data1_;
    std::string data2_;
};


Json::Object root_obj;

// User Object

MyData myData(99, "777");
root_obj["name1"] = myData;

MyData* myDataPtr = new MyData(55, "AAA");
root_obj["name2"] = myDataPtr;

std::string jsonStr;
Json::encode(root_obj, jsonStr);  // serialize

// jsonStr == {"name1":"777-99","name2":"AAA-55"}

delete myDataPtr;
```

```c++
// example2_5

std::map<std::string, std::string> map;  // any STL type (map, vector, list, set, ...)

map["name1"] = "data1";
map["name2"] = "data2";
map["name3"] = "data3";

std::string jsonStr;
Json::encode(map, jsonStr);  // serialize

// jsonStr == {"name1":"data1","name2":"data2","name3":"data3"}
```

```c++
// example2_6

std::map<std::string, std::list<MyData> > map;  // any STL type (map, vector, list, set, ...)

// User Object

std::list<MyData> list;
list.push_back(MyData(66, "666"));
list.push_back(MyData(77, "777"));
list.push_back(MyData(88, "888"));

map["name"] = list;

std::string jsonStr;
Json::encode(map, jsonStr);  // serialize

// jsonStr == {"name":["666-66","777-77","888-88"]}
```

```c++
// example2_7

std::vector<int> list;  // any STL type (map, vector, list, set, ...)

list.push_back(100);
list.push_back(200);
list.push_back(300);

std::string jsonStr;
Json::encode(list, jsonStr);  // serialize

// jsonStr == [100,200,300]

```

```c++
// example2_8

std::map<std::string, std::vector<int> > map;

std::vector<int> vec(3);
vec[0] = 100;
vec[1] = 200;
vec[2] = 300;

map["test"] = vec;

Json::EncodeStyle es1;
es1.setStandardStyle();

std::string jsonStr1;
Json::encode(map, jsonStr1, &es1);  // serialize (Human Readable)

/* jsonStr1 ==
    {
        "test": [
            100,
            200,
            300
        ]
    }
*/

Json::EncodeStyle es2;
es2.setQuat(true);
es2.setPrintNewLine(true);
es2.setEscapeCtrlChar(true);
es2.setNewLine(true, false);

std::string jsonStr2;
Json::encode(map, jsonStr2, &es2);  // serialize (for C++ Program)

/* jsonStr2 ==
    "{\n"
        "\"test\": [\n"
            "100,\n"
            "200,\n"
            "300\n"
        "]\n"
    "}\n"
*/
```

#### Compare object to JSON string

```c++
// example3_1

std::string jsonStr =
    "{"
        "\"aaa\": 100,"
        "\"bbb\": \"data\""
    "}";

Json::Value v;
Json::decode(jsonStr, v);

bool result = Json::equal(v, jsonStr);  // true
```

```c++
// example3_2

std::list<int> list;
list.push_back(100);
list.push_back(200);
list.push_back(300);

bool result1 = Json::equal(list, "[ 100, 200, 300 ]");  // true

bool result2 = Json::equal(list, "[ 300, 100, 200 ]");  // true

bool result3 = Json::equal(list, "[ 300, 100, 200 ]", false);  // false
```
