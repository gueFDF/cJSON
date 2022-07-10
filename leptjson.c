#include"leptjson.h"

#ifndef LEPT_PARSE_STACK_INIT_SIZE
#define LEPT_PARSE_STACK_INIT_SIZE 256
#endif

#define EXPECT(c,ch)  do{assert(*c->json==(ch));c->json++;}while(0)

#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')
#define PUTC(c,ch)  do{*(char*)lept_context_push(c,sizeof(char))=(ch);}while(0)
static void lept_parse_whitespace(lept_context* c)
{
    const char* p=c->json;
    while(*p==' '||*p=='\n'||*p=='\t'||*p=='\r')
    {
        p++;
    }
    c->json=p;
}

static int lept_parse_number(lept_context*c,lept_value*v)
{
    const char *p=c->json;
    if(*p=='-') p++;
    if(*p=='0') p++;
    else
    {
        if(!ISDIGIT1TO9(*p))
        return LEPT_PARSE_INVALID_VALUE;
        for(p++;ISDIGIT(*p);p++);
    }
    if(*p=='.')
    {
        p++;
        if(!ISDIGIT(*p))
        return LEPT_PARSE_INVALID_VALUE;
        for(p++;ISDIGIT(*p);p++);
    }
    if(*p=='e'||*p=='E')
    {
        p++;
        if(*p=='-'||*p=='+')
        p++;
        if(!ISDIGIT(*p))
        return LEPT_PARSE_EXPECT_VALUE;
        for(p++;ISDIGIT(*p);p++);
    }
    errno=0;
    v->u.n=strtod(c->json,NULL);
    if(errno==ERANGE&&( v->u.n==HUGE_VAL|| v->u.n==-HUGE_VAL))
        return LEPT_PARSE_NUMBER_TOO_BIG;
    v->type=LEPT_NUMBER;
    c->json=p;
    return LEPT_PARSE_OK;

}
static int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type type) 
{
    EXPECT(c,literal[0]);
    size_t i;
    for(i=0;literal[i+1];i++)
    {
        if(c->json[i]!=literal[i+1])
        return LEPT_PARSE_INVALID_VALUE;
    }
    c->json+=i;
    v->type=type;
    return LEPT_PARSE_OK;
}
static int lept_parse_object(lept_context* c, lept_value* v)
{
    int i;
    size_t size;
    lept_member m;
    int ret;
    EXPECT(c,'{');
    lept_parse_whitespace(c);
    if(*c->json=='}')
    {
        c->json++;
        v->type=LEPT_OBJECT;
        v->u.o.m=0;
        v->u.o.size=0;
        return LEPT_PARSE_OK;
    }
    m.k=NULL;
    size=0;
    for(;;)
    {   
        char*str;
        lept_init(&m.v);
        if(*c->json!='"')
        {
            ret=LEPT_PARSE_MISS_KEY;
            break;
        }
        if(lept_parse_string_raw(c,&str,&m.klen)!=LEPT_PARSE_OK)
            break;
        memcpy(m.k=(char*)malloc(m.klen+1),str,m.klen);
        m.k[m.klen]='\0';
        lept_parse_whitespace(c);
        if(*c->json!=':')
        {
            ret=LEPT_PARSE_MISS_COLON;
            break;
        }
        c->json++;
        lept_parse_whitespace(c);
        if((ret=lept_parse_value(c,&m.v))!=LEPT_PARSE_OK)
            break;
        memcpy(lept_context_push(c,sizeof(lept_member)),&m,sizeof(lept_member));
        size++;
        m.k=NULL;
        lept_parse_whitespace(c);
        if(*c->json==',')
        {
            c->json++;
            lept_parse_whitespace(c);
        }
        else if(*c->json=='}')
        {
            c->json++;
            v->type=LEPT_OBJECT;
            v->u.o.size=size;
            size*=sizeof(lept_member);
            memcpy(v->u.o.m=(lept_member*)malloc(size),lept_context_pop(c,size),size);
            return LEPT_PARSE_OK;
        }
        else
        {
            ret=LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET;
            break;
        }
    }
      free(m.k);
    for (i = 0; i < size; i++) 
    {
        lept_member* m = (lept_member*)lept_context_pop(c, sizeof(lept_member));
        free(m->k);
        lept_free(&m->v);
    }
    v->type = LEPT_NULL;
    return ret;
}
static int lept_parse_value(lept_context* c, lept_value* v) 
{
    switch (*c->json) 
    {
        case 't':  
            return lept_parse_literal(c,v,"true",LEPT_TRUE);
        case 'f':  
            return lept_parse_literal(c,v,"false",LEPT_FALSE);
        case 'n':  
            return lept_parse_literal(c,v,"null",LEPT_NULL);
        case '\0': 
            return LEPT_PARSE_EXPECT_VALUE;
        case '"':  
            return lept_parse_string(c, v);
        case '[':
            return lept_parse_array(c,v);
        case '{':
            return lept_parse_object(c,v);
        default:   
            return lept_parse_number(c,v);
    }
}
lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}
double lept_get_number(const lept_value* v)
{
    assert(v!=NULL&&v->type==LEPT_NUMBER);
    return  v->u.n;
}
const char*lept_get_string(const lept_value*v)
{
    assert(v!=NULL);
    return v->u.s.s;
}
size_t lept_get_string_length(const lept_value*v)
{
    assert(v!=NULL&&v->u.s.s!=NULL);
    return v->u.s.len;
}
int lept_get_boolean(const lept_value* v) 
{
    assert(v != NULL && (v->type == LEPT_TRUE || v->type == LEPT_FALSE));
    return v->type == LEPT_TRUE;
}

