
class IterativeDeepener
{
    
public:
    IterativeDeepener(int* position, int* positionalInformation, int* pieceValues);
    ~IterativeDeepener(void);
    void iterativeDeepening(double time_limit);
    void recordPrincipalVariation(int* PVTable, int length);
    void displayPrincipalVariation(int length);
    void displayPosition(void);
    void reset(int* position, int* positionalInformation, int* pieceValues, int maximumDepth);
    int getPrincipalFromSquare(void);
    int getPrincipalToSquare(void);
    void sortRootNodes(void);
    void setDisplayAnalysis(bool);
    void setMaxTime(double);
    
private:
    int* position;
    int* positionalInformation;
    int* pieceValues;
    int principalVariation[512];
    int rootNodes[1024];
    int maximumDepth;
    int baseEvaluation;
    int lastFromSquare;
    int lastToSquare;
    int principalFromSquare;
    int principalToSquare;
    bool displayAnalysis;
    double max_time;
};
