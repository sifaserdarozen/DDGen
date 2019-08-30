#include "jsontype.h"

#include <cctype>
#include <iostream>

std::size_t FindJsonElementDelimeter(const std::string& data, std::size_t start)
{
    const char* targets = "{}[],";

    int no_of_lp = 0;
    int no_of_lb = 0;

    std::size_t delimeter_pos = data.find_first_of(targets, start);

    while (std::string::npos != delimeter_pos)
    {
        char curr_char = data[delimeter_pos];
        switch (curr_char)
        {
            case '{' :
                no_of_lp ++;
                break;
            case '}' :
                if (no_of_lp <= 0)    // do a sanity check
                {
                    return std::string::npos;
                }
                no_of_lp --;
                break;
            case '[' :
                no_of_lb ++;
                break;
            case ']' :
                if (no_of_lb <= 0)    // do a sanity check
                {
                    return std::string::npos;
                }
                no_of_lb --;
                break;
            case ',':
                if ((0 == no_of_lp) && (0 == no_of_lb))
                {
                    return delimeter_pos;
                }
                break;
            default:
                return std::string::npos;
            break;
        }

        if (delimeter_pos < data.size())
        {
            start = delimeter_pos + 1;
            delimeter_pos = data.find_first_of(targets, start);
        }
        else
        {
            return std::string::npos;
        }

    }

    return std::string::npos;
}

void NormalizeString(const std::string& message, std::string& normalized_message)
{
    normalized_message.clear();

    // remove white spaces, very first and last parenthesis and LF if there exists
    bool in_string = false;
    bool lp_removed = false;
    int number_of_lp = 0;
    const char* char_ptr = message.c_str();
    while (*char_ptr)
    {
        char curr_char = *char_ptr++;

        // remove paranthesis if start of text
        if ('{' == curr_char)
        {
            number_of_lp ++;
            if ((1 == number_of_lp) && (0 == normalized_message.size()))
            {
                lp_removed = true;
                continue;
            }
        }

        if ('}' == curr_char)
        {
            number_of_lp --;
            if ((0 == number_of_lp) && lp_removed)
                break;
        }

        // do space normalisation if already not in string
        if (false == in_string)
        {
            if (false == isspace(curr_char))
                normalized_message.push_back(curr_char);
        }
        else
            normalized_message.push_back(curr_char);

        if ('"' == curr_char)
            in_string = !in_string;
    }
}

JsonElementType::JsonElementType()
{
   m_key = "";
   m_value_ptr = NULL;
}

JsonElementType::JsonElementType(std::string element_in_line)
{
    std::string normalized_element = element_in_line;
    // NormalizeString(element_in_line, normalized_element);

    m_key = "";
    m_value_ptr = NULL;

    // find key and value
    std::size_t pos = normalized_element.find(':');
    if (std::string::npos == pos)
        return;

    std::string key = normalized_element.substr(0, pos);
    std::string value = normalized_element.substr(pos + 1, normalized_element.size() - key.size() -1);

    if (('"' == key[0] ) && ('"' == key[key.size() - 1]))
        key = key.substr(1, key.size() - 2);

    if (('"' == value[0] ) && ('"' == value[value.size() - 1]))
    {
        value = value.substr(1, value.size() - 2);
        m_key = key;
        m_value_ptr = new StringJsonValueType(value);

        return;
    }

    if (('{' == value[0] ) && ('}' == value[value.size() - 1]))
    {
        m_key = key;
        m_value_ptr = new JsonJsonValueType(JsonType(value));
        return;
    }

    if (('[' == value[0] ) && (']' == value[value.size() - 1]))
    {
        //std::cout << "[]" << value << std::endl;
        value = value.substr(1, value.size() - 2);
        m_key = key;
        std::vector<JsonValueType*> json_vector;

        // group messages to , and append to json element vector
        std::size_t start = 0;

        std::size_t pos = FindJsonElementDelimeter(value);

        while (std::string::npos != pos)
        {
            std::string json_element = value.substr(start, pos - start);

            if (('"' == json_element[0]) && ('"' == json_element[json_element.size()-1]))
                json_vector.push_back(new StringJsonValueType(json_element.substr(1, json_element.size()-2)));
            else if (('{' == json_element[0]) && ('}' == json_element[json_element.size()-1]))
                json_vector.push_back(new JsonJsonValueType(JsonType(json_element)));

            start = pos + 1;
            pos = FindJsonElementDelimeter(value, start);
        }

        if (start < value.size())
        {
            std::string json_element = value.substr(start, pos - start);
            if (('"' == json_element[0]) && ('"' == json_element[json_element.size()-1]))
                json_vector.push_back(new StringJsonValueType(json_element.substr(1, json_element.size()-2)));
            else if (('{' == json_element[0]) && ('}' == json_element[json_element.size()-1]))
                json_vector.push_back(new JsonJsonValueType(JsonType(json_element)));
        }

        m_value_ptr = new VectorJsonValueType(json_vector);

        // clear json_element
        for(std::vector<JsonValueType*>::iterator it = json_vector.begin(); it != json_vector.end(); ++it)
        {
            delete (*it);
        }
        json_vector.clear();

        return;
    }
}

