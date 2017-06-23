/*
* Created by ShiJiahuan(li) in haut.
* for more please visit www.shallweitalk.com
* 
* Copyright 2017 SJH. All rights reserved.
*
* @Author: Haut-Stone
* @Date:   2017-06-23 09:16:43
* @Last Modified by:   Haut-Stone
* @Last Modified time: 2017-06-23 15:06:53
*/

//This is a small huffman compress and extract program.
//

#include <algorithm>
#include <iostream>
#include <cstring>
#include <vector>
#include <cstdio>
#include <queue>
#include <stack>
#include <cmath>
#include <map>
#include <set>
using namespace std;

const int N = 1000;
const int INF = 99999999;
const int BUFFERSIZE = 256;
const int ASCLLSIZEPLUS = 300;

struct Node
{
	int id;
	char value;
	double weight;
	int parent;
	int lChild;
	int rChild;
	int vis;
	int flag;

	Node(){
		id = 0;
		value = 0;
		weight = 0;
		parent = 0;
		lChild = 0;
		rChild = 0;
		vis = 0;
		flag = 0;
	}
	Node(int I, int VA, int W, int P, int L, int R, int VI, int F){
		id = I;
		value = VA;
		weight = W;
		parent = P;
		lChild = L;
		rChild = R;
		vis = VI;
		flag = F;
	}
}huffmanNode[N];

char command[N];
char systemBuffer[BUFFERSIZE];
int allCharNum;
int allInfoNum;
int allNodeNum;
int allByteNum;
int allCnt;
int allsupplement;
stack<string> format, temp;
string a = "|   ";
string b = "    ";
map<char, double> Info;
map<char, string> Code;

void help();

//
void compress();
void probabilityStatistics();
void generateInfoDictionary();
void generateHuffmanNode();
void generateCodeDictionary();
void saveHuffmanNode();
void generateTree(int NodeId);
void aTob();
void generateResult();

//
void extract();
void importHuffmanNode();
void bToa();

int main(void)
{
	cout<<"HELLO"<<endl;
	while(1){
		while(strcmp(command, "help") != 0 && strcmp(command, "compress") != 0 && strcmp(command, "extract") != 0 && strcmp(command, "exit") != 0){
			cin>>command;
			cin.clear();
			cin.ignore(1024, '\n');
			if(strcmp(command, "help") != 0 && strcmp(command, "compress") != 0 && strcmp(command, "extract") != 0 && strcmp(command, "exit") != 0){
				cout<<"command can't find"<<endl;
			}
		}
		if(strcmp(command, "help") == 0){
			help();
			memset(command, 0, sizeof(command));
		}else if(strcmp(command, "compress") == 0){
			compress();
			extract();
			memset(command, 0, sizeof(command));
		}else if(strcmp(command, "extract") == 0){
			memset(command, 0, sizeof(command));
		}else if(strcmp(command, "exit") == 0){
			exit(0);
		}else{
			exit(1);
		}
	}
	return 0;
}

void compress()
{
	probabilityStatistics();
	generateInfoDictionary();
	generateHuffmanNode();
	generateCodeDictionary();
	saveHuffmanNode();
	generateTree(allNodeNum);
	aTob();
}

void extract()
{
	importHuffmanNode();
	bToa();
}

void probabilityStatistics()
{
	FILE *readFile, *writeFile;
	readFile = fopen("/Users/li/GitHub/huffmanCompress/content.txt", "r");
	writeFile = fopen("/Users/li/GitHub/huffmanCompress/Info.txt", "w");

	char soloChar;
	int frequency[ASCLLSIZEPLUS];
	allCharNum = 0;
	allInfoNum = 0;

	memset(frequency, 0, sizeof(frequency));

	while(fscanf(readFile, "%c", &soloChar) != EOF){
		allCharNum++;
		frequency[int(soloChar)]++;
	}

	for(int i=32;i<=126;i++){
		if(frequency[i] != 0) allInfoNum++;
	}
	if(frequency[10] != 0) allInfoNum++;

	fprintf(writeFile, "%d\n", allInfoNum);
	for(int i=32;i<=126;i++){
		if(frequency[i] != 0){
			fprintf(writeFile, "%c:%f\n", char(i), double(frequency[i])/allCharNum);
		}
	}
	if(frequency[10] != 0) fprintf(writeFile, "\n:%f\n", double(frequency[10])/allCharNum);
	fclose(readFile);
	fclose(writeFile);
}

