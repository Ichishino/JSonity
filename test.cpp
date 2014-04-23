#include <stdio.h>
#include <stdlib.h>

#include "jsonity.hpp"

#include <list>
#include <set>

#ifdef WIN32
#include <tchar.h>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h> 
#endif

#ifdef NDEBUG
#undef JSONITY_ASSERT
#define JSONITY_ASSERT(b) (b)
#endif

using namespace jsonity;

static const std::string jstr1 = 
"{"
    "\"name1\" : 12345678, \n"
    "\"name2\" : -2000, \n"
    "\"name3\" : -0.12345, \n"
    "\"name4\" : 144e+4, \n"
    "\"name5\" : 333E-4, \n"
    "\"name6\" : true, \n"
    "\"name7\" : false, \n"
    "\"name8\" : null, \n"
    "\"name9\" : \"test_data1\", \n"
    "\"name10\" : \"test_data2\", \n"

    "\"name11\" : [\n"
    "{ \"sub1\" : { \"sub1sub1\" : [ 2, 3, 4, 5 ], \"sub1sub2\" : [[{},{}],[ [999] ] ] } },\n"
        "{}\n,"
        "{ \"sub2_1\" : \"abcdefg\", \"sub2_2\" : \"hijklmn\" }\n"
    "]\n"
"}";

static const std::string jstr2 = 
"{\r\n"
    "\"name1\": 12345678,\r\n"
    "\"name10\": \"test_data2\",\r\n"
    "\"name11\": [\r\n"
        "{\r\n"
            "\"sub1\": {\r\n"
                "\"sub1sub1\": [\r\n"
                    "2,\r\n"
                    "3,\r\n"
                    "4,\r\n"
                    "5\r\n"
                "],\r\n"
                "\"sub1sub2\": [\r\n"
                    "[\r\n"
                        "{},\r\n"
                        "{}\r\n"
                    "],\r\n"
                    "[\r\n"
                        "[\r\n"
                            "999\r\n"
                        "]\r\n"
                    "]\r\n"
                "]\r\n"
            "}\r\n"
        "},\r\n"
        "{},\r\n"
        "{\r\n"
            "\"sub2_1\": \"abcdefg\",\r\n"
            "\"sub2_2\": \"hijklmn\"\r\n"
        "}\r\n"
    "],\r\n"
    "\"name2\": -2000,\r\n"
    "\"name3\": -0.12345,\r\n"
    "\"name4\": 1.44e+006,\r\n"
    "\"name5\": 0.0333,\r\n"
    "\"name6\": true,\r\n"
    "\"name7\": false,\r\n"
    "\"name8\": null,\r\n"
    "\"name9\": \"test_data1\"\r\n"
"}\r\n";


void test1()
{
    Json::Value v;
    JSONITY_ASSERT(Json::decode(jstr1, v));
    JSONITY_ASSERT(v.getSize() == 11);
    JSONITY_ASSERT(!v.isEmpty());

    JSONITY_ASSERT(v.hasName("name4"));

    int n1 = v["name1"];
    JSONITY_ASSERT(n1 == 12345678);
    JSONITY_ASSERT(v["name1"] == 12345678);

    int n2 = v["name2"]; 
    JSONITY_ASSERT(n2 == -2000);
    JSONITY_ASSERT(v["name2"] == -2000);

    double n3 = v["name3"];
    JSONITY_ASSERT(n3 == -0.12345);
    JSONITY_ASSERT(v["name3"] == -0.12345);

    double n4 = v["name4"];
    JSONITY_ASSERT(n4 == 144e+4);
    JSONITY_ASSERT(v["name4"] == 144e+4);

    double n5 = v["name5"];
    JSONITY_ASSERT(n5 == 333E-4);
    JSONITY_ASSERT(v["name5"] == 333E-4);

    bool n6 = v["name6"];
    JSONITY_ASSERT(n6);
    JSONITY_ASSERT(v["name6"] == true);

    bool n7 = v["name7"];
    JSONITY_ASSERT(!n7);
    JSONITY_ASSERT(v["name7"] == false);

    JSONITY_ASSERT(v["name8"].isNull());

    const Json::String& n9 = v["name9"];
    JSONITY_ASSERT(n9 == "test_data1");
    JSONITY_ASSERT(v["name9"] == "test_data1");

    const char* n10 = v["name10"];
    JSONITY_ASSERT(std::string("test_data2") == n10);
    JSONITY_ASSERT(v["name10"] == "test_data2");

    Json::Value v9;
    JSONITY_ASSERT(v.find("name9", v9));
    JSONITY_ASSERT(v9 == "test_data1");

    std::vector<Json::Value> vec1;
    JSONITY_ASSERT(v.findRecursive("sub1sub1", vec1));
    JSONITY_ASSERT(vec1.size() == 1);
    Json::Value v11_1 = vec1[0];
    JSONITY_ASSERT(v11_1.isArray());

    Json::String jsonStr;
    Json::encode(v11_1, jsonStr);
    JSONITY_ASSERT(jsonStr == "[2,3,4,5]");

    std::list<int> list1;
    list1.push_back(2);
    list1.push_back(3);
    list1.push_back(4);
    list1.push_back(5);
    Json::Value v11_2(list1);
    JSONITY_ASSERT(v11_1 == v11_2);

    v.clear();
    JSONITY_ASSERT(v.isEmpty());
}

