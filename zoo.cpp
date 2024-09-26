// Project Identifier: 3E33912F8BAA7542FC4A1585D2DB6FE0312725B9
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <getopt.h>
#include <sstream>      
#include <algorithm>
#include <cmath>       
#include <deque>
#include <cassert>

using namespace std;


    enum class Mode {
        kNone = 0,  
        kHelp,
        kMode,
    };  // Mode{}

    struct Options {
        Mode mode = Mode::kNone;
    };  // Options{}


    void printHelp(char *argv[]) {
        cout << "Usage: " << argv[0] << " Here are your input options!\n";
        cout << "Options: \n";
        cout << "  --help, -h \n";
        cout << "  --mode, -m \n";
    }

    bool MSTcheck = false;
    bool FASTTSPcheck = false;
    bool OPTTSPcheck = false;
    // Finish this function, look for individual TODO comments internally.
    // Process the command line; there is no return value, but the Options
    // struct is passed by reference and is modified by this function to send
    // information back to the calling function.
    void getMode(int argc, char * argv[], Options &options) {
        // These are used with getopt_long()
        opterr = false; // Let us handle all error output for command line options
        int choice;
        int index = 0;
        option long_options[] = {
        {"help", no_argument,  nullptr, 'h'},
        {"mode", required_argument, nullptr, 'm'},
        { nullptr, 0, nullptr, '\0' },
        }; // long_options[]

        // Fill in the double quotes, to match the mode and help options.

        while ((choice = getopt_long(argc, argv, "hm:", long_options, &index)) != -1) {
            switch (choice) {
                case 'h':
                    printHelp(argv);
                    exit(0);
                    break;

                case 'm': {  // Need a block here to declare a variable inside a case
                    options.mode = Mode::kMode;
                    string arg{optarg};
                    if (arg != "MST" && arg != "FASTTSP" && arg != "OPTTSP") {
                        // The first line of error output has to be a 'fixed' message
                        // for the autograder to show it to you.
                        cerr << "Error: invalid mode" << "\n";
                        // The second line can provide more information, but you
                        // won't see it on the AG.
                        exit(1);
                    }  // if ..arg valid

                    if (arg == "MST") {
                        MSTcheck = true;
                        //cout << "MST" << endl;
                    
                    } else if (arg == "FASTTSP") {
                        FASTTSPcheck = true;
                    } else {
                        OPTTSPcheck = true;
                    }
                    break;
                } 

                default:
                    cerr << "Error: invalid option" << "\n";
                    exit(1);
                }  // switch ..choice
        }  // while

        // if (options.mode == Mode::kNone) {

        // }  // if ..mode

    }  // getMode()


struct Node {
    double xCor = 0;
    double yCor = 0;
    char label = 'a';
    int index = 0;
    double dist = std::numeric_limits<double>::infinity();
    int prev = -1;
    bool visited = false;
    
};

class Keeping {
private:
    int findClosestNode() {
        double minDist = std::numeric_limits<double>::infinity();
        int closestIndex = -1;

        for (size_t i = 0; i < masterList.size(); i++) {
            if (!masterList[i].visited && masterList[i].dist < minDist) {
                minDist = masterList[i].dist;
                closestIndex = (int)i;
            }
        }

        return closestIndex;
    }

    int CfindClosestNode(size_t permLength) {
        double minDist = std::numeric_limits<double>::infinity();
        int closestIndex = -1;

        for (size_t i = permLength; i < currPath.size(); i++) {
            if (!masterList[currPath[i]].visited && (masterList[currPath[i]].dist < minDist)) {
                minDist = masterList[currPath[i]].dist;
                closestIndex = (int)i;
            }
        }

        return closestIndex;
    }




public:
    int size;
    vector<Node> masterList;
    vector<int> tPath;
    vector<int> bestPath;
    vector<int> currPath;
    double bestCost;
    double currCost;

