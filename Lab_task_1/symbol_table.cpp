#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <iomanip>
#include <sstream>

using namespace std;

struct Symbol
{
    string name, type, kind;
    int line, no;
};

int main()
{
    ifstream input("input.txt");

    if (!input)
    {
        cout << "Cannot open input file!" << endl;
        return 1;
    }

    vector<Symbol> symboltable;

    string line;
    int lineNo = 1;
    int no = 1;

    regex typePattern(R"(\b(int|float|double|void|string|char)\s+)");
    regex identifierPattern(R"(^\s*([A-Za-z_][A-Za-z0-9_]*)\s*(.*)$)");

    while (getline(input, line))
    {
        smatch typeMatch;

        if (regex_search(line, typeMatch, typePattern))
        {
            string type = typeMatch[1].str();
            string declaration = line.substr(typeMatch.position(0) + typeMatch.length(0));
            size_t openParen = declaration.find('(');
            if (openParen != string::npos)
            {
                string functionPart = declaration.substr(0, openParen);
                smatch funcMatch;
                if (regex_match(functionPart, funcMatch, identifierPattern))
                {
                    Symbol s;
                    s.no = no++;
                    s.name = funcMatch[1].str();
                    s.type = type;
                    s.kind = "Function";
                    s.line = lineNo;
                    symboltable.push_back(s);
                }
            }
            else
            {
                size_t semicolon = declaration.find(';');
                if (semicolon != string::npos)
                {
                    declaration = declaration.substr(0, semicolon);
                }
                stringstream ss(declaration);
                string variable;
                while (getline(ss, variable, ','))
                {
                    size_t start = variable.find_first_not_of(" \t");
                    if (start == string::npos)
                        continue;
                    variable = variable.substr(start);
                    smatch varMatch;
                    if (regex_search(variable, varMatch, regex(R"(^([A-Za-z_][A-Za-z0-9_]*))")))
                    {
                        string name = varMatch[1].str();
                        Symbol s;
                        s.no = no++;
                        s.name = name;
                        s.type = type;
                        s.kind = "Variable";
                        s.line = lineNo;
                        symboltable.push_back(s);
                    }
                }
            }
        }

        lineNo++;
    }

    input.close();

    cout << "---------------------------------------------------------------------------------"<< endl;
    cout << left << setw(10) << "No." << setw(15) << "Name" << setw(12) << "Type" << setw(12) << "Kind" << "Line"<< endl;
    cout << "---------------------------------------------------------------------------------"<< endl;

    for (const Symbol& s : symboltable)
    {
        cout << left << setw(10) << s.no << setw(15) << s.name << setw(12) << s.type << setw(12) << s.kind << s.line << endl;
    }
    cout << "---------------------------------------------------------------------------------"<< endl;

    return 0;
}