void test2()
{
    Json::Value v;

    v = 999;
    JSONITY_ASSERT(v == 999);
    JSONITY_ASSERT(v == 999.0);
    JSONITY_ASSERT(v != 777);
    JSONITY_ASSERT(v != true);
    JSONITY_ASSERT(v != false);
    JSONITY_ASSERT(v.getBoolean());
    JSONITY_ASSERT(v.getReal() == 999.0);

    v = 0;
    JSONITY_ASSERT(v == false);
    JSONITY_ASSERT(v != true);
    JSONITY_ASSERT(v == 0.0);
    JSONITY_ASSERT(!v.getBoolean());

    v = 1;
    JSONITY_ASSERT(v == true);
    JSONITY_ASSERT(v != false);
    JSONITY_ASSERT(v == 1.0);
    JSONITY_ASSERT(v != 100);
    JSONITY_ASSERT(v.getBoolean());

    v = 0.0;
    JSONITY_ASSERT(v == false);
    JSONITY_ASSERT(v != true);
    JSONITY_ASSERT(v == 0);
    JSONITY_ASSERT(v != 100);
    JSONITY_ASSERT(!v.getBoolean());

    v = 1.0;
    JSONITY_ASSERT(v == true);
    JSONITY_ASSERT(v != false);
    JSONITY_ASSERT(v == 1);
    JSONITY_ASSERT(v != 100);
    JSONITY_ASSERT(v.getBoolean());

    v = 1.23456;
    JSONITY_ASSERT(v == 1.23456);
    JSONITY_ASSERT(v != 100);
    JSONITY_ASSERT(v != true);
    JSONITY_ASSERT(v != false);
    JSONITY_ASSERT(v.getBoolean());

    v = true;
    JSONITY_ASSERT(v == true);
    JSONITY_ASSERT(v != false);
    JSONITY_ASSERT(v == 1.0);
    JSONITY_ASSERT(v == 1);
    JSONITY_ASSERT(v != 0);
    JSONITY_ASSERT(v.getNumber() == 1);
    JSONITY_ASSERT(v.getReal() == 1.0);

    v = false;
    JSONITY_ASSERT(v == false);
    JSONITY_ASSERT(v != true);
    JSONITY_ASSERT(v == 0);
    JSONITY_ASSERT(v == 0.0);
    JSONITY_ASSERT(v != 100);
    JSONITY_ASSERT(v.getNumber() == 0);
    JSONITY_ASSERT(v.getReal() == 0.0);

    v = "test_data3";
    JSONITY_ASSERT(v == "test_data3");
    JSONITY_ASSERT(v != "test_data7");

    v = Json::null();
    JSONITY_ASSERT(v.isNull());

    return;
}

