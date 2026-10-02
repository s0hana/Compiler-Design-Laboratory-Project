#include<iostream>
#include<fstream>

using namespace std;

int main()
{
    ifstream input("input.txt");

    if(!input)
    {
        cout << "Cannot open file.";
        return 1;
    }

    ofstream output("output_comment_remove.txt");

    char c;
    bool lastWasSpace = false;

    while(input.get(c))
    {
        if(c == '/' && input.peek() == '/')
        {
            input.get();

            while(input.get(c) && c != '\n')
            {
                // do nothing
            }

            lastWasSpace = true;
        }

        else if(c == '/' && input.peek() == '*')
        {
            input.get();

            while(input.get(c))
            {
                if(c == '*' && input.peek() == '/')
                {
                    input.get();
                    break;
                }
            }

            lastWasSpace = true;
        }

        else if(c == ' ' || c == '\n' || c == '\t' || c == '\r')
        {
            if(!lastWasSpace)
            {
                output << ' ';
                lastWasSpace = true;
            }
        }

        else
        {
            output << c;
            lastWasSpace = false;
        }
    }

    input.close();
    output.close();

    cout << "Code cleaned" << endl;
    cout << "Output is saved to output_comment_remove.txt";

    return 0;
}