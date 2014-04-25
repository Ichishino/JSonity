JSonity
=======

JSonity is JSON utility for C++

## Features
* One header file only.
* Simple easy interface.

## Examples

```c++
#include "jsonity.hpp"

using namespace jsonity;
```

#### Parse JSON string

```c++
// example1

std::string jsonStr1 =
	"{ \"name1\": 100, \"name2\": true, \"name3\": [10,20,30], \"name4\": \"data\" }";

Json::Value v;
Json::decode(jsonStr1, v);   // parse

size_t size = v.getSize();  // 4

bool check = v.hasName("name1");  // true

int n = v["name1"];    // 100
bool b = v["name2"];   // true

size_t array_size = v["name3"].getSize();  // 3

int arr1 = v["name3"][0];   // 10
int arr2 = v["name3"][1];   // 20
int arr3 = v["name3"][2];   // 30

const std::string& str = v["name4"];   // "data"
```

```c++
// example2

std::string jsonStr2 =
	"{ \"name1\": {\"data1\": [-3.14,\"aaaa\",true, { \"subdata1\": [600] }] } }";

Json::Value v;
Json::decode(jsonStr2, v);   // parse

double d = v["name1"]["data1"][0];    // -3.14
const std::string& str = v["name1"]["data1"][1]; // "aaaa"
bool b = v["name1"]["data1"][2];      // "true"
int n = v["name1"]["data1"][3]["subdata1"][0];  // 600
```

#### Serialize object to JSON string

```c++
// example3

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
// 	{"name1":100,"name2":true,"name3":"data_string","name4":null,
//  "name5":["test",-400,false],"name6":{"xxx":-1.5,"yyy":true,"zzz":"test_test"},
//  "name7":[444,777]}"
```

```c++
// example4

std::map<std::string, std::list<std::string> > map;  // any STL type (map, vector, list, set, ...)

std::list<std::string> list;
list.push_back("vvv");
list.push_back("www");
list.push_back("xxx");

map["name"] = list;

std::string jsonStr;
Json::encode(map, jsonStr);  // serialize

// jsonStr == {"name":["vvv","www","xxx"]}"
```

```c++
// example5

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
// example6

std::string jsonStr = "{ \"aaa\": 100, \"bbb\": \"data\" }";

Json::Value v;
Json::decode(jsonStr, v);

bool result = Json::equal(v, jsonStr);

// result == true
```

```c++
// example7

std::list<int> list;   // any STL type (map, vector, list, set, ...)
list.push_back(100);
list.push_back(200);
list.push_back(300);

bool result1 = Json::equal(list, "[ 100, 200, 300 ]");

// result1 == true

bool result2 = Json::equal(list, "[ 300, 100, 200 ]");

// result2 == true

bool result3 = Json::equal(list, "[ 300, 100, 200 ]", false);

// result3 == false
```