void test3()
{
    {
        Json::Value v1;
        JSONITY_ASSERT(Json::decode("\"test_test_test\"", v1));
        JSONITY_ASSERT(v1.isString());
        JSONITY_ASSERT(v1.getSize() == 14);
        JSONITY_ASSERT(v1 == "test_test_test");

        Json::Value v2;
        JSONITY_ASSERT(Json::decode("\"\"", v2));
        JSONITY_ASSERT(v2.isString());
        JSONITY_ASSERT(v2.getSize() == 0);
        JSONITY_ASSERT(v2.isEmpty());
        JSONITY_ASSERT(v2 == "");

        Json::Value v3;
        JSONITY_ASSERT(Json::decode("\" \\\" \\/ \\b \\f \\n \\r \\t \"", v3));
        JSONITY_ASSERT(v3.isString());
        JSONITY_ASSERT(v3.getSize() == 15);
        Json::String s;
        Json::encode(v3, s);
        JSONITY_ASSERT(s == "\" \\\" \\/ \\b \\f \\n \\r \\t \"");
    }

    {
        Json::Value v1;
        JSONITY_ASSERT(Json::decode("[ 100,200,300,400,500 ]", v1));
        JSONITY_ASSERT(v1.isArray());
        JSONITY_ASSERT(v1.getSize() == 5);
        JSONITY_ASSERT(v1[0] == 100);
        JSONITY_ASSERT(v1[1] == 200);
        JSONITY_ASSERT(v1[2] == 300);
        JSONITY_ASSERT(v1[3] == 400);
        JSONITY_ASSERT(v1[4] == 500);

        Json::Value v2;
        JSONITY_ASSERT(Json::decode("[   ]", v2));
        JSONITY_ASSERT(v2.isArray());
        JSONITY_ASSERT(v2.getSize() == 0);
        JSONITY_ASSERT(v2.isEmpty());
    }

    {
        Json::Value v1;
        JSONITY_ASSERT(Json::decode("true", v1));
        JSONITY_ASSERT(v1.isBoolean());
        JSONITY_ASSERT(v1 == true);

        Json::Value v2;
        JSONITY_ASSERT(Json::decode("false", v2));
        JSONITY_ASSERT(v2.isBoolean());
        JSONITY_ASSERT(v2 == false);

        Json::Value v3;
        JSONITY_ASSERT(Json::decode("null", v3));
        JSONITY_ASSERT(v3.isNull());
    }

    {
        Json::Value v1;
        JSONITY_ASSERT(Json::decode("98765", v1));
        JSONITY_ASSERT(v1.isNumber());
        JSONITY_ASSERT(v1 == 98765);

        Json::Value v2;
        JSONITY_ASSERT(Json::decode("-66666", v2));
        JSONITY_ASSERT(v2.isNumber());
        JSONITY_ASSERT(v2 == -66666);
    }

    {
        Json::Value v1;
        JSONITY_ASSERT(Json::decode("-1111.2222", v1));
        JSONITY_ASSERT(v1.isReal());
        JSONITY_ASSERT(v1 == -1111.2222);

        Json::Value v2;
        JSONITY_ASSERT(Json::decode("1.4e-5", v2));
        JSONITY_ASSERT(v2.isReal());
        JSONITY_ASSERT(v2 == 1.4e-5);

        Json::Value v3;
        JSONITY_ASSERT(Json::decode("77.4E+5", v3));
        JSONITY_ASSERT(v3.isReal());
        JSONITY_ASSERT(v3 == 77.4E+5);
    }

    {
        std::map<std::string, std::map<int, std::list<std::string> > > map;

        map["aaa"][2].push_back("zzzzzzzzz");
        map["aaa"][2].push_back("xxxxxxxxx");
        map["aaa"][2].push_back("yyyyyyyyy");
        map["aaa"][5].push_back("wwwwwwwww");
        map["aaa"][5].push_back("vvvvvvvvv");
        map["aaa"][5].push_back("uuuuuuuuu");
        map["aaa"][5].push_back("ttttttttt");

        Json::Value v1(map);

        JSONITY_ASSERT(v1["aaa"]["5"][3] == "ttttttttt");

        Json::Value v2;
        Json::String s;
        Json::encode(v1, s);
        JSONITY_ASSERT(Json::decode(s, v2));

        JSONITY_ASSERT(v1 == v2);
    }

    {
        std::list<std::vector<std::set<double> > > arr;

        std::set<double> set;
        set.insert(111.111);
        set.insert(222.222);
        set.insert(333.333);

        std::vector<std::set<double> > vec;
        vec.push_back(set);
        vec.push_back(set);
        vec.push_back(set);

        arr.push_back(vec);
        arr.push_back(vec);
        arr.push_back(vec);

        Json::Value v1(arr);

        Json::Value v2;
        Json::String s;
        Json::encode(v1, s);
        JSONITY_ASSERT(Json::decode(s, v2));

        JSONITY_ASSERT(v1 == v2);
    }

    {
        std::map<std::string, std::vector<int> > map;

        std::string s;
        Json::encode(map, s);
    }

    return;
}

