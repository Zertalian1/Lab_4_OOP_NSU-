#include "CSVParser.h"
//#include "Tuple.h"
using namespace std;

int main()
{
    setlocale(LC_ALL, "Russian");
    ifstream a("input.txt");

    CSVParser<int, string> parser(a, 0/*skip first lines count*/);

    for (tuple<int, string> el : parser)
    // tuple<string, double, int> el("asd", 1.3, 5);
    //tuple<> el;
    //tuple<string> el("asd");
        cout << el << endl;

    return 0;
}