void lept_set_boolean(lept_value* v, int b) 
{
    lept_free(v);
    v->type = b ? LEPT_TRUE : LEPT_FALSE;
}
size_t lept_get_array_size(const lept_value*v)
{
    assert(v!=NULL&&v->type==LEPT_ARRAY);
    return v->u.a.size;
}
lept_value* lept_get_array_element(const lept_value*v,size_t index)
{
    assert(v!=NULL&&v->type==LEPT_ARRAY);
    assert(index<v->u.a.size);
    return &v->u.a.e[index];
}
void lept_free(lept_value* v) {
    size_t i;
    assert(v != NULL);
    switch (v->type) {
        case LEPT_STRING:
            free(v->u.s.s);
            break;
        case LEPT_ARRAY:
            for (i = 0; i < v->u.a.size; i++)
                lept_free(&v->u.a.e[i]);
            free(v->u.a.e);
            break;
        case LEPT_OBJECT:
            for (i = 0; i < v->u.o.size; i++) {
                free(v->u.o.m[i].k);
                lept_free(&v->u.o.m[i].v);
            }
            free(v->u.o.m);
            break;
        default: break;
    }
    v->type = LEPT_NULL;
}
static int lept_parse_array(lept_context*c,lept_value*v)
{
    size_t size=0;
    int i=0;
    int ret;
    EXPECT(c,'[');
    lept_parse_whitespace(c);
    if(*c->json==']')
    {
        c->json++;
        v->type=LEPT_ARRAY;
        v->u.a.size=0;
        v->u.a.e=NULL;
        return LEPT_PARSE_OK;
    }
    for(;;)
    {
        lept_value e;
        lept_init(&e);
        if((ret=lept_parse_value(c,&e))!=LEPT_PARSE_OK)
           break;
        memcpy(lept_context_push(c,sizeof(lept_value)),&e,sizeof(lept_value));
        lept_parse_whitespace(c);
        size++;
        if(*c->json==',')
        {
            c->json++;
            lept_parse_whitespace(c);
        }
        else if(*c->json==']')
        {
            c->json++;
            v->type=LEPT_ARRAY;
            v->u.a.size=size;
            size*=sizeof(lept_value);
            memcpy(v->u.a.e=(lept_value*)malloc(size),lept_context_pop(c,size),size);
            return LEPT_PARSE_OK;
        }
        else 
        {
            ret=LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
            break;
        }
    }
    for(i=0;i<size;i++)
    {
        lept_free((lept_value*)lept_context_pop(c,sizeof(lept_value)));
    }
    return ret;

}