void test4()
{
    Json::Value v1;
    JSONITY_ASSERT(Json::decode(jstr1, v1));

    Json::Value v2;
    JSONITY_ASSERT(Json::decode(jstr1, v2));

    JSONITY_ASSERT(v1.equal(v2));
    JSONITY_ASSERT(v1 == v2);

    Json::Value v3;
    Json::String s;
    Json::encode(v1, s);
    JSONITY_ASSERT(Json::decode(s, v3));

    JSONITY_ASSERT(v2.equal(v3));
    JSONITY_ASSERT(v2 == v3);
}

void test5()
{
    Json::Object root;
    root.insert("name1", Json::null());
    root.insert("name2", 100);
    root.insert("name3", "@@@@@@@@@@");
    root.insert("name4", true);
    root.insert("name5", false);

    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    root.insert("name6", vec);

    std::list<std::string> list;
    list.push_back("test1");
    list.push_back("test2");
    list.push_back("test3");
    list.push_back("test4");
    std::map<std::string, std::list<std::string> > obj;
    obj["obj1"] = list;
    list.clear();
    obj["obj2"] = list;
    root.insert("name7", obj);

    Json::Array arr(5);
    arr[0] = 777;
    arr[1] = "ABCDEF";
    arr[2] = true;
    arr[3] = 2.0;
    arr[4] = list;
    root.insert("name8", arr);

    Json::Value v;
    Json::String s;
    Json::encode(root, s);
    JSONITY_ASSERT(Json::decode(s, v));
    JSONITY_ASSERT(v == root);

    return;
}

void test6()
{
    {
        Json::Value v;
        JSONITY_ASSERT(Json::decode("[ 100, 500, 70 ]", v));

        // vector

        std::vector<int> vec;

        vec.push_back(100);
        vec.push_back(500);
        vec.push_back(70);

        JSONITY_ASSERT(v.equal(vec, true));
        JSONITY_ASSERT(v.equal(vec, false));
        JSONITY_ASSERT(v == vec);

        vec.clear();
        vec.push_back(70);
        vec.push_back(100);
        vec.push_back(500);

        JSONITY_ASSERT(v.equal(vec, true));
        JSONITY_ASSERT(!v.equal(vec, false));

        // list

        std::list<int> list;

        list.push_back(100);
        list.push_back(500);
        list.push_back(70);

        JSONITY_ASSERT(v.equal(list, true));
        JSONITY_ASSERT(v.equal(list, false));
        JSONITY_ASSERT(v == list);

        list.clear();
        list.push_back(70);
        list.push_back(100);
        list.push_back(500);

        JSONITY_ASSERT(v.equal(list, true));
        JSONITY_ASSERT(!v.equal(list, false));
    }

    {
        Json::Value v;
        JSONITY_ASSERT(Json::decode("{ \"aaa\" : \"data1\" , \"bbb\" : \"data2\" }", v));

        // map

        std::map<std::string, std::string> map;

        map["aaa"] = "data1";
        map["bbb"] = "data2";

        JSONITY_ASSERT(v.equal(map));
        JSONITY_ASSERT(v == map);
    }

    {
        Json::Value v;
        JSONITY_ASSERT(Json::decode(
            "{ \"aaa\" : \"data1\" , \"bbb\" : 256, \"ccc\" : [ 45, 77, 200], \"ddd\" : true }", v));

        // Value map

        std::map<std::string, Json::Value> map;

        map["aaa"] = "data1";
        map["bbb"] = 256;

        std::list<int> list;
        list.push_back(45);
        list.push_back(77);
        list.push_back(200);            
            
        map["ccc"] = list;
        map["ddd"] = true;

        JSONITY_ASSERT(v.equal(map));
        JSONITY_ASSERT(v == map);
    }
}

