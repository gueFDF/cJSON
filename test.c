#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"leptjson.h"
static int main_ret=0;
static int test_count=0;
static int test_pass=0;
#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)


#define EXPECT_EQ_INT(expect,actual) EXPECT_EQ_BASE(((expect)==(actual)),expect,actual,"%d")
#define EXPECT_EQ_DOUBLE(expect,actual) EXPECT_EQ_BASE(((expect)==(actual)),expect,actual,"%.17g")
#define EXPECT_EQ_STRING(expect,actual,alength) EXPECT_EQ_BASE(sizeof(expect)-1==(alength)&& memcmp(expect,actual,alength)==0,expect,actual,"%s")
#define EXPECT_TRUE(actual)  EXPECT_EQ_BASE((actual)!=0,"true","false","%s")
#define EXPECT_FALSE(actual)  EXPECT_EQ_BASE((actual)==0,"false","true","%s")

#define TESTERROR(error,json)\
    do{\
        lept_value v;\
        lept_init(&v);\
        EXPECT_EQ_INT(error,lept_parse(&v,json));\
        EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));\
         lept_free(&v);\
    }while(0)




#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%zu")

static void test_parse_array() {
    lept_value v;

    lept_init(&v);
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "[ ]"));
    EXPECT_EQ_INT(LEPT_ARRAY, lept_get_type(&v));
    EXPECT_EQ_SIZE_T(0, lept_get_array_size(&v));
    lept_free(&v);

    size_t i,j;
    lept_init(&v);
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "[ null , false , true , 123 , \"abc\" ]"));
    EXPECT_EQ_INT(LEPT_ARRAY, lept_get_type(&v));
    EXPECT_EQ_SIZE_T(5, lept_get_array_size(&v));
    EXPECT_EQ_INT(LEPT_NULL,lept_get_type(lept_get_array_element(&v,0)));
    EXPECT_EQ_INT(LEPT_FALSE,lept_get_type(lept_get_array_element(&v,1)));
    EXPECT_EQ_INT(LEPT_TRUE,lept_get_type(lept_get_array_element(&v,2)));
    EXPECT_EQ_INT(LEPT_NUMBER,lept_get_type(lept_get_array_element(&v,3)));
    EXPECT_EQ_INT(LEPT_STRING,lept_get_type(lept_get_array_element(&v,4)));
    lept_free(&v);

    lept_init(&v);
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ,[0,1,2,3],[0,1,2,3,4]]"));
    EXPECT_EQ_INT(LEPT_ARRAY, lept_get_type(&v));
    EXPECT_EQ_SIZE_T(6, lept_get_array_size(&v));
    for(i=0;i<4;i++)
    {
        lept_value*a=lept_get_array_element(&v,i);
        EXPECT_EQ_INT(LEPT_ARRAY,lept_get_type(a));
        EXPECT_EQ_SIZE_T(i,lept_get_array_size(a));
        for(j=0;j<i;j++)
        {
            lept_value*e=lept_get_array_element(&v,i);
            EXPECT_EQ_INT(LEPT_ARRAY,lept_get_type(e));
            EXPECT_EQ_SIZE_T(i,lept_get_array_size(e));
        }
    }
    lept_free(&v);
}
#define TEST_NUMBER(expect,json)\
    do{ \
        lept_value v;\
        lept_init(&v);\
        EXPECT_EQ_INT(LEPT_PARSE_OK,lept_parse(&v,json));\
        EXPECT_EQ_INT(LEPT_NUMBER,lept_get_type(&v));\
        EXPECT_EQ_DOUBLE(expect,lept_get_number(&v));\
         lept_free(&v);\
    }while(0)
#define TEST_STRING(expect,json)\
    do{\
        lept_value v;\
        lept_init(&v);\
        EXPECT_EQ_INT(LEPT_PARSE_OK,lept_parse(&v,json));\
        EXPECT_EQ_INT(LEPT_STRING,lept_get_type(&v));\
        EXPECT_EQ_STRING(expect,lept_get_string(&v),lept_get_string_length(&v));\
        lept_free(&v);\
    }while(0)
