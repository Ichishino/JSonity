#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <list>
#include <set>
#include <deque>

#if !defined(_MSC_VER) || (_MSC_VER >= 1600)
#include <array>
#include <forward_list>
#include <unordered_set>
#endif

#include "jsonity.hpp"

#ifdef JSONITY_OS_WINDOWS
#include <tchar.h>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h> 
#endif

#ifdef NDEBUG
#undef JSONITY_ASSERT
#define JSONITY_ASSERT(b) (b)
#endif

using namespace jsonity;

#ifdef _JSONITY_TEST_
#include "test.h"
#endif

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
    JSONITY_ASSERT(v == true);
    JSONITY_ASSERT(v != false);
    JSONITY_ASSERT(v.getReal() == 999.0);

    v = 0;
    JSONITY_ASSERT(v == 0);
    JSONITY_ASSERT(v == false);
    JSONITY_ASSERT(v != true);
    JSONITY_ASSERT(v == 0.0);
    JSONITY_ASSERT(v != 100);
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
    JSONITY_ASSERT(v == true);
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

    JSONITY_ASSERT(v1.compare(v2) == 0);
    JSONITY_ASSERT(v1 == v2);

    Json::Value v3;
    Json::String s;
    Json::encode(v1, s);
    JSONITY_ASSERT(Json::decode(s, v3));

    JSONITY_ASSERT(v2.compare(v3) == 0);
    JSONITY_ASSERT(v2 == v3);
}