void test7()
{
    {
        Json::Value v;
        JSONITY_ASSERT(Json::decode("[ 100, 200, 300 ]", v));

        Json::Error error;
        JSONITY_ASSERT(Json::equal(v, "[ 100, 200, 300 ]", false, &error) && error.isEmpty());
        JSONITY_ASSERT(Json::equal(v, "[ 200, 100, 300 ]", true, &error) && error.isEmpty());
        JSONITY_ASSERT(!Json::equal(v, "[ 200, 100, 300 ]", false, &error) && error.isEmpty());
    }

    {
        std::vector<int> vec;

        vec.push_back(100);
        vec.push_back(200);
        vec.push_back(300);

        Json::Error error;
        JSONITY_ASSERT(Json::equal(vec, "[ 100, 200, 300 ]", false, &error) && error.isEmpty());
    }

    {
        std::vector<int> vec;
        vec.push_back(100);
        vec.push_back(200);
        vec.push_back(300);

        std::list<std::vector<int> > list;
        list.push_back(vec);
        list.push_back(std::vector<int>());

        Json::Error error;
        JSONITY_ASSERT(Json::equal(list, "[ [100, 200, 300], [  ] ]", false, &error) && error.isEmpty());
    }

    {
        Json::Array arr(5);

        arr[0] = 999; 
        arr[1] = true; 
        arr[2] = Json::null();

        std::vector<int> vec;
        vec.push_back(100);
        vec.push_back(200);
        vec.push_back(300);
        arr[3] = vec; 

        arr[4] = "testtest";

        Json::Error error;
        JSONITY_ASSERT(Json::equal(
            arr, "[ 999, true, null, [ 100, 200, 300 ], \"testtest\" ]", false, &error) && error.isEmpty());
    }

    {
        std::map<std::string, int> map;

        map["aaa"] = 456;
        map["bbb"] = 777;

        Json::Error error;
        JSONITY_ASSERT(Json::equal(
            map, "{ \"aaa\" : 456, \"bbb\" : 777 }", false, &error) && error.isEmpty());
    }

    {
        Json::Object obj;

        obj["aaa"] = 666;
        obj["bbb"] = false;
        obj["ccc"] = Json::null();
        obj["ddd"] = "hello";

        std::map<std::string, int> map;
        map["zzz"] = 456;
        map["xxx"] = 777;
        obj["eee"] = map;

        std::vector<int> vec;
        vec.push_back(100);
        vec.push_back(200);
        vec.push_back(300);
        obj["fff"] = vec;

        Json::Error error;
        JSONITY_ASSERT(Json::equal(obj,
            "{"
                "\"aaa\" : 666,"
                "\"bbb\" : false,"
                "\"ccc\" : null,"
                "\"ddd\" : \"hello\","
                "\"eee\" : { \"zzz\" : 456, \"xxx\" : 777 },"
                "\"fff\" : [ 100, 200, 300 ]"
            "}",
            false,
            &error) && error.isEmpty());
    }

    return;
}

void test8()
{
    {
        Json::Value v;
        JSONITY_ASSERT(Json::decode(jstr1, v));

        // Human readable
        Json::EncodeStyle es;
        es.setStandardStyle();

        Json::String s;
        Json::encode(v, s, &es);

        printf("%s", s.c_str());

        JSONITY_ASSERT(Json::equal(v, s));
    }

    printf("\r\n\r\n");

    {
        Json::Value v;
        JSONITY_ASSERT(Json::decode(jstr1, v));

        Json::String s;
        Json::encode(v, s);

        printf("%s", s.c_str());

        JSONITY_ASSERT(Json::equal(v, s));
    }

    printf("\r\n\r\n");

    {
        Json::Value v;
        JSONITY_ASSERT(Json::decode(jstr2, v));

        Json::EncodeStyle es;
        es.setEscapeCtrlChar(true);

        Json::String s;
        Json::encode(v, s, &es);

        printf("%s", s.c_str());

        JSONITY_ASSERT(Json::equal(v, jstr2));
    }

    printf("\r\n\r\n");

    {
        Json::Value v;
        JSONITY_ASSERT(Json::decode("{ \"test\"  : [ [1] , [null], [true, 1, -3.14, { \"data1\":777, \"data2\":999 }, {}, [{},[], 100] ]  ]   }", v));

        Json::EncodeStyle es;
        es.setIndent(true,'\t', 1);

        Json::String s;
        Json::encode(v, s, &es);

        printf("%s", s.c_str());

        JSONITY_ASSERT(Json::equal(v, s));
    }
}