static void test_parse_error()
{
    TESTERROR(LEPT_PARSE_INVALID_VALUE,"nul");
    TESTERROR(LEPT_PARSE_INVALID_VALUE,"?");
    TESTERROR(LEPT_PARSE_EXPECT_VALUE,"");
    TESTERROR(LEPT_PARSE_EXPECT_VALUE," ");
    TESTERROR(LEPT_PARSE_OK,"null       ");
    TESTERROR(LEPT_PARSE_ROOT_NOT_SINGULAR,"null x");
    TESTERROR(LEPT_PARSE_ROOT_NOT_SINGULAR,"null      x");
    TESTERROR(LEPT_PARSE_INVALID_VALUE, "+0");
    TESTERROR(LEPT_PARSE_INVALID_VALUE, "+1");
    TESTERROR(LEPT_PARSE_INVALID_VALUE, ".123"); 
    TESTERROR(LEPT_PARSE_INVALID_VALUE, "1.");   
    TESTERROR(LEPT_PARSE_INVALID_VALUE, "INF");
    TESTERROR(LEPT_PARSE_INVALID_VALUE, "inf");
    TESTERROR(LEPT_PARSE_INVALID_VALUE, "NAN");
    TESTERROR(LEPT_PARSE_INVALID_VALUE, "nan");
    TESTERROR(LEPT_PARSE_NUMBER_TOO_BIG, "1e309");
    TESTERROR(LEPT_PARSE_NUMBER_TOO_BIG, "-1e309");
    TESTERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.' or nothing */
    TESTERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0x0");
    TESTERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0x123");

   
    TESTERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
    TESTERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
    TESTERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
    TESTERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");

    TESTERROR(LEPT_PARSE_INVALID_STRING_CHAR, "\"\x01\"");
    TESTERROR(LEPT_PARSE_INVALID_STRING_CHAR, "\"\x1F\"");

    TESTERROR(LEPT_PARSE_INVALID_VALUE, "[1,]");
    TESTERROR(LEPT_PARSE_INVALID_VALUE, "[\"a\", nul]");
}

static void test_parse_miss_comma_or_square_bracket() 
{

    TESTERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1");
    TESTERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1}");
    TESTERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1 2");
    TESTERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[[]");

}
static void test_parse_miss_key() {
    TESTERROR(LEPT_PARSE_MISS_KEY, "{:1,");
    TESTERROR(LEPT_PARSE_MISS_KEY, "{1:1,");
    TESTERROR(LEPT_PARSE_MISS_KEY, "{true:1,");
    TESTERROR(LEPT_PARSE_MISS_KEY, "{false:1,");
    TESTERROR(LEPT_PARSE_MISS_KEY, "{null:1,");
    TESTERROR(LEPT_PARSE_MISS_KEY, "{[]:1,");
    TESTERROR(LEPT_PARSE_MISS_KEY, "{{}:1,");
    TESTERROR(LEPT_PARSE_MISS_KEY, "{\"a\":1,");
}

static void test_parse_miss_colon() {
    TESTERROR(LEPT_PARSE_MISS_COLON, "{\"a\"}");
    TESTERROR(LEPT_PARSE_MISS_COLON, "{\"a\",\"b\"}");
}

static void test_parse_miss_comma_or_curly_bracket() {
    TESTERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1");
    TESTERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1]");
    TESTERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1 \"b\"");
    TESTERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":{}");
}
static void test_parse_null() 
{
    lept_value v;
    v.type = LEPT_FALSE;
    TESTERROR(LEPT_PARSE_ROOT_NOT_SINGULAR,"null    x");
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "null"));
    EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
    lept_free(&v);
}
static void test_parse_true() 
{
    lept_value v;
    v.type = LEPT_FALSE;
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "true"));
    EXPECT_EQ_INT(LEPT_TRUE, lept_get_type(&v));
    lept_free(&v);
}

