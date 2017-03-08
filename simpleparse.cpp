#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>

using  namespace std;


struct inst {
    int assembly;
    int count;
};


double avgFrequencyInFirstPage;
int currentIdx;
FILE* fp;
int totalNum;
int differentNum;
char buffer [1000];
vector <struct inst> head [10];
vector <struct inst> common;


void _register_instrcution (int assembly)
{
#ifdef DEBUG
    printf("%x\n",assembly);
#endif

    /*
    if (head == NULL)
    {
        differentNum++;
        struct inst* added = new inst;
        added->assembly = assembly;
        added->count = 1;
        head[currentIdx].push_back(*added);
        return;
    }
    */



    int i;
    for (i = 0; i <head[currentIdx].size(); i++)
    {
        

        if (head[currentIdx][i].assembly == assembly)
        {
            head[currentIdx][i].count ++;
            break;
        }


    }

    if (i == head[currentIdx].size())
    {
        differentNum++;
        struct inst* added = new inst;
        added->assembly = assembly;
        added->count = 1;
        head[currentIdx].push_back (*added);
    }

}

void _register_common_instrcution (int assembly,int count)
{
    int i;
    for (i = 0; i <common.size(); i++)
    {
        

        if (common[i].assembly == assembly)
        {
            common[i].count += count;
            break;
        }


    }

    if (i == common.size())
    {
        struct inst* added = new inst;
        added->assembly = assembly;
        added->count = count;
        common.push_back (*added);
    }
}


void register_instruction (int index)
{


    int assembly = 0;
    char temp;
    int i;
    totalNum++;

    for (i = 0; i < 8; i++)
    {
        temp = buffer[index+i-7];
        if ( (temp >= '0' && temp <= '9'))
            assembly = assembly*16 + (temp-48);
        else 
            assembly = assembly*16 + (temp-87); 

    }


    _register_instrcution (assembly);

}


int parse (void)
{

    if (fgets(buffer,1000,fp) == NULL)
        return 0;

    if (buffer[0] == '\n')
    {
        if (fgets(buffer,1000,fp) == NULL)
            return 0;
        return 1;
    }

    int count = 0;
    int i;
    for (i = 0; buffer[i] != '\n'; i++)
    {
        if ( (buffer[i] >= '0' && buffer[i] <= '9') ||  (buffer[i] >= 'a' && buffer[i] <= 'f' ))
            count ++;
        else 
            count = 0;

        if (count == 8)
        {  
            if ( buffer[i+1] != ':' )
            {    register_instruction(i);
                return 1;
            }
            count = 0;
        }
    }

    return 1;


}

bool compareByCount(const inst &a, const inst &b)
{
        return a.count > b.count;
}

void showResult (void)
{



    sort(head[currentIdx].begin(),head[currentIdx].end(),compareByCount);

    printf("total num : %d differentNum : %d\n",totalNum,differentNum);


    for (int j = 0; j < 8; j++)
    {
        int pageCounter = 0;

        for (int i = 0; i < 1024; i++)
        {
            pageCounter =  pageCounter + head[currentIdx][i + j*1024 ].count;
        }

        if (j == 0)
            avgFrequencyInFirstPage += (double)pageCounter/totalNum;

        printf("[%d] page count (descent order) (4KB) : %d\n",j+1,pageCounter);
    }

}

void init (int index)
{
    totalNum = 0;
    differentNum = 0;
    currentIdx = index;
}

int getMatchNum (int a, int b)
{
    int ret = 0;
    // page num
    for (int i = 0; i < 1; i++)
    {
        for (int j = 0 + 1024*i ; j < 1024 + 1024*i; j++)
        {
            for (int k = 0 + 1024*i ; k <1024 + 1024*i; k++)
            {
                if (head[a][j].assembly == head[b][k].assembly)
                {    
                    ret++;
                    break;
                }
            }
        }
    }

    return ret;
}

int getMatchCommonNum (int a)
{
    int ret = 0;
    
    // page num
    for (int i = 0; i < 1; i++)
    {
        for (int j = 0 + 1024*i ; j < 1024 + 1024*i; j++)
        {
            for (int k = 0 + 1024*i; k < 1024 + 1024*i; k++)
            {
                if (common[j].assembly == head[a][k].assembly)
                {    
                    ret++;
                    break;
                }
            }
        }
    }

    return ret;
}

void showCorrelation (int num)
{
    for (int i = 0; i < num; i++)
        for (int j = i+1; j < num; j++)
        {
            printf("[%d] [%d] match num : %d on 2 page\n",i,j,getMatchNum(i,j));
            
        }

    avgFrequencyInFirstPage = avgFrequencyInFirstPage/(num+1);

    printf("avgFrequencyInFirstPage : %lf\n",avgFrequencyInFirstPage);
}

void showCommonCorrelation (int num)
{
    for (int i = 0; i < num; i++)
            printf("[%d] commonmatch num : %d on 1 page\n",i,getMatchCommonNum(i));
            
}


void makeCommonInst (int num)
{
    
    for (int i = 0; i < num; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            for (int k = 0 + 1024*j ; k < 1024 + 1024*j; k++)
            {

                _register_common_instrcution(head[i][k].assembly,head[i][k].count);
            }
        }
    }

    sort (common.begin(),common.end(),compareByCount);
}



int main (int argc, char* argv[])
{
    int i,j;

    for (i = 1; i < argc ; i++)
    {
        if ( ( fp = fopen(argv[i],"r")) != NULL )
        {

            init (i-1);

            printf("start \n");

            for (j = 0; j < 7; j++)
                fgets(buffer,1000,fp);

            while(parse());

            showResult();

        }
        else 
        {

            return 0;
        }

    }

    showCorrelation(argc-1);
    makeCommonInst(argc-1);
    showCommonCorrelation(argc-1);

}