void lept_set_string(lept_value* v, const char* s, size_t len)
{
    assert(v!=NULL&&(s!=NULL||len==0));
    lept_free(v);//清理空间
    v->u.s.s=(char*)malloc(len+1);
    memcpy(v->u.s.s,s,len);
    v->u.s.s[len]='\0';
    v->u.s.len=len;
    v->type=LEPT_STRING;
}
static void*lept_context_push(lept_context*c,size_t size)
{
    void*ret;
    assert(size>0);
    if(c->top+size>=c->size)
    {
        if(c->size==0)
        c->size=LEPT_PARSE_STACK_INIT_SIZE;
        while(c->top+size>=c->size)
        c->size+=c->size>>1;
        c->stack=(char*)realloc(c->stack,c->size);
    }
    ret=c->stack+c->top;
    c->top+=size;
    return ret;
}
static void*lept_context_pop(lept_context*c,size_t size)
{
    assert(c->top>=size);
    c->top-=size;
    return c->stack+c->top;
}


static int lept_parse_string_raw(lept_context* c, char** str, size_t* Len) 
{
    unsigned u,u2;
    size_t head=c->top,len;
    const char*p;
    EXPECT(c,'\"');
    p=c->json;
    
    for(;;)
    {
        char ch=*p++;
        switch (ch)
        {
            case '\"':
                len=c->top-head;
                *str=(char*)lept_context_pop(c,len);
                *Len=len;
                c->json=p;
                return LEPT_PARSE_OK;
            case '\\':
                switch(*p++)
                {
                    case '\"': PUTC(c,'\"'); break;
                    case '\\':PUTC(c,'\\'); break;
                    case '/': PUTC(c,'/'); break;
                    case 'b':PUTC(c,'\b');break;
                    case 'f': PUTC(c,'\f');break;
                    case 'n': PUTC(c,'\n');break;
                    case 'r': PUTC(c,'\r');break;
                    case 't': PUTC(c,'\t');break;
                    case 'u':
                        if(!(p=lept_parse_hex4(p,&u)))
                        STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX);
                        if(u>=0xD800&&u<=0xDBFF)
                        {
                            if(*p++!='\\')
                            STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
                            if(*p++!='u')
                            STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
                            if(!(p=lept_parse_hex4(p,&u2)))
                            STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX);
                            if (u2 < 0xDC00 || u2 > 0xDFFF)
                            STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
                            u=0x10000+(u-0xD800)*0x400+(u2-0xDC00);
                        }
        
                        lept_encode_utf8(c,u);
                        break;
                    default:
                        c->top=head;
                        return LEPT_PARSE_INVALID_STRING_ESCAPE;
                }
                break;
            case '\0':
                c->top=head;
                return LEPT_PARSE_MISS_QUOTATION_MARK;
            default:
                if((unsigned char)ch<0x20)
                {
                    c->top=head;
                    return LEPT_PARSE_INVALID_STRING_CHAR;
                }
                PUTC(c,ch);
                break;
        }
    }
}

static int lept_parse_string(lept_context* c, lept_value* v) 
{
    int ret;
    char* s;
    size_t len;
    if ((ret = lept_parse_string_raw(c, &s, &len)) == LEPT_PARSE_OK)
        lept_set_string(v, s, len);
    return ret;
}

static const char* lept_parse_hex4(const char* p, unsigned* u)
{
    int i=0;
    char ch;
    *u=0x0000;
    for(i=0;i<4;i++)
    {
        ch=*p++;
        *u<<=4;
        if(ch>='0'&&ch<='9') *u|=ch-'0';
        else if(ch>='a'&&ch<='f') *u|=ch-'a'+10;
        else if(ch>='A'&&ch<='F') *u|=ch-'A'+10;
        else return NULL;
    }
    return p;
}
static void lept_encode_utf8(lept_context* c, unsigned u)
{
    if(u<=0x7F)
        PUTC(c,u&0xFF);
    else if(u<=0x7FF)
    {
        PUTC(c,0xC0|((u>>6) )&0xFF);
        PUTC(c,0x80|( u     &0x3F));
    }
    else if(u<=0xFFFF)
    {
        PUTC(c,0xE0|((u>>12))&0xFF);
        PUTC(c,0x80|((u>>6 ))&0x3F);
        PUTC(c,0x80|( u     &0x3F));
    }
    else if(u<=0x10FFFF)
    {
        PUTC(c,0xF0|((u>>18))&0XFF);
        PUTC(c,0x80|((u>>12))&0x3F);
        PUTC(c,0x80|((u>>6 ))&0x3F);
        PUTC(c,0x80|( u     &0x3F));
    }
}
#ifndef LEPT_PARSE_STRINGIFY_INIT_SIZE
#define LEPT_PARSE_STRINGIFY_INIT_SIZE 256
#endif