static void test_parse_false() 
{
    lept_value v;
    lept_init(&v);
    v.type = LEPT_TRUE;
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "false"));
    EXPECT_EQ_INT(LEPT_FALSE, lept_get_type(&v));
    lept_free(&v);
}

static void test_access_string() {
    lept_value v;
    lept_init(&v);
    lept_set_string(&v, "", 0);
    EXPECT_EQ_STRING("", lept_get_string(&v), lept_get_string_length(&v));
    lept_set_string(&v, "Hello", 5);
    EXPECT_EQ_STRING("Hello", lept_get_string(&v), lept_get_string_length(&v));
    lept_free(&v);

}
static void test_parse_string() 
{
    TEST_STRING("", "\"\"");
    TEST_STRING("Hello", "\"Hello\"");

    TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");


    TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
    TEST_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
    TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
    TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}


static void test_parse_number() {
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); 


    TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    TEST_NUMBER( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_NUMBER( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_NUMBER( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_NUMBER( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}


static void test_parse_invalid_unicode_hex() 
{
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u0\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u01\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u012\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u/000\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\uG000\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u0/00\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u0G00\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u00/0\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u00G0\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u000/\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u000G\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u 123\"");
}

static void test_parse_invalid_unicode_surrogate() 
{
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uDBFF\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\\\\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uDBFF\"");
    TESTERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uE000\"");
}


static void test_access_boolean() 
{
    lept_value v;
    lept_init(&v);
    lept_set_boolean(&v, 1);
    EXPECT_TRUE(lept_get_boolean(&v));
    lept_set_boolean(&v, 0);
    EXPECT_FALSE(lept_get_boolean(&v));

    lept_free(&v);
}
#define TEST_ROUNDTRIP(json)\
    do {\
        lept_value v;\
        char* json2;\
        size_t length;\
        lept_init(&v);\
        EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, json));\
        EXPECT_EQ_INT(LEPT_STRINGIFY_OK, lept_stringify(&v, &json2, &length));\
        EXPECT_EQ_STRING(json, json2, length);\
        lept_free(&v);\
        free(json2);\
    } while(0)

static void test_stringify_number() {
    TEST_ROUNDTRIP("0");
    TEST_ROUNDTRIP("-0");
    TEST_ROUNDTRIP("1");
    TEST_ROUNDTRIP("-1");
    TEST_ROUNDTRIP("1.5");
    TEST_ROUNDTRIP("-1.5");
    TEST_ROUNDTRIP("3.25");
    TEST_ROUNDTRIP("1e+20");
    TEST_ROUNDTRIP("1.234e+20");
    TEST_ROUNDTRIP("1.234e-20");

    TEST_ROUNDTRIP("1.0000000000000002"); /* the smallest number > 1 */
    TEST_ROUNDTRIP("4.9406564584124654e-324"); /* minimum denormal */
    TEST_ROUNDTRIP("-4.9406564584124654e-324");
    TEST_ROUNDTRIP("2.2250738585072009e-308");  /* Max subnormal double */
    TEST_ROUNDTRIP("-2.2250738585072009e-308");
    TEST_ROUNDTRIP("2.2250738585072014e-308");  /* Min normal positive double */
    TEST_ROUNDTRIP("-2.2250738585072014e-308");
    TEST_ROUNDTRIP("1.7976931348623157e+308");  /* Max double */
    TEST_ROUNDTRIP("-1.7976931348623157e+308");
}

static void test_stringify_string() {
    TEST_ROUNDTRIP("\"\"");
    TEST_ROUNDTRIP("\"Hello\"");
    TEST_ROUNDTRIP("\"Hello\\nWorld\"");
    TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
    TEST_ROUNDTRIP("\"Hello\\u0000World\"");
}