void test5()
{
    Json::Object root;
    root["name1"] = Json::null();
    root["name2"] = 100;
    root["name3"] = "@@@@@@@@@@";
    root["name4"] = true;
    root["name5"] = false;

    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    root["name6"] = vec;

    std::list<std::string> list;
    list.push_back("test1");
    list.push_back("test2");
    list.push_back("test3");
    list.push_back("test4");
    std::map<std::string, std::list<std::string> > obj;
    obj["obj1"] = list;
    list.clear();
    obj["obj2"] = list;
    root["name7"] = obj;

    Json::Array arr(5);
    arr[0] = 777;
    arr[1] = "ABCDEF";
    arr[2] = true;
    arr[3] = 2.0;
    arr[4] = list;
    root["name8"] = arr;

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

        JSONITY_ASSERT(v.compare(vec, true) == 0);
        JSONITY_ASSERT(v.compare(vec, false) == 0);
        JSONITY_ASSERT(v == vec);

        vec.clear();
        vec.push_back(70);
        vec.push_back(100);
        vec.push_back(500);

        JSONITY_ASSERT(v.compare(vec, true) == 0);
        JSONITY_ASSERT(!v.compare(vec, false) == 0);

        // list

        std::list<int> list;

        list.push_back(100);
        list.push_back(500);
        list.push_back(70);

        JSONITY_ASSERT(v.compare(list, true) == 0);
        JSONITY_ASSERT(v.compare(list, false) == 0);
        JSONITY_ASSERT(v == list);

        list.clear();
        list.push_back(70);
        list.push_back(100);
        list.push_back(500);

        JSONITY_ASSERT(v.compare(list, true) == 0);
        JSONITY_ASSERT(!v.compare(list, false) == 0);
    }

    {
        Json::Value v;
        JSONITY_ASSERT(Json::decode("{ \"aaa\" : \"data1\" , \"bbb\" : \"data2\" }", v));

        // map

        std::map<std::string, std::string> map;

        map["aaa"] = "data1";
        map["bbb"] = "data2";

        JSONITY_ASSERT(v.compare(map) == 0);
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

        JSONITY_ASSERT(v.compare(map) == 0);
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
        std::map<int, int> map;
        map[100] = 1111;
        map[200] = 2222;
        map[300] = 3333;

        std::string jsonStr;
        Json::encode(map, jsonStr);

        Json::Error error;
        JSONITY_ASSERT(Json::equal(map,
            "{ \"100\": 1111, \"200\": 2222, \"300\": 3333}", error.isEmpty()));

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

    {
        Json::Value v;
        JSONITY_ASSERT(Json::decode("\"\\u0019\\u0018\\u007f\"", v));

        JSONITY_ASSERT(v.getString()[0] == 0x19);
        JSONITY_ASSERT(v.getString()[1] == 0x18);
        JSONITY_ASSERT(v.getString()[2] == 0x7f);

        std::string jsonStr;
        Json::encode(v, jsonStr);

        JSONITY_ASSERT(Json::equal(v, jsonStr));
    }


    {
        Json::Value v;
        v["test1"]["test2"]["test3"] = 100;

        std::string jsonStr;
        Json::encode(v, jsonStr);

        JSONITY_ASSERT(Json::equal(v,
            "{"
                "\"test1\": {"
                    "\"test2\": {"
                        "\"test3\": 100"
                    "}"
                "}"
            "}"
        ));
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

void test9()
{
    {
        Json::Value v = true;
        int64_t n = v;

        JSONITY_ASSERT(n == 1);
    }

    {
        Json::Value v = "123456";
        int64_t n = v.toNumber();

        JSONITY_ASSERT(n == 123456);
    }

    {
        Json::Value v = -123.777;
        int64_t n = v.toNumber();

        JSONITY_ASSERT(n == -123);
    }

    {
        Json::Value v = -123.777;
        std::string s = v.toString();

        JSONITY_ASSERT(s == "-123.777");
    }

    {
        Json::Value v = true;

        int64_t n = v;
        JSONITY_ASSERT(n == 1);

        std::string s = v.toString();
        JSONITY_ASSERT(s == "1");

        v = false;

        n = v;
        JSONITY_ASSERT(n == 0);

        s = v.toString();
        JSONITY_ASSERT(s == "0");
    }
}

void test10()
{
    {
        Json::Value v1 = 100;
        Json::Value v2 = 200;

        JSONITY_ASSERT(v1 < v2);
        JSONITY_ASSERT(v1 < 300);
    }

    {
        Json::Value v1 = 200.1;
        Json::Value v2 = 200;

        JSONITY_ASSERT(v2 < v1);
        JSONITY_ASSERT(v2 < 200.1);
    }

    {
        Json::Value v1 = true;
        Json::Value v2 = false;

        JSONITY_ASSERT(v2 < v1);
        JSONITY_ASSERT(v2 < true);
    }

    {
        Json::Value v1 = -1.2;
        Json::Value v2 = 1.0;

        JSONITY_ASSERT(v1 < v2);
        JSONITY_ASSERT(v1 < 2);
    }

    {
        Json::Value v1 = "abcdefg";
        Json::Value v2 = "hijklmn";

        JSONITY_ASSERT(v1 < v2);
    }
}

void test11()
{
    {
        Json::Value v = "test";

        for (;;)
        {
            try
            {
                v.getNumber();
            }
            catch (const Json::TypeMismatchException& e)
            {
                printf("%s(%d)\n", e.what(), e.getCodeLine());
                break;
            }

            JSONITY_ASSERT(false);
            break;
        }

        for (;;)
        {
            try
            {
                v.getBoolean();
            }
            catch (const Json::TypeMismatchException& e)
            {
                printf("%s(%d)\n", e.what(), e.getCodeLine());
                break;
            }

            JSONITY_ASSERT(false);
            break;
        }

        for (;;)
        {
            try
            {
                v.getReal();
            }
            catch (const Json::TypeMismatchException& e)
            {
                printf("%s(%d)\n", e.what(), e.getCodeLine());
                break;
            }

            JSONITY_ASSERT(false);
            break;
        }

        for (;;)
        {
            try
            {
                v.getObject();
            }
            catch (const Json::TypeMismatchException& e)
            {
                printf("%s(%d)\n", e.what(), e.getCodeLine());
                break;
            }

            JSONITY_ASSERT(false);
            break;
        }

        for (;;)
        {
            try
            {
                v.getArray();
            }
            catch (const Json::TypeMismatchException& e)
            {
                printf("%s(%d)\n", e.what(), e.getCodeLine());
                break;
            }

            JSONITY_ASSERT(false);
            break;
        }
    }

    {
        Json::Value v = 100;

        for (;;)
        {
            try
            {
                v.getString();
            }
            catch (const Json::TypeMismatchException& e)
            {
                printf("%s(%d)\n", e.what(), e.getCodeLine());
                break;
            }

            JSONITY_ASSERT(false);
            break;
        }

        for (;;)
        {
            try
            {
                v.getObject();
            }
            catch (const Json::TypeMismatchException& e)
            {
                printf("%s(%d)\n", e.what(), e.getCodeLine());
                break;
            }

            JSONITY_ASSERT(false);
            break;
        }

        for (;;)
        {
            try
            {
                v.getArray();
            }
            catch (const Json::TypeMismatchException& e)
            {
                printf("%s(%d)\n", e.what(), e.getCodeLine());
                break;
            }

            JSONITY_ASSERT(false);
            break;
        }
    }

    {
        Json::Value v = 100;

        for (;;)
        {
            try
            {
                v.getString();
            }
            catch (const std::exception& e)
            {
                printf("%s\n", e.what());
                break;
            }

            JSONITY_ASSERT(false);
            break;
        }
    }
}

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

void test12()
{
    {
        std::vector<MyData> vec;
        vec.push_back(MyData(1, "111"));
        vec.push_back(MyData(2, "222"));
        vec.push_back(MyData(3, "333"));

        std::string s;
        Json::encode(vec, s);

        printf("%s\n", s.c_str());
    }

    {
        std::vector<MyData*> vec;
        vec.push_back(new MyData(100, "AAA"));
        vec.push_back(new MyData(200, "BBB"));
        vec.push_back(new MyData(300, "CCC"));

        std::string s;
        Json::encode(vec, s);

        printf("%s\n", s.c_str());

        for (size_t index= 0; index < vec.size(); ++index)
        {
            delete vec[index];
        }
    }

    {
        MyData myData1(777, "VVV");
        Json::Value v = myData1;

        MyData* myDataPtr = v.getUserValuePtr<MyData>();

        JSONITY_ASSERT(myData1.getData1() == myDataPtr->getData1());
        JSONITY_ASSERT(myData1.getData2() == myDataPtr->getData2());

        MyData& myData2 = v.getUserValue<MyData>();

        JSONITY_ASSERT(myData1.getData1() == myData2.getData1());
        JSONITY_ASSERT(myData1.getData2() == myData2.getData2());
    }

    {
        MyData* myDataPtr1 = new MyData(777, "VVV");
        Json::Value v = myDataPtr1;

        MyData* myDataPtr2 = v.getUserValuePtr<MyData>();

        JSONITY_ASSERT(myDataPtr1 == myDataPtr2);

        MyData& myData1 = v.getUserValue<MyData>();

        JSONITY_ASSERT(myDataPtr1 == &myData1);

        delete myDataPtr1;
    }

    return;
}

void test13()
{
#ifdef JSONITY_SUPPORT_CXX_11

    {
        std::array<int, 3> arr;
        arr[0] = 100;
        arr[1] = 200;
        arr[2] = 300;

        std::string jsonStr;
        Json::encode(arr, jsonStr);

        JSONITY_ASSERT(jsonStr == "[100,200,300]");
    }

    {
        std::forward_list<int> flist;
        flist.insert_after(flist.before_begin(), 300);
        flist.insert_after(flist.before_begin(), 200);
        flist.insert_after(flist.before_begin(), 100);

        std::string jsonStr;
        Json::encode(flist, jsonStr);

        JSONITY_ASSERT(jsonStr == "[100,200,300]");
    }

    {
        Json::Object obj;

        std::vector<int> vec(3);
        vec[0] = 100;
        vec[1] = 200;
        vec[2] = 300;
        obj["vec"] = vec;

        std::list<int> list;
        list.push_back(400);
        list.push_back(500);
        list.push_back(600);
        obj["list"] = list;

        std::set<int> set;
        set.insert(700);
        set.insert(800);
        set.insert(900);
        obj["set"] = set;

        std::multiset<int> mset;
        mset.insert(777);
        mset.insert(888);
        mset.insert(999);
        obj["mset"] = mset;

        std::array<std::string, 3> arr;
        arr[0] = "A00";
        arr[1] = "B00";
        arr[2] = "C00";
        obj["arr"] = arr;

        std::forward_list<std::string> flist;
        flist.insert_after(flist.before_begin(), "F00");
        flist.insert_after(flist.before_begin(), "E00");
        flist.insert_after(flist.before_begin(), "D00");
        obj["flist"] = flist;

        std::deque<double> deq;
        deq.push_back(0.1);
        deq.push_back(0.2);
        deq.push_back(0.3);
        obj["deq"] = deq;

        std::unordered_set<int> uo_set;
        uo_set.insert(123);
        uo_set.insert(456);
        uo_set.insert(789);
        obj["uo_set"] = uo_set;

        std::unordered_multiset<std::string> uo_mset;
        uo_mset.insert("red");
        uo_mset.insert("blue");
        uo_mset.insert("yellow");
        obj["uo_mset"] = uo_mset;

        std::string jsonStr;
        Json::encode(obj, jsonStr);

        JSONITY_ASSERT(Json::equal(obj,
            "{"
                "\"vec\": [ 100, 200, 300 ],"
                "\"list\": [ 400, 500, 600 ],"
                "\"set\": [ 700, 800, 900 ],"
                "\"mset\": [ 777, 888, 999 ],"
                "\"arr\": [ \"A00\", \"B00\", \"C00\" ],"
                "\"flist\": [ \"D00\", \"E00\", \"F00\" ],"
                "\"deq\": [ 0.1, 0.2, 0.3 ],"
                "\"uo_set\": [ 123, 456, 789 ],"
                "\"uo_mset\": [ \"red\", \"blue\", \"yellow\" ]"
            "}"));
    }

#endif
}

// User Object
class MyData2 : public Json::UserValue<MyData2>
{
public:
    MyData2()
    {
    }
    MyData2(const std::vector<std::string>& data)
    {
        data_ = data;
    }
    MyData2(const MyData2& other)
    {
        data_ = other.data_;
    }
    ~MyData2()
    {
    }

protected:

    // Encode
    virtual void encode(Json::EncodeContext& ctx) const
    {
        Json::encodeArray(ctx, data_);
    }

private:
    std::vector<std::string> data_;
};

void test14()
{
    {
        std::map<int, MyData2> map;

        std::vector<std::string> vec(3);
        vec[0] = "TEST1";
        vec[1] = "TEST2";
        vec[2] = "TEST3";

        MyData2 myData(vec);

        map[100] = myData;

        std::string jsonStr;
        Json::encode(map, jsonStr);

        return ;
    }
}

void test15()
{
    {
        std::string jsonStr =
            "{\n"
                "\"test1\" : 100\n"
                "\"test2\" : 200\n"
            "}";

        Json::Value v;
        Json::Error err;

        if (!Json::decode(jsonStr, v, &err))
        {
            printf("parse error: [%d]\n",
                err.getCursor().getPos() + 1);

            printf("parse error: (%d:%d)\n",
                err.getCursor().getRow() + 1,
                err.getCursor().getCol() + 1);

            JSONITY_ASSERT(err.getCursor().getPos() == 16);
            JSONITY_ASSERT(err.getCursor().getRow() == 2);
            JSONITY_ASSERT(err.getCursor().getCol() == 0);
        }
    }
}

void test16()
{
    {
        std::string jsonStr =
            "{"
                "\"test1\" : 100,"
                "\"test2\" : 200"
            "}";

        std::istringstream iss(jsonStr);

        Json::Value v;
        iss >> v;

        JSONITY_ASSERT(iss);
        JSONITY_ASSERT(Json::equal(v, jsonStr));

        std::ostringstream oss;
        oss << v;

        JSONITY_ASSERT(oss);
        JSONITY_ASSERT(oss.str() == "{\"test1\":100,\"test2\":200}");

        std::cout << v << std::endl;
    }

    {
        Json::Object obj;

        obj["test1"] = 100;
        obj["test2"] = 200;

        std::cout << obj << std::endl;
    }

    {
        Json::Array arr(2);

        arr[0] = 100;
        arr[1] = 200;

        std::cout << arr << std::endl;
    }

    {
        std::map<std::string, std::list<int> > map;

        std::list<int> list;
        list.push_back(100);
        list.push_back(200);

        map["test1"] = list;
        map["test2"] = list;

        std::cout << map << std::endl;
    }

    {
        std::list<std::map<std::string, int> > list;

        std::map<std::string, int> map;
        map["test1"] = 100;
        map["test2"] = 200;

        list.push_back(map);
        list.push_back(map);

        std::cout << list << std::endl;
    }

    return;
}

void test17()
{
    {
        std::wstring wjsonStr1 =
            L"{"
                L"\"test1\" : \"data1\","
                L"\"test2\" : 222,"
                L"\"test3\" : true"
            L"}";

        wJson::Value v;
        JSONITY_ASSERT(wJson::decode(wjsonStr1, v));
        JSONITY_ASSERT(wJson::equal(v, wjsonStr1));

        JSONITY_ASSERT(v.hasName(L"test1"));
        JSONITY_ASSERT(v[L"test1"] == L"data1");

        std::wstring wjsonStr2;
        wJson::encode(v, wjsonStr2);

        JSONITY_ASSERT(wJson::equal(v, wjsonStr2));
    }

#if defined(JSONITY_SUPPORT_CXX_11) && !defined(JSONITY_COMPILER_MSVC)
	{
        std::u16string u16jsonStr1 =
            u"{"
                u"\"test1\" : \"data1\","
                u"\"test2\" : 222,"
                u"\"test3\" : true"
            u"}";

        u16Json::Value v;
        JSONITY_ASSERT(u16Json::decode(u16jsonStr1, v));
        JSONITY_ASSERT(u16Json::equal(v, u16jsonStr1));

        JSONITY_ASSERT(v.hasName(u"test1"));
        JSONITY_ASSERT(v[u"test1"] == u"data1");
        JSONITY_ASSERT(v[u"test2"].toString() == u"222");
        JSONITY_ASSERT(v[u"test3"].toString() == u"1");
        JSONITY_ASSERT(v[u"test3"].toNumber() == 1);

        std::u16string u16jsonStr2;
        u16Json::encode(v, u16jsonStr2);

        JSONITY_ASSERT(u16Json::equal(v, u16jsonStr2));
    }

    {
        std::u32string u32jsonStr1 =
            U"{"
                U"\"test1\" : \"data1\","
                U"\"test2\" : 222,"
                U"\"test3\" : true"
            U"}";

        u32Json::Value v;
        JSONITY_ASSERT(u32Json::decode(u32jsonStr1, v));
        JSONITY_ASSERT(u32Json::equal(v, u32jsonStr1));

        JSONITY_ASSERT(v.hasName(U"test1"));
        JSONITY_ASSERT(v[U"test1"] == U"data1");
        JSONITY_ASSERT(v[U"test2"].toString() == U"222");
        JSONITY_ASSERT(v[U"test3"].toString() == U"1");
        JSONITY_ASSERT(v[U"test3"].toNumber() == 1);

        std::u32string u32jsonStr2;
        u32Json::encode(v, u32jsonStr2);

        JSONITY_ASSERT(u32Json::equal(v, u32jsonStr2));

	}

    {
    }
#endif
}

void example1_1()
{
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

    JSONITY_ASSERT(check == true);
    JSONITY_ASSERT(size == 4);
    JSONITY_ASSERT(array_size == 3);
    JSONITY_ASSERT(n1 == 100);
    JSONITY_ASSERT(n2 == 100);
    JSONITY_ASSERT(b1 == true);
    JSONITY_ASSERT(b2 == true);
    JSONITY_ASSERT(arr_n1 == 10);
    JSONITY_ASSERT(arr_n2 == 20);
    JSONITY_ASSERT(arr_n3 == 30);
    JSONITY_ASSERT(str1 == "data");
    JSONITY_ASSERT(str2 == "data");
    JSONITY_ASSERT(arr1 == arr2);

    return;
}

void example1_5()
{
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

    JSONITY_ASSERT(d == -3.14);
    JSONITY_ASSERT(str == "aaaa");
    JSONITY_ASSERT(b == true);
    JSONITY_ASSERT(n == 600);

    std::list<Json::Value> listVal;
    v.findRecursive("subdata1", listVal);

    JSONITY_ASSERT(listVal.size() == 1);

    Json::Value& v2 = *listVal.begin();

    n = v2[0]; // 600

    JSONITY_ASSERT(n == 600);

    return;
}

void example2_1()
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

    JSONITY_ASSERT(jsonStr == "{\"name1\":100,\"name2\":true,\"name3\":\"data_string\",\"name4\":null,"
      "\"name5\":[\"test\",-400,false],\"name6\":{\"xxx\":-1.5,\"yyy\":true,\"zzz\":\"test_test\"},"
      "\"name7\":[444,777]}");

    return;
}

void example2_4()
{
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

    JSONITY_ASSERT(jsonStr == "{\"name1\":\"777-99\",\"name2\":\"AAA-55\"}");

    return;
}

void example2_5()
{
    std::map<std::string, std::string> map;  // any STL type (map, vector, list, set, ...)

    map["name1"] = "data1";
    map["name2"] = "data2";
    map["name3"] = "data3";

    std::string jsonStr;
    Json::encode(map, jsonStr);  // serialize

    // jsonStr == {"name1":"data1","name2":"data2","name3":"data3"}

    JSONITY_ASSERT(jsonStr == "{\"name1\":\"data1\",\"name2\":\"data2\",\"name3\":\"data3\"}");
}

void example2_6()
{
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

    JSONITY_ASSERT(jsonStr == "{\"name\":[\"666-66\",\"777-77\",\"888-88\"]}");

    return;
}

void example2_7()
{
    std::vector<int> list;  // any STL type (map, vector, list, set, ...)

    list.push_back(100);
    list.push_back(200);
    list.push_back(300);

    std::string jsonStr;
    Json::encode(list, jsonStr);  // serialize

    // jsonStr == [100,200,300]

    JSONITY_ASSERT(jsonStr == "[100,200,300]");

    return;
}

void example2_8()
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

    return;
}

