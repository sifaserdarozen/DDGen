/** @file jsontype.h
    @brief Basic Json reader writer library

    @author Sifa Serdar Ozen
    @date 2016.03.10
    @version 1.0.0
 */

#pragma once

#include <string>
#include <vector>

namespace ddgen
{

/** @brief Find delimeter location between json elements
 *
 *  Function returns first delimeter between json elements, starting from given position.
 *  As an example input of with default start postion of 0
 *  {"a":"b","c":{...}}
 *  will return location of ","
 *
 *  @pram data json string
 *  @param start start position of search
 *  @return location of delimeter
 */
std::size_t FindJsonElementDelimeter(const std::string& data, std::size_t start = 0);

/** @brief Function that removes whitespace chracters from json string
 *
 *  This function normalizes json string to make delimeter search and element cast easier
 *  A json string of
 *  {"a": "Ali" , "b":  "c"}
 *  should be normalized as
 *  {"a":"Ali","b":"c"}
 *
 *  @param message json string to be normalized
 *  @param normalized_message normalized string of message
 */
void NormalizeString(const std::string& message, std::string& normalized_message);

class JsonValueType;
class JsonType;

/** @brief Json Element data type
 *
 *  Class for json element data type. Only string is supported for Key
 *  and for value, string, json object and array types are possible
 *
 *  @see JsonValueType
 *  @see JsonJsonValueType
 *  @see StringJsonValueType
 *  @see ArrayJsonValueType
 */
class JsonElementType
{
private:
    std::string m_key;
    JsonValueType* m_value_ptr;

public:
    JsonElementType();
    explicit JsonElementType(const std::string& elementInLine);
    JsonElementType(const std::string&  key, int value);
    JsonElementType(const std::string&  key, const std::string& value);
    JsonElementType(const std::string&  key, JsonType& value);
    JsonElementType(const JsonElementType& org);
    JsonElementType& operator = (const JsonElementType& org);
    virtual std::string Key() const;
    virtual std::string Value() const;
    virtual std::vector<std::string> GetStrArrValue() const;
    virtual void SetValue(const std::string& value);
    virtual std::string ToString() const;
    virtual ~JsonElementType();
};

/** @brief Class to hold Json data
 *
 *  Basic Json type implementation. This restricted form
 *  will have 0 to n json elements having string key and
 *  one of {string, vector, json} as values
 *
 *  @see JsonElementType
 *  @see JsonValuetype
 *  @see JsonJsonvalueType
 *  @see ArrayJsonValueType
 *  @see StringJsonValueType
 */
class JsonType
{
private:
    std::vector<JsonElementType> m_data;
    void FromString(const std::string& data);
public:
    JsonType();
    explicit JsonType(const std::string& data);
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
    explicit IntJsonValueType(int value = 0);
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
    explicit JsonJsonValueType(const JsonType& value);
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
    explicit StringJsonValueType(const std::string& value);
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
    explicit VectorJsonValueType(const std::vector<JsonValueType*>& value);
    virtual VectorJsonValueType* Clone() const;
    virtual std::string ToString() const;
    virtual std::string Value() const;
    virtual std::vector<std::string> GetStrArr() const;
    virtual ~VectorJsonValueType();
};
}
