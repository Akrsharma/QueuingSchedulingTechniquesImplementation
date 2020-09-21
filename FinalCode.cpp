#include<iostream>
#include<cstring>
#include<vector>
#include<algorithm>
#include<queue>
#include<map>
#include<fstream>
#include<cmath>
#include<random>
#include<iomanip>
using namespace std;

double countMoreThanKProb;
vector< pair<double, int> > tmp; //arrivalTime, outputPort
default_random_engine generator;
vector< vector<int> > islipReqInfo,tmpislipReqInfo;  //inputPort, corresponding outputPorts Requests

map<pair<int,int>, int> LinkUtilPerLink;
//Uniform distribution module
int uniform_distribution(int rangeLow, int rangeHigh)
{
    clock_t t; 
    t = clock();
    double d = ((double)t)/CLOCKS_PER_SEC;
    d = d*10000000;
    srand(d);
    double myRand = rand()/(1.0 + RAND_MAX); 
    int range = rangeHigh - rangeLow + 1;
    int myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;
}
//Traffic Generation Module
void trafficGeneration(int t, int tp, double pgp)
{
    tmp.clear();
    tmp.resize(tp);
    fill(tmp.begin(),tmp.end(),make_pair(-1,-1));
    double factor = 1/pgp;
    double totalNum = tp*(factor);
    
    uniform_real_distribution<double> distribution(t+0.001, t+0.01);
    uniform_int_distribution<int> distribution1(0,tp-1);
    double arrivalTime;
    int outputPort;
    int x;
    for(int i=0; i<tp; i++)
    {
         x = rand()%(int)(floor(totalNum));
        if(x<tp) 
        {
            arrivalTime = distribution(generator);
            outputPort = uniform_distribution(0,tp-1);
            tmp[i].first = arrivalTime;
            tmp[i].second = outputPort;
        }
        
    }
}


