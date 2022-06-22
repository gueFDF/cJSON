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



#define TESTERROR(error,json)\
    do{\
        lept_value v;\
        v.type=LEPT_FALSE;\
        EXPECT_EQ_INT(error,lept_parse(&v,json));\
        EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));\
    }while(0)


#define TEST_NUMBER(expect,json)\
    do{ \
        lept_value v;\
        v.type=LEPT_NULL;\
        EXPECT_EQ_INT(LEPT_PARSE_OK,lept_parse(&v,json));\
        EXPECT_EQ_INT(LEPT_NUMBER,lept_get_type(&v));\
        EXPECT_EQ_DOUBLE(expect,lept_get_number(&v));\
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
}
static void test_parse_null() 
{
    lept_value v;
    v.type = LEPT_FALSE;TESTERROR(LEPT_PARSE_ROOT_NOT_SINGULAR,"null    x");
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "null"));
    EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
}
static void test_parse_true() 
{
    lept_value v;
    v.type = LEPT_FALSE;
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "true"));
    EXPECT_EQ_INT(LEPT_TRUE, lept_get_type(&v));
}

static void test_parse_false() 
{
    lept_value v;
    v.type = LEPT_TRUE;
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "false"));
    EXPECT_EQ_INT(LEPT_FALSE, lept_get_type(&v));
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
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
}
static void test_parse()
{
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_number();
    test_parse_error();
}
int main() {
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}