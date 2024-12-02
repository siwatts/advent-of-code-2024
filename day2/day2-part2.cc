#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

bool isLevelSafe(vector<int> level);

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 2\n--\n";
    // For testing
    int debuglimit = 1;
    int debug = 0;
    bool debugapply = false;
    if (debugapply) { cout << "DEBUG MODE : ON\nLINE LIMIT : " << debuglimit << "\n--" << endl; }

    // Input file
    ifstream input("input");

    // Variables for output
    int sum = 0;

    // Read file and get numbers
    string line;
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        //cout << line << endl;

        vector<int> num;

        // TODO: Make general function number parser for this year
        // Read numbers
        size_t i = 0, j = 0;
        while (i < line.length())
        {
            // i = start of no.
            // j = end of no. (i.e. pos. of char. after last digit, (j-i)=len)
            while (j < line.length() && isdigit(line[j])) {
                j++;
            }
            // Found digit from pos. i to j
            num.push_back(stoi(line.substr(i,j-i)));
            //cout << "Parsed number: '" << line.substr(i,j-i) << "'\n";
            // Find start of next number first
            i = j;
            while (i < line.length() && !isdigit(line[i])) {
                i++;
            }
            // Either found start of next no. or hit the end of the string
            j = i;
        }
        //cout << "Found " << num.size() << " num" << endl;

        // Program logic
        // Find out if all asc or desc
        bool safe;
        bool isAsc;
        bool isDesc;

        // Make a sorted copy of the vector so we don't modify the original
        vector<int> numSort = num;
        sort(numSort.begin(), numSort.end());
        int removedItems;
        vector<vector<int>> listValidSubArrays;

        // If they match, we are ascending
        isAsc = false;
        if (num == numSort) {
            removedItems = 0;
            isAsc = true;
        }
        else {
            // Part 2: Tolerate 1 incorrect element, try removing each in turn
            // to see if we still end up with valid sub-arrays
            // The logic around which of the possible elements should be removed is difficult
            // to know in advance when combined with the 2nd part of part 2 involving
            // the differences between elements. So just store them all to try later
            for (size_t i = 0; i < num.size(); i++) {
                vector<int> subList = num;
                vector<int> subSorted = numSort;
                auto sortedIndex = find(subSorted.begin(), subSorted.end(), num[i]);
                if (sortedIndex == subSorted.end())
                    cout << "ERROR: Didn't find the element we're about to remove in the sorted copy array, something went badly wrong!\n";
                subList.erase(subList.begin() + i);
                subSorted.erase(sortedIndex);
                // Check validity
                if (subList == subSorted) {
                    //cout << "Found a valid sublist by removing element " << i << " of line " << debug << endl;
                    listValidSubArrays.push_back(subList);
                    removedItems = 1;
                    isAsc = true;
                }
            }
        }

        // If not Asc, try Desc
        isDesc = false;
        if (!isAsc) {
            // May be descending instead, check by reversing sorted array
            reverse(numSort.begin(), numSort.end());
            if (num == numSort) {
                removedItems = 0;
                isDesc = true;
            }
            else {
                // Try removing items, as above
                for (size_t i = 0; i < num.size(); i++) {
                    vector<int> subList = num;
                    vector<int> subSorted = numSort;
                    auto sortedIndex = find(subSorted.begin(), subSorted.end(), num[i]);
                    if (sortedIndex == subSorted.end())
                        cout << "ERROR: Didn't find the element we're about to remove in the sorted copy array, something went badly wrong!\n";
                    subList.erase(subList.begin() + i);
                    subSorted.erase(sortedIndex);
                    // Check validity
                    if (subList == subSorted) {
                        //cout << "Found a valid sublist by removing element " << i << " of line " << debug << endl;
                        listValidSubArrays.push_back(subList);
                        removedItems = 1;
                        isDesc = true;
                    }
                }
            }
        }

        //cout << "Vector orig:\n";
        //for (int i = 0; i < num.size(); i++) {
        //    cout << num[i] << endl;
        //}

        if (isAsc || isDesc) {
            if (removedItems == 0) {
                // We didn't remove any items, so we can check our original vector
                safe = isLevelSafe(num);
                if (!safe) {
                    // If we failed here, we still one last chance to remove an element
                    // So try all those too
                    for (size_t i = 0; i < num.size(); i++) {
                        vector<int> subList = num;
                        subList.erase(subList.begin() + i);
                        listValidSubArrays.push_back(subList);
                        removedItems = 1;
                    }
                }
            }
            // Might have some sub arrays to test, either from asc/desc stage or above
            if (removedItems == 1 && listValidSubArrays.size() > 0) {
                // We have some valid sub arrays to check
                safe = false;
                for (size_t i = 0; i < listValidSubArrays.size(); i++) {
                    if (isLevelSafe(listValidSubArrays[i]))
                        safe = true;
                }
            }
        }

        // Keep running sum
        if (safe)
            sum++;
    }

    // Output
    cout << "--\n";
    cout << "Sum of safe reports = " << sum << endl;

    // Finished with input file
    input.close();

    cout << "--\nEnd.\n";
    return 0;
}

bool isLevelSafe(vector<int> level)
{
    // Check that difference between successive values is no more than 3, and not 0
    // This function assumes that the array is already valid and sorted (ie. ascending
    // or descending order)
    bool safe = true;
    for (size_t i = 1; safe && i < level.size(); i++) {
        if (abs(level[i] - level[i-1]) == 0 || abs(level[i] - level[i-1]) > 3) {
            safe = false;
        }
    }
    return safe;
}

