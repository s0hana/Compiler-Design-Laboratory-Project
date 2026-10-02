#include<iostream>
#include<fstream>
#include<regex>
#include<unordered_set>

using namespace std;

int main()
{
    ifstream input("input.txt");
    if(!input)
    {
        cout << "Cannot open file!" ;
        return 1;
    }
    string code(istreambuf_iterator<char>(input), {});
    input.close();
    code = regex_replace(code, regex(R"(//[^\n]*|/\*[\s\S]*?\*/)"), "");
    unordered_set<string> keyword = {
         "alignas","alignof","and","and_eq","asm","auto",
        "bitand","bitor","bool","break","case","catch",
        "char","char8_t","char16_t","char32_t","class",
        "compl","concept","const","consteval","constexpr",
        "constinit","const_cast","continue","co_await",
        "co_return","co_yield","decltype","default","delete",
        "do","double","dynamic_cast","else","enum","explicit",
        "export","extern","false","float","for","friend",
        "goto","if","inline","int","long","mutable",
        "namespace","new","noexcept","not","not_eq",
        "nullptr","operator","or","or_eq","private",
        "protected","public","reinterpret_cast","requires",
        "return","short","signed","sizeof","static",
        "static_assert","static_cast","struct","switch",
        "template","this","thread_local","throw","true",
        "try","typedef","typeid","typename","union",
        "unsigned","using","virtual","void","volatile",
        "wchar_t","while","xor","xor_eq"
    };

    regex token(R"(\n|[0-9]+[A-Za-z_][A-Za-z0-9_]*|[A-Za-z_][A-Za-z0-9_]*|[0-9]+|\+|-|\*|/|%|\+\+|--|==|!=|<=|>=|\|\||&&|\||&|~|\^|\?|:|;|,|\.|\(|\)|\{|\}|\[|\]|=|!|<|>)");
    smatch match;
    int line = 1;
    while (regex_search(code, match, token))
    {
        string t = match.str();
        if(t=="\n")
        {
            cout << "Line "<<line<<": Newline"<<endl;
            line++;
        }
        else if(regex_match(t, regex(R"([0-9]+[A-Za-z_][A-Za-z0-9_]*)")))
        {
            cout << "Error -- Line "<<line<<": Invalid identifier."<<endl;

        }
        else if(regex_match(t, regex(R"([A-Za-z_][A-Za-z0-9_]*)")))
        {
            cout << "Line "<<line<<": Identifier -> "<< t <<endl;

        }
        else if(regex_match(t, regex(R"([0-9]+)")))
        {
            cout << "Line "<<line<<": Number -> "<< t <<endl;

        }
        else if(regex_match(t, regex(R"(\+|-|\*|/|%|\+\+|--|==|!=|<=|>=|\|\||&&|\||&|~|\^|=|!|<|>)")))
        {
            cout << "Line "<<line<<": Operator -> "<< t <<endl;

        }
        else if(keyword.count(t))
        {
            cout << "Line "<<line<<": Keyword -> "<< t <<endl;
        }
        else
        {
            cout << "Line "<<line<<": Symbol -> "<< t <<endl;
        }
        code = match.suffix();
    }
    return 0;
}