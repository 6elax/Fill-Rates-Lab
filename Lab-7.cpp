/* Alexis Luo 
Lab 7 - Fill Rates 
email: aluo324@student.fuhsd.org */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <algorithm> // for using transform 
#include <cctype> // for using toupper
using namespace std;

#define MAX 100

// function prototypes
int getData(ifstream& inFile, string crn[], string className[], string modality[], int current[], int max[], int wl[], double fillRate[]);
void output(ofstream& outFile, string crn[], string className[], int curEnroll[], int maxEnroll[], int numWL[], double fillRate[], int n);
double avgFillRate(double fillRate[], string modality[], string modType, int n);
int search(string crn[], int num, string target);
void fillRateMin(ofstream& outFile, string crn[], string course[], int curEnroll[], int maxEnroll[], int numWL[], double fillRate[], double minFillRate, int n);
void sortSelect(double arr[], int n, string crn[], string course[], int curEnroll[], int maxEnroll[], int wl[], string modality[]);
void signature();


int main(){

    //declare variables
    ifstream inFile;
    ofstream outFile;
    string crn[MAX];
    string course[MAX];
    string modality[MAX];
    int curEnroll[MAX];
    int maxEnroll[MAX];
    int numWL[MAX];
    double fillRate[MAX];
    int numClasses, loc;
    string modType, target;
    double minFillRate;


    //Open file
    inFile.open("Spring2024.txt");
    outFile.open("Lab-7-output.txt");
    if (inFile.fail() || outFile.fail()){
        cout << "No Such File" << endl;
        system("pause");
        exit(100);
    }

     //Input the entire contents of the product file
    numClasses = getData(inFile, crn, course, modality, curEnroll, maxEnroll, numWL, fillRate);
    //output data
    output(outFile, crn, course, curEnroll, maxEnroll, numWL, fillRate, numClasses);

    //average fill rate of a class modality
    cout << "Enter modality (ONLINE, HYBRID, BOTH): ";
    cin >> modType;
    transform(modType.begin(), modType.end(), modType.begin(), ::toupper);
    double avgFillR = avgFillRate(fillRate, modality, modType, numClasses);
    cout << "Average fill rate of " << modType << " classes: " << fixed << setprecision(1) << avgFillR << "%" << endl;

    //Looking for a particular course 
    cout << "Enter CRN id: ";
    cin >> target;
    /*cout << endl;
    cout << "-----" << endl;
    cout << endl;*/
    cout << "//// searching for course with CRN id " << target << " ////" << endl;
    loc = search(crn, numClasses, target);
    if (loc == -1){
        cout << "NO SUCH CLASS!" << endl; }
    else{
        cout << "    Class: " << course[loc];
        cout << "    Fill Rate: " << fixed << setprecision(1) << fillRate[loc] << "%" << endl; }

    //classes below a fill rate minimum
    cout << "Enter a numerical value that represents the lowest fill rate accpetable: ";
    cin >> minFillRate;
    fillRateMin(outFile, crn, course, curEnroll, maxEnroll, numWL, fillRate, minFillRate, numClasses);

    //Sort by the fill rate 
    sortSelect(fillRate, numClasses, crn, course, curEnroll, maxEnroll, numWL, modality);

    //Output each class's id, quantity on hand, re-order point, and price
    outFile << endl;
    outFile << "-----"<< endl;
    outFile<<endl;
    outFile << "Classes sorted by fill rate in ascending order: " << endl;
    output(outFile, crn, course, curEnroll, maxEnroll, numWL, fillRate, numClasses);

    //signature
    signature();

    //close files
    inFile.close();
    outFile.close();

    return 0;
}//main
/////////////////////////////////

int getData(ifstream& inFile, string crn[], string className[], string modality[], int current[], int max[], int wl[], double fillRate[]){
    /*Pre: inFile - reference to input file
    crn[] - array of course CRN id numbers
    className[] - name of course
    modality[] - type of class - online, hybrid, etc
    current[] - number of students currently enrolled in a course
    max[] - maximum number of students in a course
    wl[] - number of students waitlisted in a course
    fillRate[] - course fill rate
    Post: count - Number of courses offered by CIS department
    Purpose: read in inventory */

    int count = 0;
    string classtemp1;
    string classtemp2;
    while (!inFile.eof() && count < MAX){
        inFile >> crn[count];
        //getline(inFile, className[count]);
        inFile >> classtemp1 >> classtemp2; // avoid white spaces for class string
        className[count] = classtemp1+classtemp2;
        inFile >> modality[count] >> current[count] >> max[count] >> wl[count];

        if (modality[count] != "ONLINE"){
            modality[count] = "HYBRID";
        }

        if (max[count] > 0){
            fillRate[count] = 100.0 * (current[count] + wl[count]) / max[count];
            count++;
        }
    }

    return count;
}//getData
/////////////////////////////////  

