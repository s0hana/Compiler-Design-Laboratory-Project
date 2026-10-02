#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <cctype>

using namespace std;

// Add explicit concatenation '.'
string addConcatenation(string regex)
{
    string result = "";

    for(int i = 0; i < regex.length(); i++)
    {
        result += regex[i];

        if(i + 1 < regex.length())
        {
            char a = regex[i];
            char b = regex[i + 1];

            bool first =
                isalnum(a) ||
                a == ')' ||
                a == '*' ||
                a == '+' ||
                a == '?';

            bool second =
                isalnum(b) ||
                b == '(';

            if(first && second)
            {
                result += '.';
            }
        }
    }

    return result;
}


// Operator precedence
int precedence(char op)
{
    if(op == '|')
    {
        return 1;
    }

    if(op == '.')
    {
        return 2;
    }

    return 0;
}


// Convert infix regex to postfix
string infixToPostfix(string regex)
{
    stack<char> st;

    string output = "";

    for(char c : regex)
    {
        // Operand
        if(isalnum(c))
        {
            output += c;
        }

        // Opening bracket
        else if(c == '(')
        {
            st.push(c);
        }

        // Closing bracket
        else if(c == ')')
        {
            while(!st.empty() && st.top() != '(')
            {
                output += st.top();
                st.pop();
            }

            if(!st.empty())
            {
                st.pop();
            }
        }

        // Unary operators
        else if(c == '*' || c == '+' || c == '?')
        {
            output += c;
        }

        // Binary operators
        else
        {
            while(!st.empty() &&
                  st.top() != '(' &&
                  precedence(st.top()) >= precedence(c))
            {
                output += st.top();
                st.pop();
            }

            st.push(c);
        }
    }

    // Empty remaining operators
    while(!st.empty())
    {
        output += st.top();
        st.pop();
    }

    return output;
}

int main()
{
    string regex;

    cout << "Enter regular expression: ";
    cin >> regex;


    // Step 1: Add concatenation
    string concatenated = addConcatenation(regex);

    cout << "\nConcatenated : "
         << concatenated << endl;


    // Step 2: Convert to postfix
    string postfix = infixToPostfix(concatenated);

    cout << "Postfix      : "
         << postfix << endl;

    return 0;
}