int lept_stringify(const lept_value* v, char** json, size_t* length) {
    lept_context c;
    int ret;
    assert(v != NULL);
    assert(json != NULL);
    c.stack = (char*)malloc(c.size = LEPT_PARSE_STRINGIFY_INIT_SIZE);
    c.top = 0;
    if ((ret = lept_stringify_value(&c, v)) != LEPT_STRINGIFY_OK) {
        free(c.stack);
        *json = NULL;
        return ret;
    }
    if (length)
        *length = c.top;
    PUTC(&c, '\0');
    *json = c.stack;
    return LEPT_STRINGIFY_OK;
}
#define PUTS(c, s, len)     memcpy(lept_context_push(c, len), s, len)
static void lept_stringify_string(lept_context* c, const char* s, size_t len)
{
    size_t i,size;
    char*head,*p;
    assert(s!=NULL);
    p=head=lept_context_push(c,size=len*6+2);
    *p++='"';
    static const char hex_digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    for(i=0;i<len;i++)
    {
        unsigned char ch=(unsigned char)s[i];
        switch(ch)
        {
            case '\"': *p++ = '\\'; *p++ = '\"'; break;
            case '\\': *p++ = '\\'; *p++ = '\\'; break;
            case '\b': *p++ = '\\'; *p++ = 'b';  break;
            case '\f': *p++ = '\\'; *p++ = 'f';  break;
            case '\n': *p++ = '\\'; *p++ = 'n';  break;
            case '\r': *p++ = '\\'; *p++ = 'r';  break;
            case '\t': *p++ = '\\'; *p++ = 't';  break;

            
            default:
            {
                if(ch<0x20)
                {
                    *p++='\\';
                    *p++='u';
                    *p++='0';
                    *p++='0';
                    *p++=hex_digits[ch>>4];
                    *p++=hex_digits[ch&15];
                }
                else 
                    *p++=ch;
            }
        }
    }
    *p++='"';
    c->top-=size-(p-head);
}
static int lept_stringify_value(lept_context* c, const lept_value* v) {
    size_t i;
    int ret;
    switch (v->type) {
        case LEPT_NULL:   PUTS(c, "null",  4); break;
        case LEPT_FALSE:  PUTS(c, "false", 5); break;
        case LEPT_TRUE:   PUTS(c, "true",  4); break;
        case LEPT_NUMBER: 
        {
            c->top-=32-sprintf(lept_context_push(c,32),"%.17g",v->u.n);
            break;
        }
        case LEPT_STRING:
        {
            lept_stringify_string(c,v->u.s.s,v->u.s.len);
            break;
        }
        case LEPT_ARRAY:
        {
            PUTC(c,'[');
            for(i=0;i<v->u.a.size;i++)
            {
               if(i>0)
                PUTC(c,',');
               lept_stringify_value(c,&v->u.a.e[i]);
            }
            PUTC(c,']');
            break;
        }
        case LEPT_OBJECT:
        {
            PUTC(c, '{');
            for (i = 0; i < v->u.o.size; i++) {
                if (i > 0)
                    PUTC(c, ',');
                lept_stringify_string(c, v->u.o.m[i].k, v->u.o.m[i].klen);
                PUTC(c, ':');
                lept_stringify_value(c, &v->u.o.m[i].v);
            }
            PUTC(c, '}');
            break;
        }
    }
    return LEPT_STRINGIFY_OK;
}
#define LEPT_KEY_NOT_EXIST ((size_t)-1)