//Packet Scheduling Module
void packetScheduling(int t, int bufferInSize, int bufferOutSize, string queueType, vector< queue<int> > &inputQueues,  vector< queue<int> > &outputQueues, 
vector< pair< pair<int,int> , pair<double, int> > > &packetInformation)
{ 
    //INQ implementation
    if(queueType == "INQ")
    {
        int tmpSize = tmp.size();
        for(int i=0; i<tmpSize; i++)
        {
            if(tmp[i].first != -1)
            {    
                
                if(inputQueues[i].size()<bufferInSize)
                {
                    packetInformation.push_back({{i,tmp[i].second},{tmp[i].first,-1}});
                    inputQueues[i].push(packetInformation.size()-1);
                }
            }
        }
        int ports = inputQueues.size();
        map< int, vector<int> > hashMap; //outputPort, vector of input ports
        for(int i=0; i<ports; i++)
        {
            if(!inputQueues[i].empty())
            {
                hashMap[packetInformation[inputQueues[i].front()].first.second].push_back(i);
            }
        }
        int j=0;
        //Scheduling Part
        for(int i=0; i<ports; i++)
        {
            if(!inputQueues[i].empty())
            {
                if(hashMap[packetInformation[inputQueues[i].front()].first.second].size() == 1)
                {
                    if(outputQueues[packetInformation[inputQueues[i].front()].first.second].size()<bufferOutSize)
                    {
                        outputQueues[packetInformation[inputQueues[i].front()].first.second].push(inputQueues[i].front());
                        inputQueues[i].pop();
                    }
                }
                else if(hashMap[packetInformation[inputQueues[i].front()].first.second].size() >= 2 && j==0)
                {
                    if(outputQueues[packetInformation[inputQueues[i].front()].first.second].size() < bufferOutSize)
                    {
                        int hashmapisize = hashMap[packetInformation[inputQueues[i].front()].first.second].size();
                        uniform_int_distribution<int> distribution(0,hashmapisize-1);
                        int randIntPort = distribution(generator);
                        outputQueues[packetInformation[inputQueues[i].front()].first.second].push(inputQueues[hashMap[packetInformation[inputQueues[i].front()].first.second][randIntPort]].front());
                        inputQueues[hashMap[packetInformation[inputQueues[i].front()].first.second][randIntPort]].pop();
                        j = 1;
                    }

                }
            }

        }
    }//KOUQ implementation
    else if(queueType == "KOUQ")
    {
        int tmpSize = tmp.size();
        for(int i=0; i<tmpSize; i++)
        {
            if(tmp[i].first != -1)
            {    
                
                if(inputQueues[i].size()<bufferInSize)
                {
                    packetInformation.push_back({{i,tmp[i].second},{tmp[i].first,-1}});
                    inputQueues[i].push(packetInformation.size()-1);
                }
            }
        }
        int ports = inputQueues.size();
        map< int, vector<int> > hashMap; //outputPort, vector of input ports
        int countPortsMoreThanK=0;
        for(int i=0; i<ports; i++)// here i means input port
        {
            if(!inputQueues[i].empty())
            {
                
                int hashkey = packetInformation[inputQueues[i].front()].first.second;
                hashMap[hashkey].push_back(-1);
                int size = hashMap[hashkey].size();
                int j = size-2;
                while(j>=0)
                {   
                    if(packetInformation[inputQueues[i].front()].second.first <= 
                    packetInformation[inputQueues[hashMap[hashkey][j]].front()].second.first)
                    {
                        hashMap[hashkey][j+1] = hashMap[hashkey][j];
                    }
                    else
                        break;
                    j--;
                    
                }
                hashMap[hashkey][j+1] = i;
            }
        }

        for(int i=0; i<ports; i++) // here i means output port
        {
            int check = 0;//to count countPortMoreThanK;
            if(hashMap[i].size() == 1)
            {
                if(outputQueues[i].size()<bufferOutSize)
                {
                    outputQueues[i].push(inputQueues[hashMap[i][0]].front());
                    inputQueues[hashMap[i][0]].pop();
                }
            }
            else if(hashMap[i].size()>=2)
            {
                int hashkeysize = hashMap[i].size();
                int j=outputQueues[i].size(), k=0;
                while(j < bufferOutSize && k < hashkeysize)
                {
                    outputQueues[i].push(inputQueues[hashMap[i][k]].front());
                    inputQueues[hashMap[i][k]].pop();
                    j++;k++;
                }
                while(k<hashkeysize)
                {
                    if(check == 0)
                        countPortsMoreThanK++;
                    check = 1;
                    inputQueues[hashMap[i][k]].pop();
                    k++;
                }
            }

        }
        countMoreThanKProb += (countPortsMoreThanK/double(ports));


    }
    else  //ISLIP implementation
    {
        int ports = tmp.size();

        vector<int> Matching(ports); //input ports to output ports maximal size matching 
        fill(Matching.begin(),Matching.end(),-1);
        vector<bool> outPortAfterOneRound(ports);
        fill(outPortAfterOneRound.begin(), outPortAfterOneRound.end(), false);
        //ISLIP Req. Phase
        for(int i=0; i<ports; i++)
        {
            if(tmp[i].first != -1)
            {
                if(islipReqInfo[i].size()<bufferInSize)
                {
                    packetInformation.push_back({{i,tmp[i].second},{tmp[i].first,-1}});
                    islipReqInfo[i].push_back(packetInformation.size()-1);

                }
            }
        }
        vector<int>::iterator it1;
        //Grant and Accept Phase for all iterations
        for(int i=0; i<ports; i++)
        {
            
            if(!islipReqInfo[i].empty())
            {
                it1 = islipReqInfo[i].begin();
                int size = islipReqInfo[i].size();
                for(int j=0; j<size; j++)
                {

                    int outputPortOfJthPacket = packetInformation[islipReqInfo[i][j]].first.second;
                    if(outPortAfterOneRound[outputPortOfJthPacket] == false)
                    {
                        Matching[i] = islipReqInfo[i][j];
                        islipReqInfo[i].erase(it1+j);
                        outPortAfterOneRound[outputPortOfJthPacket] = true;
                        break; 
                    }
                    

                }
            }
        }
        for(int i=0; i<ports; i++)
        {
            if(Matching[i] != -1)
            {
                if(outputQueues[i].size() < bufferOutSize)
                    outputQueues[packetInformation[Matching[i]].first.second].push(Matching[i]);
            }
        }


        
    }
    


}
//Packet Transmission Phase
void packetTransmission(int t, vector< queue<int> > &outputQueues, vector< pair< pair<int,int> , pair<double, int> > > &packetInformation)
{
    int ports = outputQueues.size();
    for(int i=0; i<ports; i++)
    {
        if(!outputQueues[i].empty())
        {
            int packetNum = outputQueues[i].front();
            outputQueues[i].pop();
            packetInformation[packetNum].second.second = t;
        }
    }

}
/*./a.out -N switchportcount −B buffersize −p packetgenprob −queue QueueType −K knockout −out outputfile −T maxtimeslots*/
// 0       1     2            3     4       5     6            7      8       9    10      11      12      13    14
int main(int args, char *argv[])  
{
    

    int totalPort, bufferInSize, bufferOutSize, maxTimeSlot;
    string queueType, outputfile;
    double packetGenProb;
    totalPort = atoi(argv[2]);
    bufferInSize = atoi(argv[4]);
    maxTimeSlot = atoi(argv[14]);
    queueType = argv[8];
    packetGenProb = atof(argv[6]);
    outputfile = argv[12];

    if(queueType == "KOUQ")
    {
        bufferOutSize = atof(argv[10]);
    }
    else
    {
        bufferOutSize = bufferInSize;
    }
    
    islipReqInfo.resize(totalPort);
    vector< queue<int> > inputQueues(totalPort);
    vector< queue<int> > outputQueues(totalPort);
    vector< pair< pair<int,int> , pair<double, int> > >packetInformation;  //intputport, outputport,arrival time, departureTime
    
    //Simulation till maxTimeSlot
    for(int t=1; t<=maxTimeSlot; t++)
    {
        if(t==1)
        {
            trafficGeneration(t,totalPort,packetGenProb);
        }
        else if(t==2)
        {
            
            packetScheduling(t,bufferInSize, bufferOutSize, queueType, inputQueues, outputQueues, packetInformation);
            trafficGeneration(t,totalPort, packetGenProb);
            
        }
        else
        {
            packetTransmission(t, outputQueues, packetInformation);
            packetScheduling(t, bufferInSize, bufferOutSize, queueType, inputQueues, outputQueues, packetInformation);
            trafficGeneration(t,totalPort, packetGenProb);
            
        }


    }

    int transmittedPackets = 0;
    double AvgPacketDelay = 0;
    double AvgStandDeviation = 0;
    
    for(int i=0; i<packetInformation.size(); i++)
    {

        if(packetInformation[i].second.second != -1)
        {
            transmittedPackets++;
            AvgPacketDelay += (packetInformation[i].second.second - floor(packetInformation[i].second.first));
            LinkUtilPerLink[{packetInformation[i].first.first,packetInformation[i].first.second}]++;
            
        }
        
    }
    AvgPacketDelay /= transmittedPackets;
    for(int i=0; i<packetInformation.size(); i++)
    {
        if(packetInformation[i].second.second != -1)
        {
            double base = (packetInformation[i].second.second - floor(packetInformation[i].second.first)) - AvgPacketDelay;
            AvgStandDeviation += pow(base,2);
        }
    }
    
    AvgStandDeviation = sqrt(AvgStandDeviation/transmittedPackets);
    cout<<"Average Packet delay is : "<<AvgPacketDelay<<", Avg Standard Deviation : "<<AvgStandDeviation<<endl<<endl;;
    cout<<"Transmitted Packets : "<<transmittedPackets<<",  Total Packets : "<<packetInformation.size()<<endl;
    int sum = 0;
    for(int i=0; i<totalPort; i++)
    {
        for(int j=0; j<totalPort; j++)
        {
            //cout<<"Link Utilization of Link "<<i<<"->"<<j<<" : "<<LinkUtilPerLink[{i,j}]<<endl;
            sum+=LinkUtilPerLink[{i,j}];
        }
    }
    double AvgLinkUtilization = (sum/double(totalPort))/maxTimeSlot;
    cout<<"Avg Link Utilization : "<<AvgLinkUtilization;
    cout<<endl;
    double KOUQDropProbability;
    KOUQDropProbability = countMoreThanKProb/maxTimeSlot;
    if(queueType == "KOUQ")
        cout<<"KOUQ drop probability : "<<KOUQDropProbability<<endl<<endl;

    //Output file information 
    ifstream fin(outputfile, ios::in); 
    if(fin.peek() == std::ifstream::traits_type::eof() )
    {
        // Empty File
        fin.close();
        ofstream fout(outputfile, ios::out | ios::app);
        fout<<setw(10)<<left<<"N";
        fout<<setw(10)<<left<<"P";
        fout<<setw(20)<<left<<"Queue Type";
        fout<<setw(30)<<left<<"Average Packet Delay";
        fout<<setw(40)<<left<<"Standard Deviation of Packet Delay";
        fout<<setw(40)<<left<<"Average Link Utilization";
        fout<<endl<<endl;
        fout<<setw(10)<<left<<totalPort;
        fout<<setw(10)<<left<<packetGenProb;
        fout<<setw(20)<<left<<queueType;
        fout<<setw(30)<<left<<AvgPacketDelay;
        fout<<setw(40)<<left<<AvgStandDeviation;
        fout<<setw(40)<<left<<AvgLinkUtilization;
        fout<<endl<<endl;
        fout.close();
    }
    else
    {
        fin.close();
        ofstream fout(outputfile, ios::out | ios::app);
        fout<<setw(10)<<left<<totalPort;
        fout<<setw(10)<<left<<packetGenProb;
        fout<<setw(20)<<left<<queueType;
        fout<<setw(30)<<left<<AvgPacketDelay;
        fout<<setw(40)<<left<<AvgStandDeviation;
        fout<<setw(40)<<left<<AvgLinkUtilization;
        fout<<endl<<endl;
        fout.close();
    }
   
    return 0;
}