static void test_stringify_array() {
    TEST_ROUNDTRIP("[]");
    TEST_ROUNDTRIP("[null,false,true,123,\"abc\",[1,2,3]]");
}

static void test_stringify_object() {
    TEST_ROUNDTRIP("{}");
    TEST_ROUNDTRIP("{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}");
}

static void test_stringify() {
    TEST_ROUNDTRIP("null");
    TEST_ROUNDTRIP("false");
    TEST_ROUNDTRIP("true");
    test_stringify_number();
    test_stringify_string();
    test_stringify_array();
    test_stringify_object();
}


#define TEST_EQUAL(json1, json2, equality) \
    do {\
        lept_value v1, v2;\
        lept_init(&v1);\
        lept_init(&v2);\
        EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v1, json1));\
        EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v2, json2));\
        EXPECT_EQ_INT(equality, lept_is_equal(&v1, &v2));\
        lept_free(&v1);\
        lept_free(&v2);\
    } while(0)

static void test_equal() {
    TEST_EQUAL("true", "true", 1);
    TEST_EQUAL("true", "false", 0);
    TEST_EQUAL("false", "false", 1);
    TEST_EQUAL("null", "null", 1);
    TEST_EQUAL("null", "0", 0);
    TEST_EQUAL("123", "123", 1);
    TEST_EQUAL("123", "456", 0);
    TEST_EQUAL("\"abc\"", "\"abc\"", 1);
    TEST_EQUAL("\"abc\"", "\"abcd\"", 0);
    TEST_EQUAL("[]", "[]", 1);
    TEST_EQUAL("[]", "null", 0);
    TEST_EQUAL("[1,2,3]", "[1,2,3]", 1);
    TEST_EQUAL("[1,2,3]", "[1,2,3,4]", 0);
    TEST_EQUAL("[[]]", "[[]]", 1);
    TEST_EQUAL("{}", "{}", 1);
    TEST_EQUAL("{}", "null", 0);
    TEST_EQUAL("{}", "[]", 0);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2}", 1);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"b\":2,\"a\":1}", 1);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":3}", 0);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2,\"c\":3}", 0);
    TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":{}}}}", 1);
    TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":[]}}}", 0);
}