    void readfile() {
        bool s = false;
        bool w = false;
        bool b = false;
        cin >> size;
        masterList.reserve(size);
        for(int i = 0; i < size; i++) {
            Node temp;
            temp.index = i;
            cin >> temp.xCor >> temp.yCor;
            if ((temp.xCor == 0 && temp.yCor <= 0) || (temp.xCor <= 0 && temp.yCor == 0)) {
                temp.label = 'b';
                b = true;
                //cout << "border check" << endl;
            } else if ((temp.xCor < 0 && temp.yCor < 0)) {
                temp.label = 'w';
                w = true;
                //cout << "wild check" << endl;
            } else {
                temp.label = 's';
                s = true;
                //cout << "safe check" << endl;
            }
            masterList.push_back(temp);
            //cout << temp.xCor << " " << temp.yCor << endl;
        }
        if(MSTcheck) {
            if(s == true && w == true && b == false){
                cerr << "Cannot construct MST" << endl;
                exit(1);
            }
        }
        //cout << masterList.size() << endl;
    }
    //Part A: MST
    //(0,0) (6,8)
    double euADist (Node& a, Node& b) {
        if((a.label == 's' && b.label == 'w') || (b.label == 's' && a.label == 'w')) {
            return std::numeric_limits<double>::infinity();
        } else {
            return sqrt(((b.xCor - a.xCor) * (b.xCor - a.xCor)) + ((b.yCor - a.yCor) * (b.yCor - a.yCor)));
        }
    }
    double fullTotal = 0;
    void MST() {
        
        int remainingNodes = (int)masterList.size();
        masterList[0].dist = 0; 
    

        while (remainingNodes > 0) {
            int closestIndex = findClosestNode();
            masterList[closestIndex].visited = true;
            remainingNodes--;
            fullTotal += masterList[closestIndex].dist;


            //cout << "Add Node " << closestIndex << " with distance " << masterList[closestIndex].dist << " to MST\n";

            for (size_t i = 0; i < masterList.size(); i++) {
                if (!masterList[i].visited && (euADist(masterList[closestIndex], masterList[i]) != std::numeric_limits<double>::infinity())) {
                    double distance = euADist(masterList[closestIndex], masterList[i]);
                    if (distance < masterList[i].dist) {
                        masterList[i].dist = distance;
                        masterList[i].prev = closestIndex;
                    }
                }
            }
        }

    }

    void MSToutput() {
        cout << fullTotal << "\n";
        for(int i = 1; i < (int)masterList.size(); i++){
            cout << min(masterList[i].prev, i ) <<" "<< max(masterList[i].prev, i ) << "\n";

        }
    }

    double euBDist (Node& a, Node& b) {
        return sqrt(((b.xCor - a.xCor) * (b.xCor - a.xCor)) + ((b.yCor - a.yCor) * (b.yCor - a.yCor)));    
    }

    //start at first
    // insert index 1, then look inbetween onwards
    double TSPtotal = 0;
    void FASTTSP() {
        
        
        //double min = std::numeric_limits<double>::infinity();
        tPath.push_back(0);
        tPath.push_back(1);
        tPath.push_back(0);
        TSPtotal += 2 * euBDist(masterList[0], masterList[1]);
        // 0 1 0
        for(int k = 2; k < (int)masterList.size(); k++) { // has 5 in spec exp
            int indexbefore = 0;
            double min = std::numeric_limits<double>::infinity();
            for(int i = 0; i < (int)tPath.size() - 1; i++) { // has 3 at the start
                //Cik + Cjk - Cij changes cost
                double Cik = euBDist(masterList[tPath[i]], masterList[k]);
                double Cjk = euBDist(masterList[tPath[i + 1]], masterList[k]);
                double Cij = euBDist(masterList[tPath[i]], masterList[tPath[i + 1]]);
                double distances = Cik + Cjk - Cij;
                if(distances < min){
                    //cout << tPath[i] << " "<< tPath[i+1] << " "<< k << " "<< distances << endl;
                    min = distances;
                    indexbefore = i;
                }
            }
            //cout << indexbefore << endl;
            // if(indexbefore == -1) {
            //     cout << "something wrong got index -1" << endl;
            // }
            tPath.insert(tPath.begin() + indexbefore + 1, k);
            TSPtotal += min;
        }
        //ans 013240 or 042310
        //02310
        //042310
        

    }

    void FASTTSPoutput () {
        cout << TSPtotal << "\n";
        for(int i = 0; i < (int)tPath.size() - 2; i++){
            cout << tPath[i] << " ";
        }
        cout << tPath[size - 1] << "\n";
    }


    void genPerms(size_t permLength) {
        //cout << "start of genperms " << permLength << endl;
            if (permLength == currPath.size()) {
            // Do something with the path
            //    lets get the total path length see 
            //    if its better than the best so far 
            //    and update best so far
                currCost += euBDist(masterList[currPath[permLength - 1]], masterList[currPath[0]]);
                //cout << "currcost right before " << currCost << endl;
                if(currCost < bestCost) {
                    bestPath = currPath;
                    bestCost = currCost;

                }
                currCost -= euBDist(masterList[currPath[permLength - 1]], masterList[currPath[0]]);
                return;
            }  // if ..complete path

            if (!promising(permLength)) {
                return;
            }  // if ..not promising

            for (size_t i = permLength; i < currPath.size(); ++i) {
                swap(currPath[permLength], currPath[i]);
                currCost += euBDist(masterList[currPath[permLength - 1]], masterList[currPath[permLength]]);
                genPerms(permLength + 1);
                //cout << "failed perm " << permLength << endl;
                currCost -= euBDist(masterList[currPath[permLength - 1]], masterList[currPath[permLength]]);
                swap(currPath[permLength], currPath[i]);
            }  // for ..unpermuted elements
        }  // genPerms()