size_t lept_find_object_index(const lept_value* v, const char* key, size_t klen) 
{
    assert(v!=NULL&&v->type==LEPT_OBJECT&&key!=NULL);
    size_t i;
    for(i=0;i<v->u.o.size;i++)
    {
        if(v->u.o.m[i].klen==klen&&memcmp(v->u.o.m[i].k,key,klen)==0)
            return 1;
    }
    return  LEPT_KEY_NOT_EXIST;

}
lept_value* lept_find_object_value(const lept_value*v,const char*key,size_t klen)
{
    size_t index=lept_find_object_index(v,key,klen);
    return index!=LEPT_KEY_NOT_EXIST?&v->u.o.m[index].v:NULL;
}
int lept_is_equal(const lept_value* lhs, const lept_value* rhs)
{
    size_t i,j;
    int flag=0;
    assert(lhs!=NULL&&rhs!=NULL);
    if(lhs->type!=rhs->type)
    {
        return 0;
    }
    switch(lhs->type)
    {
        case LEPT_STRING:
            return lhs->u.s.len==rhs->u.s.len&&memcmp(lhs->u.s.s,rhs->u.s.s,rhs->u.s.len)==0;
        case LEPT_NUMBER:
            return lhs->u.n==rhs->u.n;
        case LEPT_ARRAY:
            if(lhs->u.a.size!=rhs->u.a.size)
                return 0;
            for(i=0;i<lhs->u.a.size;i++)
            {
                if(!lept_is_equal(&lhs->u.a.e[i],&rhs->u.a.e[i]))
                return 0;
            }
            return 1;
        case LEPT_OBJECT:
            if(lhs->u.o.size!=rhs->u.o.size)
                return 0;
            for(i=0;i<lhs->u.o.size;i++)
            {
                flag=0;
                for(j=0;j<rhs->u.o.size;j++)
                {
                    if(lhs->u.o.m[i].klen==rhs->u.o.m[j].klen)
                    {
                        if(memcmp(lhs->u.o.m[i].k,rhs->u.o.m[j].k,lhs->u.o.m[i].klen)==0)
                        {
                            if(lept_is_equal(&lhs->u.o.m[i].v,&rhs->u.o.m[j].v))
                                flag=1;
                        }
                    }
                }
                if(flag==0)
                return 0;
            }
            return 1;

    }
    return 1;
}
void lept_copy(lept_value* dst, const lept_value* src) {
    size_t i;
    assert(src != NULL);
    switch (src->type) 
    {
        case LEPT_STRING:
            lept_set_string(dst, src->u.s.s, src->u.s.len);
            break;
        case LEPT_ARRAY:
            dst->type=src->type;
            dst->u.a.size=src->u.a.size;
            dst->u.a.e=(lept_value*)malloc(sizeof(lept_value)*src->u.a.size);
            for(i=0;i<dst->u.a.size;i++)
            {
                lept_copy(&dst->u.a.e[i],&src->u.a.e[i]);
            }
            break;
        case LEPT_OBJECT:
            dst->type=src->type;
            dst->u.o.size=src->u.o.size;
            dst->u.o.m=(lept_member*)malloc(sizeof(lept_member)*src->u.a.size);
            for(i=0;i<src->u.o.size;i++)
            {
                dst->u.o.m[i].klen=src->u.o.m[i].klen;
                dst->u.o.m[i].k=(char*)malloc(sizeof(char)*src->u.o.m[i].klen+1);
                memcpy(dst->u.o.m[i].k,src->u.o.m[i].k,src->u.o.m[i].klen+1);
                dst->u.o.m[i].k[src->u.o.m[i].klen]='\0';
                lept_copy(&dst->u.o.m[i].v,&src->u.o.m[i].v);
            }
            break;
        default:
            lept_free(dst);
             dst->type=src->type;
            memcpy(dst, src, sizeof(lept_value));
            break;
    }
}
void lept_set_number(lept_value* v, double n)
{
    v->type=LEPT_NUMBER;
    v->u.n=n;
}
void lept_set_array(lept_value* v, size_t capacity) 
{
    assert(v != NULL);
    lept_free(v);
    v->type = LEPT_ARRAY;
    v->u.a.size = 0;
    v->u.a.capacity = capacity;
    v->u.a.e = capacity > 0 ? (lept_value*)malloc(capacity * sizeof(lept_value)) : NULL;
}
void lept_set_object(lept_value* v, size_t capacity)
{
    assert(v!=NULL);
    lept_free(v);
    v->type=LEPT_OBJECT;
    v->u.o.size=0;
    v->u.o.capacity=capacity;
    v->u.o.m=capacity>0?(lept_member*)malloc(capacity*sizeof(lept_member)):NULL;
}
void lept_reserve_array(lept_value* v, size_t capacity) 
{
    assert(v != NULL && v->type == LEPT_ARRAY);
    if (v->u.a.capacity < capacity) {
        v->u.a.capacity = capacity;
        v->u.a.e = (lept_value*)realloc(v->u.a.e, capacity * sizeof(lept_value));
    }
}
void lept_reserve_object(lept_value* v, size_t capacity)
{
    assert(v!=NULL&&v->type==LEPT_OBJECT);
     if (v->u.o.capacity < capacity) {
        v->u.o.capacity = capacity;
        v->u.o.m = (lept_member*)realloc(v->u.o.m, capacity * sizeof(lept_member));
    }

}
void lept_shrink_array(lept_value* v) 
{
    assert(v != NULL && v->type == LEPT_ARRAY);
    if (v->u.a.capacity > v->u.a.size) 
    {
        v->u.a.capacity = v->u.a.size;
        v->u.a.e = (lept_value*)realloc(v->u.a.e, v->u.a.capacity * sizeof(lept_value));
    }
}
lept_value* lept_pushback_array_element(lept_value* v) {
    assert(v != NULL && v->type == LEPT_ARRAY);
    if (v->u.a.size == v->u.a.capacity)
        lept_reserve_array(v, v->u.a.capacity == 0 ? 1 : v->u.a.capacity * 2);
    lept_init(&v->u.a.e[v->u.a.size]);
    return &v->u.a.e[v->u.a.size++];
}