void generateInfoDictionary()
{
	FILE *readFile;
	readFile = fopen("/Users/li/GitHub/huffmanCompress/Info.txt", "r");

	char soloChar;
	int useless;
	char format;
	double soloValue;

	fscanf(readFile, "%d", &useless);
    fgetc(readFile);
    for(int i=1;i<=allInfoNum;i++){
        fscanf(readFile, "%c%c%lf", &soloChar, &format, &soloValue);
        fgetc(readFile);
        Info[soloChar] = soloValue;
    }
    fclose(readFile);
}

void generateHuffmanNode()
{
	memset(huffmanNode, 0, sizeof(huffmanNode));
	allCnt = 1;
	for(map<char, double>::iterator it = Info.begin();it!=Info.end();it++,allCnt++){
		huffmanNode[allCnt].id = allCnt;
		huffmanNode[allCnt].value = it->first;
		huffmanNode[allCnt].weight = it->second;
	}
	allNodeNum = (--allCnt)*2-1;

	//之后用优先队列来代替
	for(int k=allCnt+1;k<=allNodeNum;k++){
		double min1 = INF;
		double min2 = INF;
		int id1 = 0;
		int id2 = 0;
		for(int i=1;i<=k-1;i++){
			if(!huffmanNode[i].vis){
				if(huffmanNode[i].weight < min2){
					if(huffmanNode[i].weight < min1){
						min2 = min1;
						id2 = id1;
						min1 = huffmanNode[i].weight;
						id1 = i;
					}else{
						min2 = huffmanNode[i].weight;
						id2 = i;
					}
				}
			}
		}

		huffmanNode[id1].parent = k;
		huffmanNode[id1].vis = 1;
		huffmanNode[id2].parent = k;
		huffmanNode[id2].vis = 1;
		huffmanNode[k].id = k;
		huffmanNode[k].weight = min1 + min2;
		huffmanNode[k].lChild = id1;
		huffmanNode[k].rChild = id2;
	}
}

void generateCodeDictionary()
{
	char soloCode[N];
	soloCode[allCnt-1] = '\0';
	for(int i=1;i<=allCnt;i++){
		int start = allCnt-1;
		int now = i;
		char nowValue = huffmanNode[now].value;
		int father = huffmanNode[i].parent;
		while(father != 0){
			if(huffmanNode[father].lChild == now){
				soloCode[--start] = '0';
			}else{
				soloCode[--start] = '1';
			}
			now = father;
			father = huffmanNode[father].parent;
		}
		char temp[N];
		strcpy(temp, &soloCode[start]);
		Code[nowValue] = temp;
	}
}

void saveHuffmanNode()
{
	FILE *huffmanNodeFile;
	huffmanNodeFile = fopen("/Users/li/GitHub/huffmanCompress/huffmanNode", "wb");
	fwrite(huffmanNode+1, sizeof(Node), allNodeNum, huffmanNodeFile);
	fclose(huffmanNodeFile);
}

void aTob()
{

	FILE *readFile;
	readFile = fopen("/Users/li/GitHub/huffmanCompress/content.txt", "r");

	memset(systemBuffer, 0, sizeof(systemBuffer));

	char soloChar;
	char soloCode[N];
	int len;
	int systemBufferPtr = 0;

	while(fscanf(readFile, "%c", &soloChar) != EOF){
		strcpy(soloCode, Code[soloChar].c_str());
		len = strlen(soloCode);
		for(int i=0;i<len;i++){
			systemBuffer[systemBufferPtr] = soloCode[i];
			if(systemBufferPtr == 255){
				generateResult();
				memset(systemBuffer, 0, sizeof(systemBuffer));
				systemBufferPtr = 0;
			}else{
				systemBufferPtr++;
			}
		}
	}
	generateResult();
	fclose(readFile);
}

void generateResult()
{
	FILE *resultFile;
	resultFile = fopen("/Users/li/GitHub/huffmanCompress/result", "ab");

	unsigned char soloByte;
	int soloCnt = 0;
	int soloAns = 0;

	if(strlen(systemBuffer) == 256){
		allByteNum += 32;
		for(int i=0;i<256;i++){
			soloAns = soloAns * 2 + (systemBuffer[i] - '0');
			soloCnt++;
			if(soloCnt == 8){
				soloCnt = 0;
				soloByte = char(soloAns);
				fwrite(&soloByte, sizeof(unsigned char), 1, resultFile);
				soloAns = 0;
			}
		}
	}else{
		allByteNum += strlen(systemBuffer) / 8;
		for(int i=0;i<strlen(systemBuffer);i++){
			soloAns = soloAns * 2 + (systemBuffer[i] - '0');
			soloCnt++;
			if(soloCnt == 8){
				soloCnt = 0;
				soloByte = char(soloAns);
				fwrite(&soloByte, sizeof(unsigned char), 1, resultFile);
				soloAns = 0;
			}
		}

		if(soloCnt != 0){
			allByteNum += 1;
			allsupplement = 8 - soloCnt;
			while(soloCnt<8){
				soloAns = soloAns * 2;
				soloCnt++;
			}
			soloByte = char(soloAns);
			fwrite(&soloByte, sizeof(unsigned char), 1, resultFile);
		}
	}
	fclose(resultFile);
}