static void test_copy() {
    lept_value v1, v2;
    lept_init(&v1);
    lept_parse(&v1, "{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
    lept_init(&v2);
    lept_copy(&v2, &v1);
    EXPECT_TRUE(lept_is_equal(&v2, &v1));
    lept_free(&v1);
    lept_free(&v2);
}

static void test_move() {
    lept_value v1, v2, v3;
    lept_init(&v1);
    lept_parse(&v1, "{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
    lept_init(&v2);
    lept_copy(&v2, &v1);
    lept_init(&v3);
    lept_move(&v3, &v2);
    EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v2));
    EXPECT_TRUE(lept_is_equal(&v3, &v1));
    lept_free(&v1);
    lept_free(&v2);
    lept_free(&v3);
}
static void test_swap() {
    lept_value v1, v2;
    lept_init(&v1);
    lept_init(&v2);
    lept_set_string(&v1, "Hello",  5);
    lept_set_string(&v2, "World!", 6);
    lept_swap(&v1, &v2);
    EXPECT_EQ_STRING("World!", lept_get_string(&v1), lept_get_string_length(&v1));
    EXPECT_EQ_STRING("Hello",  lept_get_string(&v2), lept_get_string_length(&v2));
    lept_free(&v1);
    lept_free(&v2);
}
static void test_access_number() {
    lept_value v;
    lept_init(&v);
    lept_set_string(&v, "a", 1);
    lept_set_number(&v, 1234.5);
    EXPECT_EQ_DOUBLE(1234.5, lept_get_number(&v));
    lept_free(&v);
}
static void test_access_array() {
    lept_value a, e;
    size_t i, j;

    lept_init(&a);

    for (j = 0; j <= 5; j += 5) {
        lept_set_array(&a, j);
        EXPECT_EQ_SIZE_T(0, lept_get_array_size(&a));
        EXPECT_EQ_SIZE_T(j, lept_get_array_capacity(&a));
        for (i = 0; i < 10; i++) {
            lept_init(&e);
            lept_set_number(&e, i);
            lept_move(lept_pushback_array_element(&a), &e);
            lept_free(&e);
        }

        EXPECT_EQ_SIZE_T(10, lept_get_array_size(&a));
        for (i = 0; i < 10; i++)
            EXPECT_EQ_DOUBLE((double)i, lept_get_number(lept_get_array_element(&a, i)));
    }

    lept_popback_array_element(&a);
    EXPECT_EQ_SIZE_T(9, lept_get_array_size(&a));
    for (i = 0; i < 9; i++)
        EXPECT_EQ_DOUBLE((double)i, lept_get_number(lept_get_array_element(&a, i)));

    lept_erase_array_element(&a, 4, 0);
    EXPECT_EQ_SIZE_T(9, lept_get_array_size(&a));
    for (i = 0; i < 9; i++)
        EXPECT_EQ_DOUBLE((double)i, lept_get_number(lept_get_array_element(&a, i)));

    lept_erase_array_element(&a, 8, 1);
    EXPECT_EQ_SIZE_T(8, lept_get_array_size(&a));
    for (i = 0; i < 8; i++)
        EXPECT_EQ_DOUBLE((double)i, lept_get_number(lept_get_array_element(&a, i)));

    lept_erase_array_element(&a, 0, 2);
    EXPECT_EQ_SIZE_T(6, lept_get_array_size(&a));
    for (i = 0; i < 6; i++)
        EXPECT_EQ_DOUBLE((double)i + 2, lept_get_number(lept_get_array_element(&a, i)));


    for (i = 0; i < 2; i++) {
        lept_init(&e);
        lept_set_number(&e, i);
        lept_move(lept_insert_array_element(&a, i), &e);
        lept_free(&e);
    }
    
    EXPECT_EQ_SIZE_T(8, lept_get_array_size(&a));
    for (i = 0; i < 8; i++)
        EXPECT_EQ_DOUBLE((double)i, lept_get_number(lept_get_array_element(&a, i)));

    EXPECT_TRUE(lept_get_array_capacity(&a) > 8);
    lept_shrink_array(&a);
    EXPECT_EQ_SIZE_T(8, lept_get_array_capacity(&a));
    EXPECT_EQ_SIZE_T(8, lept_get_array_size(&a));
    for (i = 0; i < 8; i++)
        EXPECT_EQ_DOUBLE((double)i, lept_get_number(lept_get_array_element(&a, i)));

    lept_set_string(&e, "Hello", 5);
    lept_move(lept_pushback_array_element(&a), &e);     /* Test if element is freed */
    lept_free(&e);

    i = lept_get_array_capacity(&a);
    lept_clear_array(&a);
    EXPECT_EQ_SIZE_T(0, lept_get_array_size(&a));
    EXPECT_EQ_SIZE_T(i, lept_get_array_capacity(&a));   /* capacity remains unchanged */
    lept_shrink_array(&a);
    EXPECT_EQ_SIZE_T(0, lept_get_array_capacity(&a));

    lept_free(&a);
}
static void test_parse()
{
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_number();
    test_parse_error();
    test_access_string();
    test_parse_string();
    test_parse_invalid_unicode_hex();
    test_parse_invalid_unicode_surrogate();
    test_access_boolean();
    test_access_number();
    test_parse_array();
    test_parse_miss_comma_or_square_bracket();
    test_parse_miss_key();
    test_parse_miss_colon();
    test_parse_miss_comma_or_curly_bracket();
    test_stringify();
    test_equal();
    test_copy();
    test_move();
    test_swap();
    test_access_array();
}
int main() {
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}