        void ResetMST(size_t permLength){
            for(int i = (int)permLength; i < (int)(currPath.size()); i++){
                masterList[currPath[i]].visited = false;
                masterList[currPath[i]].dist = std::numeric_limits<double>::infinity();
            }
        }

        double OPTMST(size_t permLength) {
            ResetMST(permLength);

            int unvistiedNodes = (int)(currPath.size() - permLength);
            masterList[currPath[permLength]].dist = 0; 
            //masterList[currPath[permLength]].visited = true; 
            double mTotal = 0;
            while (unvistiedNodes > 0) {
                int closestIndex = CfindClosestNode(permLength);
                masterList[currPath[closestIndex]].visited = true;
                unvistiedNodes--;
                cout << "closestindex " << closestIndex << endl;
                mTotal += masterList[currPath[closestIndex]].dist;


                //cout << "Add Node " << closestIndex << " with distance " << masterList[closestIndex].dist << " to MST\n";

                for (size_t i = permLength; i < (currPath.size() - permLength); i++) {
                    if (!masterList[currPath[i]].visited) {
                        double distance = euBDist(masterList[currPath[closestIndex]], masterList[currPath[i]]);
                        
                        if (distance < masterList[currPath[i]].dist) {
                            masterList[currPath[i]].dist = distance;
                            //masterList[currPath[i]].prev = currPath[closestIndex];
                            cout << "bestmst " << currPath[i] << " " << currPath[closestIndex] << endl;
                        }
                    }
                    
                }
            }

            return mTotal;
        }


        bool promising (size_t permLength) {
            if(((int)currPath.size() - permLength) < 5){
                return true;
            }
            double minarm1 = std::numeric_limits<double>::infinity();
            double minarm2 = std::numeric_limits<double>::infinity();
            for(int i = (int)permLength; i < (int)currPath.size(); i++){
                //double distance = distanceMatrix[unused[j]][path.back()];
                double distarm1 = euBDist(masterList[currPath[0]], masterList[currPath[i]]);
                double distarm2 = euBDist(masterList[currPath[i]], masterList[currPath[permLength - 1]]);
                    // update current closest dist
                if(distarm1 < minarm1){
                    minarm1 = distarm1;
                }

                if(distarm2 < minarm2){
                    minarm2 = distarm2;
                }    
            }

            double MSTamount = OPTMST(permLength);

            cout << "currCost " << currCost << endl;
            cout << "minarm1 " << minarm1 << endl;
            cout << "minarm2 " << minarm2 << endl;
            cout << "MSTamount " << MSTamount << endl;

            if ((currCost + minarm2 + minarm1 + MSTamount) < bestCost) {
                return true;
            } 

            return false;
        

        }

    
    // void distMatrixMaker() {
    //     for(int i = 0; i < (int)distMatrix.size(); i++){
    //         for(int j = 0; j < (int)distMatrix[i].size(); j++){
    //             distMatrix[i][j] = euBDist(masterList[i], masterList[j]);  
    //         }    
    //     }
    // }

    
    void OPTTSP () {
        
       
    
        
        //distMatrixMaker();
        //vector<double> currCost ((int)masterList.size());

        FASTTSP();
        bestPath = tPath;
        currPath = tPath;
        bestCost = TSPtotal;
        currCost = 0;
        cout << bestCost << endl;

        bestPath.pop_back();
        currPath.pop_back();
     
        size_t permlength = 1;
        genPerms(permlength);

        cout << bestCost << "\n";
        for(int i = 0; i < (int)bestPath.size() - 1; i++){
            cout << bestPath[i] << " ";
        }
        cout << bestPath.back() << "\n";


    }









    
    
    
};




    int main(int argc, char** argv) {
        ios_base::sync_with_stdio(false);
        cout << std::setprecision(2);
        cout << std::fixed;
        //cout << "test1" << "\n";
        Options options;
        getMode(argc, argv, options);
        Keeping test;
        test.readfile();
        if(MSTcheck == true) {
            //cout << "check" << endl;
            test.MST();
            test.MSToutput();
        }
        if(FASTTSPcheck == true) {
            test.FASTTSP();
            test.FASTTSPoutput();
        }
        if(OPTTSPcheck == true) {
            test.OPTTSP();
        }
        
        
        //cout << "Helluo, World!" << endl;
        
        //cout << "end" << "\n";

        return 0;


    }