void generateTree(int NodeId)
{
	FILE *writeFile;
	writeFile = fopen("/Users/li/GitHub/huffmanCompress/tree.txt", "a");

	string c;

	while(!format.empty()){
		c = format.top();
		format.pop();
		temp.push(c);
	}

	while(!temp.empty()){
		c = temp.top();
		fprintf(writeFile, "%s", c.c_str());
		temp.pop();
		format.push(c);
	}

	
	if(huffmanNode[NodeId].lChild == 0 && huffmanNode[NodeId].rChild == 0){
		if(huffmanNode[NodeId].value == '\n'){
			fprintf(writeFile, "+---\\n:%f\n", huffmanNode[NodeId].weight);
		}else{
			fprintf(writeFile, "+---%c:%f\n", huffmanNode[NodeId].value, huffmanNode[NodeId].weight);
		}
	}else{
		fprintf(writeFile, "+---%f\n", huffmanNode[NodeId].weight);
	}

	//如果父节点有左儿子
	if(huffmanNode[huffmanNode[NodeId].parent].lChild != 0 && huffmanNode[huffmanNode[NodeId].parent].flag == 0){
		format.push(a);
		huffmanNode[huffmanNode[NodeId].parent].flag = 1;
	}else{
		format.push(b);
	}
	
	if(huffmanNode[NodeId].rChild != 0) generateTree(huffmanNode[NodeId].rChild);
	if(huffmanNode[NodeId].lChild != 0) generateTree(huffmanNode[NodeId].lChild);

	format.pop();
}

void help()
{
	printf("Usage:\n");
	printf("  <command> [options]\n\n");
	printf("Commands:\n");
	printf("  %-20s %-20s\n", "help", "Show help for commands.");
	printf("  %-20s %-20s\n", "compress", "compress ascll file to binary");
	printf("  %-20s %-20s\n", "extract", "extract binary file to ascll");
	printf("  %-20s %-20s\n\n", "exit", "exit program");
	printf("General Options:\n");
	printf("  None\n");
}

void bToa()
{
	FILE *readFile, *writeFile;
	readFile = fopen("/Users/li/GitHub/huffmanCompress/result", "rb");
	writeFile = fopen("/Users/li/GitHub/huffmanCompress/back.txt", "w");
	int soloInt;
	char soloTuple[9];
	int soloTuplePtr = 8;
	soloTuple[soloTuplePtr--] = '\0'; 
	unsigned char inCode[999999];
	fread(inCode, sizeof(unsigned char), allByteNum, readFile);

	for(int i=0;i<allByteNum;i++){
		soloInt = int(inCode[i]);

		while(soloTuplePtr >= 0){
			soloTuple[soloTuplePtr--] = (soloInt % 2) + '0';
			soloInt = soloInt / 2;
		}

		fprintf(writeFile, "%s", soloTuple);
		soloTuplePtr = 8;
		soloTuple[soloTuplePtr--] = '\0';
	}
	fclose(readFile);
	fclose(writeFile);

	readFile = fopen("/Users/li/GitHub/huffmanCompress/back.txt", "r");
	writeFile = fopen("/Users/li/GitHub/huffmanCompress/ascll.txt", "w");

	char inChar[999999];
	int len;
	int mapLen;
	int first = 0;

	fscanf(readFile ,"%s", inChar);
	len = strlen(inChar);

	len -= allsupplement;
	
	while(first < len){
		for(map<char, string>::iterator it = Code.begin();it!=Code.end();it++){
			mapLen = it->second.length();
			char temp[N];
			for(int i=0;i<mapLen;i++){
				temp[i] = inChar[first + i];
				temp[i+1] = '\0';
			}
			if(strcmp(temp, it->second.c_str()) == 0){
				cout<<it->first;
				fprintf(writeFile, "%c", it->first);
				first += mapLen;
				break;
			}
		}
	}
	fprintf(writeFile, "\n");
	fclose(readFile);
	fclose(writeFile);
}

void importHuffmanNode()
{
	FILE *huffmanNodeFile;
	huffmanNodeFile = fopen("/Users/li/GitHub/huffmanCompress/huffmanNode", "rb");
	fread(huffmanNode+1, sizeof(Node), allNodeNum, huffmanNodeFile);
	fclose(huffmanNodeFile);
}