JsonElementType::JsonElementType(std::string key, int value)
{
    m_key = key;
    m_value_ptr = new IntJsonValueType(value);
}

JsonElementType::JsonElementType(std::string key, std::string value)
{
    m_key = key;
    m_value_ptr = new StringJsonValueType(value);
}

JsonElementType::JsonElementType(std::string key, JsonType& value)
{
    m_key = key;
    m_value_ptr = new JsonJsonValueType(value);
}

JsonElementType::JsonElementType(const JsonElementType& org)
{
    m_key = org.m_key;
    if (org.m_value_ptr)
        m_value_ptr = org.m_value_ptr -> Clone();
    else
        m_value_ptr = NULL;
}

std::string JsonElementType::Key() const
{
    return m_key;
}

std::string JsonElementType::Value() const
{
    if (m_value_ptr)
        return m_value_ptr -> Value();
    else
        return "null";
}

std::vector<std::string> JsonElementType::GetStrArrValue() const
{
    return m_value_ptr->GetStrArr();
}

void JsonElementType::SetValue(const std::string& value)
{
    if (m_value_ptr)
    {
        delete m_value_ptr;
        m_value_ptr = NULL;
    }

    m_value_ptr = new StringJsonValueType(value);
}

std::string JsonElementType::ToString() const
{
    if (NULL != m_value_ptr)
        return "\"" + m_key + "\"" + ": " + m_value_ptr -> ToString();
    else
        return "\"" + m_key + "\"" + ": " + "\"null\"";
}

JsonElementType::~JsonElementType()
{
    if (m_value_ptr)
    {
        delete m_value_ptr;
        m_value_ptr = NULL;
    }
}

// *******************************************************************************

JsonValueType::JsonValueType()
{

}

std::vector<std::string> JsonValueType::GetStrArr() const
{
    std::vector<std::string> dummy_vec;
    return dummy_vec;
}

JsonValueType::~JsonValueType()
{

}

// ********************************************************************************

IntJsonValueType::IntJsonValueType(int value)
{
    m_value = value;
}

IntJsonValueType* IntJsonValueType::Clone() const
{
    return new IntJsonValueType(m_value);
}

std::string IntJsonValueType::ToString() const
{
    return std::to_string(m_value);
}

std::string IntJsonValueType::Value() const
{
    return ToString();
}

IntJsonValueType::~IntJsonValueType()
{

}

// **********************************************************************************

JsonJsonValueType::JsonJsonValueType(const JsonType& value)
{
    m_value = value;
}

JsonJsonValueType* JsonJsonValueType::Clone() const
{
    return new JsonJsonValueType(m_value);
}

std::string JsonJsonValueType::ToString() const
{
    return m_value.ToString();
}

std::string JsonJsonValueType::Value() const
{
    return ToString();
}

JsonJsonValueType::~JsonJsonValueType()
{

}

// **********************************************************************************

NullJsonValueType::NullJsonValueType()
{

}

NullJsonValueType* NullJsonValueType::Clone() const
{
    return new NullJsonValueType();
}

std::string NullJsonValueType::ToString() const
{
    return "null";
}

std::string NullJsonValueType::Value() const
{
    return ToString();
}

NullJsonValueType::~NullJsonValueType()
{

}

// **********************************************************************************

StringJsonValueType::StringJsonValueType(const std::string& value)
{
    m_value = value;
}

StringJsonValueType* StringJsonValueType::Clone() const
{
    return new StringJsonValueType(m_value);
}