void example3_1()
{
    std::string jsonStr =
        "{"
            "\"aaa\": 100,"
            "\"bbb\": \"data\""
        "}";

    Json::Value v;
    Json::decode(jsonStr, v);

    bool result = Json::equal(v, jsonStr);  // true

    JSONITY_ASSERT(result);
}

void example3_2()
{
    std::list<int> list;
    list.push_back(100);
    list.push_back(200);
    list.push_back(300);

    bool result1 = Json::equal(list, "[ 100, 200, 300 ]");  // true
    
    bool result2 = Json::equal(list, "[ 300, 100, 200 ]");  // true

    bool result3 = Json::equal(list, "[ 300, 100, 200 ]", false);  // false

    JSONITY_ASSERT(result1);
    JSONITY_ASSERT(result2);
    JSONITY_ASSERT(!result3);
}

#ifdef JSONITY_OS_WINDOWS
int _tmain(int, _TCHAR**) {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#else
int main(int, char**) {
#endif

#ifdef JSONITY_SUPPORT_CXX_11
    printf("c++11\n");
#endif

    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    test10();
    test11();
    test12();
    test13();
    test14();
    test15();
    test16();
    test17();

#ifdef _JSONITY_TEST_
    test_unicode();
    read_file();
#endif

    example1_1();
    example1_5();

    example2_1();
    example2_4();
    example2_5();
    example2_6();
    example2_7();
    example2_8();

    example3_1();
    example3_2();

    return 0;
}