void lept_popback_array_element(lept_value* v) {
    assert(v != NULL && v->type == LEPT_ARRAY && v->u.a.size > 0);
    lept_free(&v->u.a.e[--v->u.a.size]);
}
size_t lept_get_array_capacity(const lept_value* v)
{
    assert(v != NULL && v->type == LEPT_ARRAY);
    return v->u.a.capacity;
}
size_t lept_get_object_capacity(const lept_value* v)
{
    assert(v!=NULL&&v->type==LEPT_OBJECT);
    return v->u.o.capacity;
}
lept_value* lept_insert_array_element(lept_value* v, size_t index)
{
    lept_pushback_array_element(v);
    size_t i;
    for(i=v->u.a.size-2;i>=index;i--)
    {
        if(i==-1)
        break;
         v->u.a.e[i+1]=v->u.a.e[i];
    }
    return &v->u.a.e[index];
}
void lept_erase_array_element(lept_value* v, size_t index, size_t count)
{
    assert(v!=NULL&&v->type==LEPT_ARRAY);
    size_t i;
    v->u.a.size-=count;
    for(i=index;i<v->u.a.size;i++)
    {
        v->u.a.e[i]=v->u.a.e[i+count];
    }
}

void lept_move(lept_value* dst, lept_value* src) 
{
    assert(src!=NULL);
    lept_free(dst);
    memcpy(dst,src,sizeof(lept_value));
    lept_init(src);

}
void lept_clear_array(lept_value* v)
{
    assert(v!=NULL&&v->type==LEPT_ARRAY);
    v->u.a.size=0;
}
void lept_swap(lept_value* lhs, lept_value* rhs)
{
    assert(lhs!=NULL&&rhs!=NULL);
    lept_value temp;
    if(lhs!=rhs)
    {
        memcpy(&temp,lhs,sizeof(lept_value));
    memcpy(lhs,rhs,sizeof(lept_value));
    memcpy(rhs,&temp,sizeof(lept_value));
    }
}
int lept_parse(lept_value* v, const char* json)
{
    lept_context c;
    c.json=json;
    c.top=c.size=0;
    c.stack=NULL;
    v->type=LEPT_NULL;
    lept_init(v);//初始化
    lept_parse_whitespace(&c);
    int ret;//接受返回值
    if((ret=lept_parse_value(&c,v))==LEPT_PARSE_OK)
    {
        lept_parse_whitespace(&c);
        if(*c.json!='\0')
        {
            v->type=LEPT_NULL;
            ret=LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    assert(c.top==0);
    free(c.stack);
    return ret; 
}