std::string StringJsonValueType::ToString() const
{
    std::string escaped_value = m_value;

    const char* targets = "\\\"";

    std::size_t start = 0;
    std::size_t pos = escaped_value.find_first_of(targets, start);
    while (std::string::npos != pos)
    {
        escaped_value.insert(pos, "\\");
        start = pos + 2;
        if (pos >= escaped_value.size() )
            break;
        pos = escaped_value.find_first_of(targets, start);
    }

    return "\"" + escaped_value + "\"";
}

std::string StringJsonValueType::Value() const
{
    return m_value;
}

StringJsonValueType::~StringJsonValueType()
{

}

// ************************************************************************************

VectorJsonValueType::VectorJsonValueType(const std::vector<JsonValueType*>& value)
{
    for (std::vector<JsonValueType*>::const_iterator itr = value.begin(); itr != value.end(); ++itr)
    {
        m_value.push_back((*itr)->Clone());
    }
}

VectorJsonValueType* VectorJsonValueType::Clone() const
{
    return new VectorJsonValueType(m_value);
}

std::string VectorJsonValueType::ToString() const
{
    std::string value_in_str = "[";

    for (std::vector<JsonValueType*>::const_iterator itr = m_value.begin(); itr != m_value.end(); ++itr)
    {
        if (itr != m_value.begin())
            value_in_str += ", ";

        value_in_str += (*itr)->ToString();
    }

    value_in_str.append("]");

    return value_in_str;
}

std::string VectorJsonValueType::Value() const
{
    return ToString();
}

std::vector<std::string> VectorJsonValueType::GetStrArr() const
{
    std::vector<std::string> str_arr;
    for (std::vector<JsonValueType*>::const_iterator itr = m_value.begin(); itr != m_value.end(); ++itr)
    {
        str_arr.push_back((*itr)->Value());
    }
    return str_arr;
}

VectorJsonValueType::~VectorJsonValueType()
{
    for (std::vector<JsonValueType*>::const_iterator itr = m_value.begin(); itr != m_value.end(); ++itr)
    {
        delete (*itr);
    }
    m_value.clear();
}

// ********************************************************************

JsonType::JsonType()
{
}

JsonType::JsonType(const std::string& data)
{
    FromString(data);
}

std::string JsonType::operator[] (const std::string& key) const
{
    std::string value;

    for (std::vector<JsonElementType>::const_iterator it = m_data.begin(); it != m_data.end(); ++it)
    {
        if (key == it -> Key())
            return it -> Value();
    }

    return value;
}

std::vector<std::string> JsonType::GetStrArrValue (const std::string& key) const
{
    std::vector<std::string> value;

    for (std::vector<JsonElementType>::const_iterator it = m_data.begin(); it != m_data.end(); ++it)
    {
        if (key == it -> Key())
            return it -> GetStrArrValue();
    }
    return value;
}

void JsonType::FromString(const std::string& data)
{
    m_data.clear();
    Add(data);
}

std::string JsonType::ToString() const
{
    std::string value_in_str = "{";

    for (std::vector<JsonElementType>::const_iterator itr = m_data.begin(); itr != m_data.end(); ++itr)
    {
        if (itr != m_data.begin())
            value_in_str += ", ";

        value_in_str += itr -> ToString();
    }

    value_in_str.append("}");

    return value_in_str;
}


void JsonType::Add(const JsonElementType& data)
{
    m_data.push_back(data);
}


void JsonType::Add(const std::string& data)
{
    std::string stripped_data;
    NormalizeString(data, stripped_data);

    // group messages to , and append to json element vector
    std::size_t start = 0;

    std::size_t pos = FindJsonElementDelimeter( stripped_data );

    while (std::string::npos != pos)
    {
        std::string json_element = stripped_data.substr(start, pos - start);
        m_data.push_back(JsonElementType(json_element));
        start = pos + 1;

        pos = FindJsonElementDelimeter( stripped_data, start );
    }

    if (start < stripped_data.size())
    {
        std::string json_element = stripped_data.substr(start, pos - start);
        m_data.push_back(JsonElementType(json_element));
    }
}

void JsonType::Add(const std::string& key, const std::string& value)
{
    for (std::vector<JsonElementType>::iterator it = m_data.begin(); it != m_data.end(); ++it)
    {
        if (key == it -> Key())
        {
            it -> SetValue(value);
            return;
        }
    }

    m_data.push_back(JsonElementType(key, value));
}

JsonType::~JsonType()
{

}