void output(ofstream& outFile, string crn[], string className[], int curEnroll[], int maxEnroll[], int numWL[], double fillRate[], int n){
    /* pre: outFile - reference to output file
            crn[] - array of course CRN id numbers
            className[] - name of course
            curEnroll[] - number of students currently enrolled in a course
            maxEnroll[] - maximum number of students in a course
            numWL[] - number of students waitlisted in a course
            fillRate[] - fill rate of a class
            n - number of classes offered
       post: nothing
       purpose: to output the values out */
    
    outFile << fixed << setprecision(1);
    outFile << "CRN   Course        Current      Max         Number on  Fill " << endl;
    outFile << "                    Enrollment   Enrollment  Wait-list  Rate %" << endl;

    for (int i = 0; i < n; i++){
        //outFile << crn[i] << "." << className[i] << "." << endl;
        outFile << left << setw(6) << crn[i] << setw(15) << className[i] << right << setw(4) << curEnroll[i] << setw(12) << maxEnroll[i] << setw(12) << numWL[i] << setw(12) << fillRate[i] << endl;
    }
    return;

}//output
/////////////////////////////////

double avgFillRate(double fillRate[], string modality[], string modType, int n){
    /* Pre: fillRate[] - fill rates of all classes
            modality[] - array of modality of courses
            modType - modality of course
            n - number of courses offered
    Post: average fill rate
    Purpose: find the average fill rate for classes */

    double avg = 0;
    int numModClass=0;
    for(int i = 0; i < n; i++){
        if (modality[i] == modType || modType == "BOTH"){
            avg += fillRate[i];
            numModClass++;
        }
    }
    
    if(numModClass > 0)
        avg = avg / numModClass;
    return avg;
}//avgFillRate
/////////////////////////////////

int search(string crn[], int num, string target){
    /* Pre: crn[] - values being searched
            num - number of values to be searched through
            target - value being searched for
    Post: index of where it was found or -1 if not found
    Purpose: find location of value being searched for*/

    int loc = -1;
    for (int i = 0; i < num; i++){
        //cout << crn[i] << endl;
        if (crn[i] == target){
            loc = i;
        }
    }
    
    return loc;
}//search
/////////////////////////////////

void fillRateMin(ofstream& outFile, string crn[], string course[], int curEnroll[], int maxEnroll[], int numWL[], double fillRate[], double minFillRate, int n){
    /* pre: outFile - reference to output file
            crn[] - crn id of course
            course[] - array of class name
            curEnroll[] - amount of students currently enrolled in a course
            maxEnroll[] - max amount of students able to enroll in a course
            numWL[] - number of stuents waitlisted in a course
            fillRate[] - array of class fill rates
            minFillRate - minimum fill rate
            n - number of courses offered
       post: none
       purpose: find classes with fill rates below minimum fill rate
    */
    int numBelow=0;
    outFile << endl;
    outFile << "-----" << fixed << setprecision(1) << endl;
    outFile << endl;
    outFile << "Classes below fill rate of " << minFillRate << "%:" << endl;
    for(int i=0; i<n; i++){
        if (fillRate[i] < minFillRate){
            numBelow++;
            outFile << crn[i] << setw(12) << course[i] << setw(8) << curEnroll[i] << setw(12) << maxEnroll[i] << setw(12) << numWL[i] << setw(12) << fillRate[i] << endl;
        }
    }
    if (numBelow==0){
        outFile << "No class has a fill rate below " << minFillRate << "%" << endl;
    }

}//fillRateMin
/////////////////////////////////

void sortSelect(double arr[], int n, string crn[], string course[], int curEnroll[], int maxEnroll[], int wl[], string modality[]){
    /* pre: arr[] - swapping other array elements based on this arr[] (will be input as fillRate[] in this problem)
            n - number of courses offered
            crn - crn id of a course
            course[] - name of class
            curEnroll[] - currently enrolled students in a class
            maxEnroll[] - max students able to enroll in a class
            wl[] - number of students waitlisted in a class
        post: none
        purpose: to reorganize arrays based on arr[] (in this problem, fillRate[])
    */
    int current; int walker;
    int smallestIndex;
    string tempStr; int tempInt; double tempDbl;

    for (current = 0; current < n - 1; current++) {
        smallestIndex = current;
        for (walker = current; walker < n; walker++){
            if (arr[walker] < arr[smallestIndex])
                smallestIndex = walker;
        }//for walker

        //Swap to position smallest at what is the current position
        tempDbl = arr[current];
        arr[current] = arr[smallestIndex];
        arr[smallestIndex] = tempDbl;

        tempStr = crn[current];
        crn[current] = crn[smallestIndex];
        crn[smallestIndex] = tempStr;

        tempStr = course[current];
        course[current] = course[smallestIndex];
        course[smallestIndex] = tempStr;

        tempInt = curEnroll[current];
        curEnroll[current] = curEnroll[smallestIndex];
        curEnroll[smallestIndex] = tempInt;

        tempInt = maxEnroll[current];
        maxEnroll[current] = maxEnroll[smallestIndex];
        maxEnroll[smallestIndex] = tempInt;

        tempInt = wl[current];
        wl[current] = wl[smallestIndex];
        wl[smallestIndex] = tempInt;

        tempStr = modality[current];
        modality[current] = modality[smallestIndex];
        modality[smallestIndex] = tempStr;
    }//for current 
    return;
}

void signature(){
    cout << endl;
    cout << " * ---------------------------- * " << endl;
    cout << " *       Lab 7 - Fill Rates     * " << endl; 
    cout << " *   aluo324@student.fuhsd.org  * " << endl; 
    cout << " *   Programmed by Alexis Luo   * " << endl; 
    cout << " * ---------------------------- * " << endl;
    cout << endl;
}