void example1()
{
    std::string jsonStr1 = "{ \"name1\": 100, \"name2\": true, \"name3\": [10,20,30], \"name4\": \"data\" }";

    Json::Value v;
    Json::decode(jsonStr1, v);

    size_t size = v.getSize();

    bool check = v.hasName("name1");

    int n = v["name1"];
    bool b = v["name2"];

    size_t array_size = v["name3"].getSize();

    int arr1 = v["name3"][0];
    int arr2 = v["name3"][1];
    int arr3 = v["name3"][2];

    const std::string& str = v["name4"];

    JSONITY_ASSERT(check == true);
    JSONITY_ASSERT(size == 4);
    JSONITY_ASSERT(array_size == 3);
    JSONITY_ASSERT(n == 100);
    JSONITY_ASSERT(b == true);
    JSONITY_ASSERT(arr1 == 10);
    JSONITY_ASSERT(arr2 == 20);
    JSONITY_ASSERT(arr3 == 30);
    JSONITY_ASSERT(str == "data");

    return;
}

void example2()
{
    std::string jsonStr2 = "{ \"name1\": {\"data1\": [-3.14,\"aaaa\",true, { \"subdata1\": [600] }] } }";

    Json::Value v;
    Json::decode(jsonStr2, v);

    double d = v["name1"]["data1"][0];
    std::string& str = v["name1"]["data1"][1];
    bool b = v["name1"]["data1"][2];
    int n = v["name1"]["data1"][3]["subdata1"][0];

    JSONITY_ASSERT(d == -3.14);
    JSONITY_ASSERT(str == "aaaa");
    JSONITY_ASSERT(b == true);
    JSONITY_ASSERT(n == 600);

    return;
}

void example3()
{
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

    std::list<int> list;
    list.push_back(444);
    list.push_back(777);

    root_obj["name7"] = list;

    std::string jsonStr;
    Json::encode(root_obj, jsonStr);

    // jsonStr == jsonStr == {"name1":100,"name2":true,"name3":"data_string","name4":null,
    //     "name5":["test",-400,false],"name6":{"xxx":-1.5,"yyy":true,"zzz":"test_test"},"name7":[444,777]}

    return;
}

void example4()
{
    std::map<std::string, std::list<std::string> > map;

    std::list<std::string> list;
    list.push_back("vvv");
    list.push_back("www");
    list.push_back("xxx");

    map["name"] = list;

    std::string jsonStr;
    Json::encode(map, jsonStr);

    // jsonStr == "{"name\":["vvv","www","xxx"]}"

    return;
}

void example5()
{
    std::map<std::string, std::vector<int> > map;

    std::vector<int> vec(3);
    vec[0] = 100;
    vec[1] = 200;
    vec[2] = 300;

    map["test"] = vec;

    Json::EncodeStyle es1;
    es1.setStandardStyle();

    std::string jsonStr1;
    Json::encode(map, jsonStr1, &es1);

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
    Json::encode(map, jsonStr2, &es2);

    /* jsonStr2 ==
        "{\n"
            "\"test\": [\n"
                "100,\n"
                "200,\n"
                "300\n"
            "]\n"
        "}\n"
    */

    return;
}

void example6()
{
    std::string jsonStr = "{ \"aaa\": 100, \"bbb\": \"data\" }";

    Json::Value v;
    Json::decode(jsonStr, v);

    bool result = Json::equal(v, jsonStr);

    JSONITY_ASSERT(result);
}

void example7()
{
    std::list<int> list;
    list.push_back(100);
    list.push_back(200);
    list.push_back(300);

    bool result1 = Json::equal(list, "[ 100, 200, 300 ]");
    
    bool result2 = Json::equal(list, "[ 300, 100, 200 ]");

    bool result3 = Json::equal(list, "[ 300, 100, 200 ]", false);

    JSONITY_ASSERT(result1);
    JSONITY_ASSERT(result2);
    JSONITY_ASSERT(!result3);
}

#ifdef WIN32
int _tmain(int, _TCHAR**) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#else
int main(int, char**) {
#endif

    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    example1();
    example2();
    example3();
    example4();
    example5();
    example6();
    example7();

    return 0;
}