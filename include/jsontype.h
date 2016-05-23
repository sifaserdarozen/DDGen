/** @file jsontype.h
    @brief Basic Json reader writer library

    @author Sifa Serdar Ozen
    @date 2016.03.10
    @version 1.0.0
 */
#ifndef JSON_SIS_
#define JSON_SIS_

#include <string>
#include <vector>

// function that will find the very first json key value pair delimeter
std::size_t FindJsonElementDelimeter(const std::string& data, std::size_t start = 0);
void NormalizeString(const std::string& message, std::string& normalized_message);

class JsonValueType;
class JsonType;

class JsonElementType
{
private:
    std::string m_key;
    JsonValueType* m_value_ptr;

public:  
    JsonElementType();
    JsonElementType(std::string element_in_line);
    JsonElementType(std::string key, int value);
    JsonElementType(std::string key, std::string value);
    JsonElementType(std::string key, JsonType& value);
    JsonElementType(const JsonElementType& org);
    virtual std::string Key() const;
    virtual std::string Value() const;
    virtual std::vector<std::string> GetStrArrValue() const;
    virtual void SetValue(const std::string& value);
    virtual std::string ToString() const;
    virtual ~JsonElementType();
};

class JsonType
{
private:
    std::vector<JsonElementType> m_data;
    void FromString(const std::string& data);
public:
    JsonType();
    JsonType(const std::string& data);
    virtual std::string operator[] (const std::string& key) const;
    virtual std::vector<std::string> GetStrArrValue (const std::string& key) const;
    virtual void Add(const std::string& data);
    virtual void Add(const JsonElementType& data);
    virtual void Add(const std::string& key, const std::string& value);
    virtual std::string ToString() const;
    virtual ~JsonType();
};

class JsonValueType
{
private:

public:
    JsonValueType();
    virtual std::string ToString() const = 0;
    virtual std::string Value()const = 0;
    virtual JsonValueType* Clone() const = 0;
    virtual std::vector<std::string> GetStrArr() const;
    virtual ~JsonValueType();

};

class IntJsonValueType : public JsonValueType
{
private:
    int m_value;
public:
    IntJsonValueType(int value = 0);
    virtual std::string ToString() const;
    virtual std::string Value() const;
    virtual IntJsonValueType* Clone() const;
    virtual ~IntJsonValueType();

};

class JsonJsonValueType : public JsonValueType
{
private:
    JsonType m_value;
public:
    JsonJsonValueType(const JsonType& value);
    virtual JsonJsonValueType* Clone() const;
    virtual std::string ToString() const;
    virtual std::string Value() const;
    virtual ~JsonJsonValueType();
};

class NullJsonValueType : public JsonValueType
{
private:

public:
    NullJsonValueType();
    virtual NullJsonValueType* Clone() const;
    virtual std::string ToString() const;
    virtual std::string Value() const;
    virtual ~NullJsonValueType();

};

class StringJsonValueType : public JsonValueType
{
private:
    std::string m_value;
public:
    StringJsonValueType(const std::string& value);
    virtual StringJsonValueType* Clone() const;
    virtual std::string ToString() const;
    virtual std::string Value() const;
    virtual ~StringJsonValueType();
};

class VectorJsonValueType : public JsonValueType
{
private:
    std::vector<JsonValueType*> m_value;
public:
    VectorJsonValueType(const std::vector<JsonValueType*>& value);
    virtual VectorJsonValueType* Clone() const;
    virtual std::string ToString() const;
    virtual std::string Value() const;
    virtual std::vector<std::string> GetStrArr() const;
    virtual ~VectorJsonValueType();
};

#endif