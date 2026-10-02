#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <cctype>

using namespace std;


// ==================== STRUCTURES ====================

struct Transition
{
    int from;
    char symbol;
    int to;
};

struct NFA
{
    int start;
    int end;
};


// ==================== GLOBAL VARIABLES ====================

int stateCount = 0;
vector<Transition> transitions;


// ==================== REGEX FUNCTIONS ====================

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


// ==================== NFA FUNCTIONS ====================

// Create new state
int newState()
{
    return stateCount++;
}


// Add transition
void addTransition(int from, char symbol, int to)
{
    transitions.push_back({from, symbol, to});
}


// Create NFA for a single symbol
NFA createSymbolNFA(char symbol)
{
    int start = newState();
    int end = newState();

    addTransition(start, symbol, end);

    return {start, end};
}


// Concatenation
NFA concatenate(NFA nfa1, NFA nfa2)
{
    int oldStart = nfa2.start;
    int newStart = nfa1.end;

    // Redirect transitions of nfa2.start
    for(auto& t : transitions)
    {
        if(t.from == oldStart)
        {
            t.from = newStart;
        }

        if(t.to == oldStart)
        {
            t.to = newStart;
        }
    }

    return {nfa1.start, nfa2.end};
}


// Union
NFA unionNFA(NFA nfa1, NFA nfa2)
{
    int start = newState();
    int end = newState();

    // New start to both NFAs
    addTransition(start, '#', nfa1.start);
    addTransition(start, '#', nfa2.start);

    // Both NFAs to new final state
    addTransition(nfa1.end, '#', end);
    addTransition(nfa2.end, '#', end);

    return {start, end};
}


// Kleene Star
// a* = zero or more a
NFA kleeneStar(NFA nfa)
{
    int start = newState();
    int end = newState();

    // Start can enter NFA or skip it
    addTransition(start, '#', nfa.start);
    addTransition(start, '#', end);

    // Repeat or finish
    addTransition(nfa.end, '#', nfa.start);
    addTransition(nfa.end, '#', end);

    return {start, end};
}


// Plus
// a+ = one or more a
// Equivalent to aa*
NFA plusNFA(NFA nfa)
{
    int start = newState();
    int end = newState();

    // Must enter NFA at least once
    addTransition(start, '#', nfa.start);

    // After one occurrence:
//  repeat OR finish
    addTransition(nfa.end, '#', nfa.start);
    addTransition(nfa.end, '#', end);

    return {start, end};
}


// Question Mark
// a? = zero or one a
NFA questionNFA(NFA nfa)
{
    int start = newState();
    int end = newState();

    // Either enter NFA or skip it
    addTransition(start, '#', nfa.start);
    addTransition(start, '#', end);

    // After one occurrence, finish
    addTransition(nfa.end, '#', end);

    return {start, end};
}


// ==================== BUILD NFA ====================

NFA buildNFA(string postfix)
{
    stack<NFA> st;

    for(char c : postfix)
    {
        // Symbol
        if(isalnum(c))
        {
            st.push(createSymbolNFA(c));
        }

        // Concatenation
        else if(c == '.')
        {
            NFA nfa2 = st.top();
            st.pop();

            NFA nfa1 = st.top();
            st.pop();

            st.push(concatenate(nfa1, nfa2));
        }

        // Union
        else if(c == '|')
        {
            NFA nfa2 = st.top();
            st.pop();

            NFA nfa1 = st.top();
            st.pop();

            st.push(unionNFA(nfa1, nfa2));
        }

        // Kleene Star
        else if(c == '*')
        {
            NFA nfa = st.top();
            st.pop();

            st.push(kleeneStar(nfa));
        }

        // Plus
        else if(c == '+')
        {
            NFA nfa = st.top();
            st.pop();

            st.push(plusNFA(nfa));
        }

        // Question Mark
        else if(c == '?')
        {
            NFA nfa = st.top();
            st.pop();

            st.push(questionNFA(nfa));
        }
    }

    return st.top();
}


// ==================== NORMALIZE STATES ====================

void normalizeNFA(NFA& finalNFA)
{
    set<int> activeStates;

    activeStates.insert(finalNFA.start);
    activeStates.insert(finalNFA.end);

    for(const auto& t : transitions)
    {
        activeStates.insert(t.from);
        activeStates.insert(t.to);
    }

    map<int, int> oldToNew;

    int newId = 0;

    for(int s : activeStates)
    {
        oldToNew[s] = newId++;
    }

    for(auto& t : transitions)
    {
        t.from = oldToNew[t.from];
        t.to = oldToNew[t.to];
    }

    finalNFA.start = oldToNew[finalNFA.start];
    finalNFA.end = oldToNew[finalNFA.end];

    stateCount = newId;
}


// ==================== PRINT TRANSITION TABLE ====================

void printTransitionTable(NFA finalNFA)
{
    set<char> symbols;

    // Find all input symbols
    for(auto& t : transitions)
    {
        if(t.symbol != '#')
        {
            symbols.insert(t.symbol);
        }
    }

    cout << "\n========== NFA TRANSITION TABLE ==========\n\n";


    // Table heading
    cout << "State\t";

    for(char symbol : symbols)
    {
        cout << symbol << "\t";
    }

    cout << "Epsilon\n";


    // Print each state
    for(int state = 0; state < stateCount; state++)
    {
        // State name
        if(state == finalNFA.start)
        {
            cout << "->q" << state << "\t";
        }
        else if(state == finalNFA.end)
        {
            cout << "*q" << state << "\t";
        }
        else
        {
            cout << "q" << state << "\t";
        }


        // Print symbol transitions
        for(char symbol : symbols)
        {
            vector<int> nextStates;

            for(auto t : transitions)
            {
                if(t.from == state &&
                   t.symbol == symbol)
                {
                    nextStates.push_back(t.to);
                }
            }


            if(nextStates.empty())
            {
                cout << "-\t";
            }
            else
            {
                cout << "{";

                for(int i = 0; i < nextStates.size(); i++)
                {
                    cout << "q" << nextStates[i];

                    if(i != nextStates.size() - 1)
                    {
                        cout << ",";
                    }
                }

                cout << "}\t";
            }
        }


        // Print epsilon transitions
        vector<int> epsilonStates;

        for(auto t : transitions)
        {
            if(t.from == state &&
               t.symbol == '#')
            {
                epsilonStates.push_back(t.to);
            }
        }


        if(epsilonStates.empty())
        {
            cout << "-";
        }
        else
        {
            cout << "{";

            for(int i = 0; i < epsilonStates.size(); i++)
            {
                cout << "q" << epsilonStates[i];

                if(i != epsilonStates.size() - 1)
                {
                    cout << ",";
                }
            }

            cout << "}";
        }

        cout << endl;
    }


    cout << "\n-> = Start State\n";
    cout << "*  = Final State\n";
}


// ==================== MAIN ====================

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


    // Step 3: Build NFA
    NFA finalNFA = buildNFA(postfix);


    // Step 4: Normalize state numbers
    normalizeNFA(finalNFA);


    // Step 5: Print transition table
    printTransitionTable(finalNFA);


    // Step 6: Print NFA information
    cout << "\nStart State  : q"
         << finalNFA.start << endl;

    cout << "Final State  : q"
         << finalNFA.end << endl;

    cout << "Total States : "
         << stateCount << endl;


    return 0;
}