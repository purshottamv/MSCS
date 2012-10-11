/* ============================================================================
 Name        : sim_p.c
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : Program simulating Instruction Level Parallelism
 ============================================================================
 */
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>

#define E_SUCCESS	1
#define E_FAIL		0

#define MAXCHARS 200

using namespace std;
/**********************************************************************
1. Parse the arch_params_file and store all the Op-Codes into a map structure
2. Parse the assembly code file and execute it.
**********************************************************************/
map<string,int> opcode_cycles;
map<string,int> integer_registers;
map<string,double> float_registers;
map<string,long> mlabels;
vector<string> vprogram;

char *mem;
// char **program;
long lCycleCount,lInstructionCount;
int programCounter=0;

void ltrim(string&);
void rtrim(string&);
bool validateRegister(bool, char *);

int main(int argc, char *argv[])
{
	int i;
	char *archParamFile, *initMemFile, *assemblyFile, *outputFile;
	int parseArchParams(char *);
	void initializeRegisters();
	int parseAssemblyFile(char *);
	void executeProgram();
	bool copyMemoryFile(char *);
	bool dumpMemoryFile(char *);
	if(argc < 5)
	{
		cout<<"Error : Parameters missing : sim_p <arch_params_file> <init_mem_file> <assembly_file> <output_file>";
		exit(1);
	}

	archParamFile = argv[1];
	initMemFile = argv[2];	
	assemblyFile = argv[3];
	outputFile =  argv[4];

	initializeRegisters();
	if(!parseArchParams(archParamFile))
		exit(1);
	else if(!parseAssemblyFile(assemblyFile))
		exit(1);
	else if(!copyMemoryFile(initMemFile))
		exit(1);

	cout<<"\n------------ EXECUTING THE PROGRAM ----------------"<<endl;
	executeProgram();
	cout<<"\nTotal Cycles = "<<lCycleCount<<endl;
	cout<<"\nTotal Instructions = "<<lInstructionCount<<endl;
	cout<<"\nCPI = "<<(double)lCycleCount/lInstructionCount<<endl;

	if(!dumpMemoryFile(outputFile))
		exit(1);

	return 1;
}

void executeProgram()
{
	lInstructionCount=0;
	lCycleCount = 0;
	int i,icurrentLine=0;
	bool bRetVal = E_SUCCESS;
	char c = 'A';
	ifstream in;
	string sLine, slabel, sOpCode;
	char cOpCode[10],cCycles[4];
	//vector<string>::size_type ix = 0; 

	bool executeNOOP(string& sOpCode,string& sLine);
	bool executeADD(string& sOpCode,string& sLine);
	bool executeSUB(string& sOpCode,string& sLine);
	bool executeMUL(string& sOpCode,string& sLine);
	bool executeDIV(string& sOpCode,string& sLine);
	bool executeL(string& sOpCode,string& sLine);
	bool executeLI(string& sOpCode,string& sLine);
	bool executeS(string& sOpCode,string& sLine);
	bool executeBEQ(string& sOpCode,string& sLine);
	bool executeBNEQ(string& sOpCode,string& sLine);
	bool executeBGTZ(string& sOpCode,string& sLine);
	bool executeBLTZ(string& sOpCode,string& sLine);
	bool executeMOVE(string& sOpCode,string& sLine);
	
// Initialize the memory
/*		float val = 45.9754;
		char number[9];
		memset(&number,'\0',9);
		for(int j= 0; j <=100; j=j+8)
		{
			sprintf(number,"%f",val);
			memcpy(&mem[0+j],&number,8);
			val++;
			memset(&number,'\0',9);
			memcpy(&number,&mem[0+j],8);
			cout<<"Value = "<<atof(number)<<endl;

			sprintf(number,"%f",val);
			memcpy(&mem[800+j],(char *)&number,8);
			val++;
			memset(&number,'\0',9);
			memcpy(&number,&mem[800+j],8);
			cout<<"Value = "<<atof(number)<<endl;
		}
*/
	while(programCounter != vprogram.size() && bRetVal)
	{
		lInstructionCount++;
		sLine = vprogram[programCounter];
		if(sLine.length() > 0 && (i = sLine.find(" ")) > 0)
		{
			memset(cOpCode,'\0',10);
			sLine.copy(cOpCode,i,0);
			cOpCode[i] = '\0';
			sOpCode.assign(sLine,0,i);
			sLine.assign(sLine,i,sLine.length()-i);
			ltrim(sLine);
			cout<<endl<<sOpCode<<" "<<sLine<<" "<<endl;
			if(sOpCode.compare("NOOP") == 0)
				bRetVal = executeNOOP(sOpCode,sLine);
			else if(sOpCode.compare(0,3,"ADD") == 0)
				bRetVal = executeADD(sOpCode,sLine);
			else if(sOpCode.compare(0,3,"SUB") == 0)
				bRetVal = executeSUB(sOpCode,sLine);
			else if(sOpCode.compare(0,3,"MUL") == 0)
				bRetVal = executeMUL(sOpCode,sLine);
			else if(sOpCode.compare(0,3,"DIV") == 0)
				bRetVal = executeDIV(sOpCode,sLine);
			else if(sOpCode.compare(0,2,"LI") == 0)
				bRetVal = executeLI(sOpCode,sLine);
			else if(sOpCode.compare(0,1,"L") == 0)
				bRetVal = executeL(sOpCode,sLine);
			else if(sOpCode.compare(0,1,"S") == 0)
				bRetVal = executeS(sOpCode,sLine);
			else if(sOpCode.compare(0,3,"BEQ") == 0)
				bRetVal = executeBEQ(sOpCode,sLine);
			else if(sOpCode.compare(0,4,"BNEQ") == 0)
				bRetVal = executeBNEQ(sOpCode,sLine);
			else if(sOpCode.compare(0,4,"BGTZ") == 0)
				bRetVal = executeBGTZ(sOpCode,sLine);
			else if(sOpCode.compare(0,4,"BLTZ") == 0)
				bRetVal = executeBLTZ(sOpCode,sLine);
			else if(sOpCode.compare(0,4,"MOVE") == 0)
				bRetVal = executeMOVE(sOpCode,sLine);
			else
			{
				cout<<"Invalid OpCode. Line No. "<<programCounter+1<<endl;
				bRetVal = E_FAIL;
			}
		}
		++programCounter;
	//	sleep(1);
	}
}

void initializeRegisters()
{
	integer_registers.insert(make_pair("R0",0));
	integer_registers.insert(make_pair("R1",0));
	integer_registers.insert(make_pair("R2",0));
	integer_registers.insert(make_pair("R3",0));
	integer_registers.insert(make_pair("R4",0));
	integer_registers.insert(make_pair("R5",0));
	integer_registers.insert(make_pair("R6",0));
	integer_registers.insert(make_pair("R7",0));
	integer_registers.insert(make_pair("R8",0));
	integer_registers.insert(make_pair("R9",0));
	integer_registers.insert(make_pair("R10",0));
	integer_registers.insert(make_pair("R11",0));
	integer_registers.insert(make_pair("R12",0));
	integer_registers.insert(make_pair("R13",0));
	integer_registers.insert(make_pair("R14",0));
	integer_registers.insert(make_pair("R15",0));

	float_registers.insert(make_pair("F0",0));
	float_registers.insert(make_pair("F1",0));
	float_registers.insert(make_pair("F2",0));
	float_registers.insert(make_pair("F3",0));
	float_registers.insert(make_pair("F4",0));
	float_registers.insert(make_pair("F5",0));
	float_registers.insert(make_pair("F6",0));
	float_registers.insert(make_pair("F7",0));
	float_registers.insert(make_pair("F8",0));
	float_registers.insert(make_pair("F9",0));
	float_registers.insert(make_pair("F10",0));
	float_registers.insert(make_pair("F11",0));
	float_registers.insert(make_pair("F12",0));
	float_registers.insert(make_pair("F13",0));
	float_registers.insert(make_pair("F14",0));
	float_registers.insert(make_pair("F15",0));

	integer_registers["R0"] = 0;
}

int parseArchParams(char *archParamFile)
{
	ifstream in;
	char cCommand[MAXCHARS];
	string sLine;
	char cOpCode[10],cCycles[4];
	int iCycles, iRetVal = E_SUCCESS;
	int i;

	in.open(archParamFile,ios::in);
	if(!in)
	{
		cout<<"Error Opening File "<<archParamFile<<endl;
		return E_FAIL;
	}
	while(!in.eof())
	{
		memset(&cCycles,'\0',4);
		memset(&cOpCode,'\0',10);
		in.getline(cCommand,MAXCHARS-1); 
		sLine.assign(cCommand);
		if(sLine.compare(0,1,"-") == 0)
		{
			break;
		}
		if((i=sLine.find(" ")) < 0)
		{
			iRetVal = E_FAIL;
			break;
		}
		sLine.copy(cOpCode,i,0);
		cOpCode[i] = '\0';
		if((i = sLine.find_last_of(" ")) < 0)
		{
			iRetVal = E_FAIL;
			break;
		}
		sLine.copy(cCycles,sLine.length()-i-1,i+1);
		if((iCycles = atoi(cCycles)) == 0)
		{
			iRetVal = E_FAIL;
			break;
		}
		opcode_cycles.insert(make_pair(cOpCode,iCycles));
	}
/*  Iterating the map
	map<string,int>::const_iterator it = opcode_cycles.begin();
	cout<<"Iterating the map"<<endl;
	while(it != opcode_cycles.end())
	{
		cout<<it->first<<" "<<it->second<<endl;
		++it;
	}
*/	
	if(iRetVal == E_FAIL)
	{	
		cout<<"File Format Error:\nMake sure Opcode and Number of Cycles are separated by at least one space"<<endl;
	}
	else
		in.close();
	return iRetVal;
}

int parseAssemblyFile(char *assemblyFile)
{
	int iRetVal = E_SUCCESS;
		
	ifstream in;
	char cCommand[MAXCHARS];
	string sLine, slabel;
	char cOpCode[10],cCycles[4];
	long iLineNumber = 0;
	int i;
	for(i=0;i<4;i++)
		cCycles[i] = '\0';
	for(i=0;i<10;i++)
		cOpCode[i] = '\0';

	in.open(assemblyFile,ios::in);
	if(!in)
	{
		cout<<"Error Opening File "<<assemblyFile<<endl;
		return E_FAIL;
	}
	cout<<"------------ START OF PROGRAM ----------------"<<endl;
	while(!in.eof())
	{
		in.getline(cCommand,MAXCHARS-1);
//		cout<<cCommand<<endl;
		sLine.assign(cCommand);	
	// Step 1 : Trim down the comment part first
		if((i = sLine.find(";")) >=0)
			sLine.assign(sLine,0,i);
	//	cout<<"; found at "<<i<<endl;
	//	cout<<"String = "<<sLine<<endl;
	
	// Step 2 : Check for the label. If found mark it and trim it
		if((i = sLine.find(":")) >=0)
		{	
//			cout<<": found at "<<i<<endl;
//			cout<<"sLine length = "<<sLine.length()<<endl;
			slabel.assign(sLine,0,i);
			sLine.assign(sLine,i+1,sLine.length()-i);
//			cout<<"sLine = "<<sLine<<endl;
//			cout<<"sLabel = "<<slabel<<endl;
			mlabels.insert(make_pair(slabel,iLineNumber));
		}
		ltrim(sLine);
	// Step 3 : Check for Opcode
		if(sLine.length() > 0 && (i = sLine.find(" ")) > 0)
		{
//			sLine.copy(cOpCode,i,0);
//			cOpCode[i] = '\0';
//			sLine.assign(sLine,i,sLine.length()-i);
//			cout<<"sLine before trimming = "<<sLine<<" length = "<<sLine.length()<<endl;
			ltrim(sLine);
			rtrim(sLine);
//			cout<<"opCode = "<<cOpCode<<endl;
//			cout<<"sLine after trimming = "<<sLine<<" length = "<<sLine.length()<<endl<<endl;
		}
		else
		{
			//iLineNumber++;
			continue;
		}

	// Step 4 : Save the program in the vector
		vprogram.push_back(sLine);
		cout<<sLine<<endl;
		iLineNumber++;
	}

			/*  Iterating the map and vector
				cout<<"Total lines parsed = "<<iLineNumber<<endl;
				map<string,long>::const_iterator it = mlabels.begin();
				cout<<"Iterating the labels map"<<endl;
				while(it != mlabels.end())
				{
					cout<<it->first<<" "<<it->second<<endl;
					++it;
				}
				cout<<"Iterating through the vector"<<endl;
				for (vector<string>::size_type ix = 0; ix != vprogram.size(); ++ix)
					cout<<vprogram[ix]<<endl;
			*/
	cout<<"------------ END OF PROGRAM ----------------"<<endl<<endl;
	return iRetVal;
}

void ltrim(string& sLine)
{
	int i =0;
	while(sLine.compare(i,1," ") == 0 && i < sLine.length())
		i++;
	sLine.assign(sLine,i,sLine.length()-i);
}

void rtrim(string& sLine)
{
	int i =sLine.length();
	while(sLine.compare(i-1,1," ") == 0 && i > 0)
		i--;
	sLine.assign(sLine,0,i);
}

bool executeNOOP(string& sOpCode,string& sLine)
{
	bool bRetVal = E_SUCCESS;
	char cOpCode[10];
	memset(&cOpCode,'\0',10);
	sOpCode.copy(cOpCode,sOpCode.length(),0);
	lCycleCount	+= opcode_cycles[cOpCode];
	cout<<opcode_cycles[cOpCode]<<" Cycle\n";
	return bRetVal;

}
bool executeADD(string& sOpCode,string& sLine)
{
	bool isDouble = 0;
	int i,icount=0;
	char reg[4];
	int *is1,*is2, *itarget;
	double *ds1,*ds2,*dtarget;

	if(sOpCode.find(".D") < sOpCode.length())
		isDouble = 1;
	while(icount < 3)
	{
		if((i = sLine.find(",")) > 0)
		{
			sLine.copy(reg,i,0);
			reg[i] = '\0';
		}
		else
		{
			sLine.copy(reg,sLine.length(),0);
			reg[sLine.length()] = '\0';
		}
		if(!validateRegister(isDouble,reg))
		{
			cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the target register
		{
			if(isDouble)
				dtarget = &float_registers[reg];
			else
				itarget = &integer_registers[reg];
		}
		else if(icount == 1)	// store the source 1 register
		{
			if(isDouble)
				ds1 = &float_registers[reg];
			else
				is1 = &integer_registers[reg];

			if(isDouble)
				cout<<*ds1<<" + ";
			else
				cout<<*is1<<" + ";
		}
		else if(icount == 2)	// store the source 2 register
		{
			if(isDouble)
				ds2 = &float_registers[reg];
			else
				is2 = &integer_registers[reg];
			if(isDouble)
				cout<<*ds2;
			else
				cout<<*is2;
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}
	if(isDouble)
		*dtarget = *ds1 + *ds2;
	else
		*itarget = *is1 + *is2;
	if(isDouble)
		cout<<" = "<<*dtarget<<endl;
	else
		cout<<" = "<<*itarget<<endl;

	char cOpCode[10];
	memset(&cOpCode,'\0',10);
	sOpCode.copy(cOpCode,sOpCode.length(),0);
	lCycleCount	+= opcode_cycles[cOpCode];
	cout<<opcode_cycles[cOpCode]<<" Cycle\n";

	return E_SUCCESS;
}
bool executeSUB(string& sOpCode,string& sLine)
{
	bool isDouble = 0;
	int i,icount=0;
	char reg[4];
	int *is1,*is2, *itarget;
	double *ds1,*ds2,*dtarget;

	if(sOpCode.find(".D") < sOpCode.length())
		isDouble = 1;
	while(icount < 3)
	{
		if((i = sLine.find(",")) > 0)
		{
			sLine.copy(reg,i,0);
			reg[i] = '\0';
		}
		else
		{
			sLine.copy(reg,sLine.length(),0);
			reg[sLine.length()] = '\0';
		}
		if(!validateRegister(isDouble,reg))
		{
			cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the target register
		{
			if(isDouble)
				dtarget = &float_registers[reg];
			else
				itarget = &integer_registers[reg];
		}
		else if(icount == 1)	// store the source 1 register
		{
			if(isDouble)
				ds1 = &float_registers[reg];
			else
				is1 = &integer_registers[reg];

			if(isDouble)
				cout<<*ds1<<" - ";
			else
				cout<<*is1<<" - ";
		}
		else if(icount == 2)	// store the source 2 register
		{
			if(isDouble)
				ds2 = &float_registers[reg];
			else
				is2 = &integer_registers[reg];
			if(isDouble)
				cout<<*ds2;
			else
				cout<<*is2;
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}
	if(isDouble)
		*dtarget = *ds1 - *ds2;
	else
		*itarget = *is1 - *is2;
	if(isDouble)
		cout<<" = "<<*dtarget<<endl;
	else
		cout<<" = "<<*itarget<<endl;

	char cOpCode[10];
	memset(&cOpCode,'\0',10);
	sOpCode.copy(cOpCode,sOpCode.length(),0);
	lCycleCount	+= opcode_cycles[cOpCode];
	cout<<opcode_cycles[cOpCode]<<" Cycle\n";

	return E_SUCCESS;
}

bool executeMUL(string& sOpCode,string& sLine)
{
	bool isDouble = 0;
	int i,icount=0;
	char reg[4];
	int *is1,*is2, *itarget;
	double *ds1,*ds2,*dtarget;

	if(sOpCode.find(".D") < sOpCode.length())
		isDouble = 1;
	while(icount < 3)
	{
		if((i = sLine.find(",")) > 0)
		{
			sLine.copy(reg,i,0);
			reg[i] = '\0';
		}
		else
		{
			sLine.copy(reg,sLine.length(),0);
			reg[sLine.length()] = '\0';
		}
		if(!validateRegister(isDouble,reg))
		{
			cout<<"Invalid register. "<<reg<<" Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the target register
		{
			if(isDouble)
				dtarget = &float_registers[reg];
			else
				itarget = &integer_registers[reg];
		}
		else if(icount == 1)	// store the source 1 register
		{
			if(isDouble)
				ds1 = &float_registers[reg];
			else
				is1 = &integer_registers[reg];

			if(isDouble)
				cout<<*ds1<<" * ";
			else
				cout<<*is1<<" * ";
		}
		else if(icount == 2)	// store the source 2 register
		{
			if(isDouble)
				ds2 = &float_registers[reg];
			else
				is2 = &integer_registers[reg];
			if(isDouble)
				cout<<*ds2;
			else
				cout<<*is2;
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}
	if(isDouble)
		*dtarget = *ds1 * *ds2;
	else
		*itarget = *is1 * *is2;
	if(isDouble)
		cout<<" = "<<*dtarget<<endl;
	else
		cout<<" = "<<*itarget<<endl;

	char cOpCode[10];
	memset(&cOpCode,'\0',10);
	sOpCode.copy(cOpCode,sOpCode.length(),0);
	lCycleCount	+= opcode_cycles[cOpCode];
	cout<<opcode_cycles[cOpCode]<<" Cycle\n";

	return E_SUCCESS;
}

bool executeDIV(string& sOpCode,string& sLine)
{
	bool isDouble = 0;
	int i,icount=0;
	char reg[4];
	int *is1,*is2, *itarget;
	double *ds1,*ds2,*dtarget;

	if(sOpCode.find(".D") < sOpCode.length())
		isDouble = 1;
	while(icount < 3)
	{
		if((i = sLine.find(",")) > 0)
		{
			sLine.copy(reg,i,0);
			reg[i] = '\0';
		}
		else
		{
			sLine.copy(reg,sLine.length(),0);
			reg[sLine.length()] = '\0';
		}
		if(!validateRegister(isDouble,reg))
		{
			cout<<"Invalid register. "<<reg<<" Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the target register
		{
			if(isDouble)
				dtarget = &float_registers[reg];
			else
				itarget = &integer_registers[reg];
		}
		else if(icount == 1)	// store the source 1 register
		{
			if(isDouble)
				ds1 = &float_registers[reg];
			else
				is1 = &integer_registers[reg];

			if(isDouble)
				cout<<*ds1<<" / ";
			else
				cout<<*is1<<" / ";
		}
		else if(icount == 2)	// store the source 2 register
		{
			if(isDouble)
				ds2 = &float_registers[reg];
			else
				is2 = &integer_registers[reg];
			if(isDouble)
				cout<<*ds2;
			else
				cout<<*is2;
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}
	if(isDouble)
		*dtarget = *ds1 / *ds2;
	else
		*itarget = *is1 / *is2;
	if(isDouble)
		cout<<" = "<<*dtarget<<endl;
	else
		cout<<" = "<<*itarget<<endl;

	char cOpCode[10];
	memset(&cOpCode,'\0',10);
	sOpCode.copy(cOpCode,sOpCode.length(),0);
	lCycleCount	+= opcode_cycles[cOpCode];
	cout<<opcode_cycles[cOpCode]<<" Cycle\n";

	return E_SUCCESS;
}
bool executeL(string& sOpCode,string& sLine)
{
	bool bRetVal = E_SUCCESS;
	bool isDouble = 0;
	int i,icount=0,i2;
	char reg[4],offset[10];
	int *is, *itarget;
	double *dtarget;

	if(sOpCode.find(".D") < sOpCode.length())
		isDouble = 1;

	if((i = sLine.find(",")) > 0)
	{
		sLine.copy(reg,i,0);
		reg[i] = '\0';
	}
	else
	{
		cout<<"Syntax Error. Line No. "<<programCounter+1<<endl;
		return E_FAIL;
	}
	if(!validateRegister(isDouble,reg))
	{
		cout<<"Invalid register. "<<reg<<" Program Line No. "<<programCounter+1<<endl;
		return E_FAIL;
	}
	sLine.assign(sLine,i+1,sLine.length()-i-1);
	if(isDouble)
		dtarget = &float_registers[reg];
	else
		itarget = &integer_registers[reg];

	if((i = sLine.find("(")) > 0)
	{
		sLine.copy(offset,i,0);
		offset[i] = '\0';
		if((i2 = sLine.find(")",i)) > 0)
		{
			memset(reg,'\0',4);
			sLine.copy(reg,i2-i-1,i+1);
			reg[i2-i] = '\0';
			if(!validateRegister(0,reg)) // Overriding the first argument
			{
				cout<<"Invalid register "<<reg<<". "<<reg<<" Program Line No. "<<programCounter+1<<endl;
				return E_FAIL;
			}
			is = &integer_registers[reg];
		}
		else
		{	
			cout<<"Syntax Error. Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
	}
	else
	{
		cout<<"Syntax Error. Line No. "<<programCounter+1<<endl;
		return E_FAIL;
	}
	if(isDouble)
	{
		char number[9];
		memset(&number,'\0',9);
		memcpy(&number,&mem[long((*is + atoi(offset)))],8);
		*dtarget = atof(number);
		cout<<*dtarget<<" Loaded from ("<<offset<<")"<<*is<<endl;
	}
	else
	{
		char number[5];
		memset(&number,'\0',5);	
		memcpy(&number,&mem[long((*is + atoi(offset)))],4);
		*itarget = atoi(number);
		cout<<*itarget<<" Loaded from ("<<offset<<")"<<*is<<endl;
	}

	char cOpCode[10];
	memset(&cOpCode,'\0',10);
	sOpCode.copy(cOpCode,sOpCode.length(),0);
	lCycleCount	+= opcode_cycles[cOpCode];
	cout<<opcode_cycles[cOpCode]<<" Cycle\n";

	return bRetVal;
}

bool executeLI(string& sOpCode,string& sLine)
{
	bool isDouble = 0;
	int i;
	char reg[4],immediate[20];
	int *itarget;
	
	memset(&immediate,'\0',20);
	memset(&reg,'\0',4);
	if((i = sLine.find(",")) > 0)
	{
		sLine.copy(reg,i,0);
		reg[i] = '\0';
	}
	else
	{
		cout<<"Syntax Error. Line No. "<<programCounter+1<<endl;
		return E_FAIL;
	}
	if(!validateRegister(isDouble,reg))
	{
		cout<<"Invalid register. "<<reg<<" Program Line No. "<<programCounter+1<<endl;
		return E_FAIL;
	}
	sLine.copy(immediate,sLine.length()-i,i+1);
	immediate[sLine.length()-i+1] = '\0';
	itarget = &integer_registers[reg];
	*itarget = atoi(immediate);
	cout<<reg<<" = "<<*itarget<<endl;

	char cOpCode[10];
	memset(&cOpCode,'\0',10);
	sOpCode.copy(cOpCode,sOpCode.length(),0);
	lCycleCount	+= opcode_cycles[cOpCode];
	cout<<opcode_cycles[cOpCode]<<" Cycle\n";

	return E_SUCCESS;
}

bool executeS(string& sOpCode,string& sLine)
{
	bool isDouble = 0;
	int i,icount=0,i2;
	char reg[4],offset[10];
	int *is, *itarget;
	double *ds,*dtarget;

	if(sOpCode.find(".D") < sOpCode.length())
		isDouble = 1;

	if((i = sLine.find(",")) > 0)
	{
		sLine.copy(reg,i,0);
		reg[i] = '\0';
	}
	else
	{
		cout<<"Syntax Error. Line No. "<<programCounter+1<<endl;
		return E_FAIL;
	}
	if(!validateRegister(isDouble,reg))
	{
		cout<<"Invalid register. "<<reg<<" Line No."<<programCounter+1<<endl;
		return E_FAIL;
	}
	sLine.assign(sLine,i+1,sLine.length()-i-1);
//	cout<<"sLine = "<<sLine<<endl;

	if(isDouble)
		dtarget = &float_registers[reg];
	else
		itarget = &integer_registers[reg];

	if((i = sLine.find("(")) > 0)
	{
		sLine.copy(offset,i,0);
		offset[i] = '\0';
//		cout<<"Offset = "<<offset<<endl;
		if((i2 = sLine.find(")",i)) > 0)
		{
			memset(reg,'\0',4);
			sLine.copy(reg,i2-i-1,i+1);
			reg[i2-i] = '\0';
			if(!validateRegister(0,reg)) // Overriding the first argument
			{
				cout<<"Invalid register. "<<reg<<" Line No. "<<programCounter+1<<endl;
				return E_FAIL;
			}
/*			if(isDouble)
				ds = &float_registers[reg];
			else
*/				is = &integer_registers[reg];
		}
		else
		{	
			cout<<"Syntax Error. Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
	}
	else
	{
		cout<<"Syntax Error. Line No. "<<programCounter+1<<endl;
		return E_FAIL;
	}
	if(isDouble)
	{
		char number[9];
		memset(&number,'\0',9);
		sprintf(number,"%f",*dtarget);
		memcpy(&mem[long((*is + atoi(offset)))],&number,8);
		cout<<*dtarget<<" stored in ("<<offset<<")"<<*is<<endl;
	}
	else
	{
		char number[5];
		memset(&number,'\0',5);	
		sprintf(number,"%d",*itarget);
		memcpy(&mem[long((*is + atoi(offset)))],&number,4);
		cout<<*itarget<<" stored in ("<<offset<<")"<<*is<<endl;
	}

	char cOpCode[10];
	memset(&cOpCode,'\0',10);
	sOpCode.copy(cOpCode,sOpCode.length(),0);
	lCycleCount	+= opcode_cycles[cOpCode];
	cout<<opcode_cycles[cOpCode]<<" Cycle\n";

	return E_SUCCESS;
}


bool executeBEQ(string& sOpCode,string& sLine)
{
	bool isDouble = 0;
	int i,icount=0;
	char reg[20];
	int *is1,*is2;
	double *ds1,*ds2;
	long *branchTarget;

	if(sOpCode.find(".D") < sOpCode.length())
		isDouble = 1;
	while(icount < 3)
	{
		if((i = sLine.find(",")) > 0)
		{
			sLine.copy(reg,i,0);
			reg[i] = '\0';
		}
		else
		{
			sLine.copy(reg,sLine.length(),0);
			reg[sLine.length()] = '\0';
		}
		if(icount < 2 && !validateRegister(isDouble,reg))
		{
			cout<<"Invalid register. "<<reg<<" Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the target register
		{
			if(isDouble)
				ds1 = &float_registers[reg];
			else
				is1 = &integer_registers[reg];
		}
		else if(icount == 1)	// store the source 1 register
		{
			if(isDouble)
				ds2 = &float_registers[reg];
			else
				is2 = &integer_registers[reg];
		}
		else if(icount == 2)	// store the source 2 register
		{
				if(isDouble) cout<<"s1 = "<<(*ds1)<<" s2 = "<<(*ds2)<<endl;
				else cout<<"s1 = "<<(*is1)<<" s2 = "<<(*is2)<<endl;

				if((isDouble && (*ds1) == (*ds2)) ||
					(!isDouble && (*is1) == (*is2)))
				{
					if(mlabels.count(reg) != 1)
					{
						cout<<"Invalid Label. Program Line No."<<programCounter<<endl;
						return E_FAIL;
					}
					branchTarget = &mlabels[reg];
					programCounter = *branchTarget - 1;
					cout<<"Jumping to Line "<<programCounter+2<<endl;
				}
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}

	char cOpCode[10];
	memset(&cOpCode,'\0',10);
	sOpCode.copy(cOpCode,sOpCode.length(),0);
	lCycleCount	+= opcode_cycles[cOpCode];
	cout<<opcode_cycles[cOpCode]<<" Cycle\n";

	return E_SUCCESS;
}
bool executeBNEQ(string& sOpCode,string& sLine)
{
	bool isDouble = 0;
	int i,icount=0;
	char reg[20];
	int *is1,*is2;
	double *ds1,*ds2;
	long *branchTarget;

	if(sOpCode.find(".D") < sOpCode.length())
		isDouble = 1;
	while(icount < 3)
	{
		if((i = sLine.find(",")) > 0)
		{
			sLine.copy(reg,i,0);
			reg[i] = '\0';
		}
		else
		{
			sLine.copy(reg,sLine.length(),0);
			reg[sLine.length()] = '\0';
		}
		if(icount < 2 && !validateRegister(isDouble,reg))
		{
			cout<<"Invalid register. "<<reg<<" Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the target register
		{
			if(isDouble)
				ds1 = &float_registers[reg];
			else
				is1 = &integer_registers[reg];
		}
		else if(icount == 1)	// store the source 1 register
		{
			if(isDouble)
				ds2 = &float_registers[reg];
			else
				is2 = &integer_registers[reg];
		}
		else if(icount == 2)	// store the source 2 register
		{
				if(isDouble) cout<<"s1 = "<<(*ds1)<<" s2 = "<<(*ds2)<<endl;
				else cout<<"s1 = "<<(*is1)<<" s2 = "<<(*is2)<<endl;

				if((isDouble && (*ds1) != (*ds2)) ||
					(!isDouble && (*is1) != (*is2)))
				{
					if(mlabels.count(reg) != 1)
					{
						cout<<"Invalid Label. Program Line No."<<programCounter<<endl;
						return E_FAIL;
					}
					branchTarget = &mlabels[reg];
					programCounter = *branchTarget - 1;
					cout<<"Jumping to Line "<<programCounter+2<<endl;
				}
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}

	char cOpCode[10];
	memset(&cOpCode,'\0',10);
	sOpCode.copy(cOpCode,sOpCode.length(),0);
	lCycleCount	+= opcode_cycles[cOpCode];
	cout<<opcode_cycles[cOpCode]<<" Cycle\n";

	return E_SUCCESS;
}
bool executeBGTZ(string& sOpCode,string& sLine)
{
	bool isDouble = 0;
	int i,icount=0;
	char reg[20];
	int *is1,*is2;
	double *ds1,*ds2;
	long *branchTarget;

	if(sOpCode.find(".D") < sOpCode.length())
		isDouble = 1;
	while(icount < 2)
	{
		if((i = sLine.find(",")) > 0)
		{
			sLine.copy(reg,i,0);
			reg[i] = '\0';
		}
		else
		{
			sLine.copy(reg,sLine.length(),0);
			reg[sLine.length()] = '\0';
		}
		if(icount < 1 && !validateRegister(isDouble,reg))
		{
			cout<<"Invalid register. "<<reg<<" Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the target register
		{
			if(isDouble)
				ds1 = &float_registers[reg];
			else
				is1 = &integer_registers[reg];
		}
		else if(icount == 1)	// store the source 2 register
		{
				if((isDouble && (*ds1) > 0) ||
					(!isDouble && (*is1) > 0))
				{
					if(mlabels.count(reg) != 1)
					{
						cout<<"Invalid Label. Program Line No."<<programCounter<<endl;
						return E_FAIL;
					}
					branchTarget = &mlabels[reg];
					programCounter = *branchTarget - 1;
					cout<<"Jumping to Line "<<programCounter+2<<endl;
				}
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}

	char cOpCode[10];
	memset(&cOpCode,'\0',10);
	sOpCode.copy(cOpCode,sOpCode.length(),0);
	lCycleCount	+= opcode_cycles[cOpCode];
	cout<<opcode_cycles[cOpCode]<<" Cycle\n";

	return E_SUCCESS;
}

bool executeBLTZ(string& sOpCode,string& sLine)
{
	bool isDouble = 0;
	int i,icount=0;
	char reg[20];
	int *is1,*is2;
	double *ds1,*ds2;
	long *branchTarget;

	if(sOpCode.find(".D") < sOpCode.length())
		isDouble = 1;
	while(icount < 2)
	{
		if((i = sLine.find(",")) > 0)
		{
			sLine.copy(reg,i,0);
			reg[i] = '\0';
		}
		else
		{
			sLine.copy(reg,sLine.length(),0);
			reg[sLine.length()] = '\0';
		}
		if(icount < 1 && !validateRegister(isDouble,reg))
		{
			cout<<"Invalid register. "<<reg<<" Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the target register
		{
			if(isDouble)
				ds1 = &float_registers[reg];
			else
				is1 = &integer_registers[reg];
		}
		else if(icount == 1)	// store the source 2 register
		{
				if((isDouble && (*ds1) < 0) ||
					(!isDouble && (*is1) < 0))
				{
					if(mlabels.count(reg) != 1)
					{
						cout<<"Invalid Label. Program Line No."<<programCounter<<endl;
						return E_FAIL;
					}
					branchTarget = &mlabels[reg];
					programCounter = *branchTarget - 1;
					cout<<"Jumping to Line "<<programCounter+2<<endl;
				}
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}

	char cOpCode[10];
	memset(&cOpCode,'\0',10);
	sOpCode.copy(cOpCode,sOpCode.length(),0);
	lCycleCount	+= opcode_cycles[cOpCode];
	cout<<opcode_cycles[cOpCode]<<" Cycle\n";

	return E_SUCCESS;
}
bool executeMOVE(string& sOpCode,string& sLine)
{
	bool isDouble = 0;
	int i,icount=0;
	char reg[4];
	int *is1,*is2, *itarget;
	double *ds1,*ds2,*dtarget;

	if(sOpCode.find(".D") < sOpCode.length())
		isDouble = 1;
	while(icount < 2)
	{
		if((i = sLine.find(",")) > 0)
		{
			sLine.copy(reg,i,0);
			reg[i] = '\0';
		}
		else
		{
			sLine.copy(reg,sLine.length(),0);
			reg[sLine.length()] = '\0';
		}
		if(icount == 1 && (!validateRegister(isDouble?0:1,reg)) ) 
		{
			cout<<"Invalid register. "<<reg<<" Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		else if(icount == 0 && (!validateRegister(isDouble,reg)) )
		{
			cout<<"Invalid register. "<<reg<<" Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 1)	// store the target register
		{
			if(isDouble)
				itarget = &integer_registers[reg];
			else
				dtarget = &float_registers[reg];
		}
		else if(icount == 0)	// store the source 1 register
		{
			if(isDouble)
				ds1 = &float_registers[reg];
			else
				is1 = &integer_registers[reg];
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}
	if(isDouble)
	{
		// move floating point register into an integer register
		*itarget = *ds1;
		cout<<*ds1<<" -> "<<*itarget<<endl;
	}
	else
	{	// move an integer register into a floating point register
		*dtarget = *is1;
		cout<<*is1<<" -> "<<*dtarget<<endl;
	}

	char cOpCode[10];
	memset(&cOpCode,'\0',10);
	sOpCode.copy(cOpCode,sOpCode.length(),0);
	lCycleCount	+= opcode_cycles[cOpCode];
	cout<<opcode_cycles[cOpCode]<<" Cycle\n";

	return E_SUCCESS;
}

bool validateRegister(bool isDouble,char* reg)
{
	bool bRetVal = E_SUCCESS;
	if((isDouble && float_registers.count(reg) == 0) ||
		(!isDouble && integer_registers.count(reg) == 0))
		bRetVal = E_FAIL;
//	cout<<"Inside validateRegister - "<<reg<<" Found = "<<bRetVal<<" isDouble = "<<isDouble<<endl;

	return bRetVal;
}

bool copyMemoryFile(char *initMemFile)
{
	ifstream in;
	long count = 0;
	char cdata;
	bool bRetVal = E_SUCCESS;
	// Allocate the memory
	mem = new char[1024*1024];

	in.open(initMemFile,ios::in);
	if(!in)
	{
		cout<<"Error Opening File "<<initMemFile<<endl;
		return E_FAIL;
	}
	while(!in.eof() && count <= (1024*1024))
	{	
		in.read(&cdata,sizeof(char)); 
		memcpy(&mem[count],&cdata,sizeof(char));
		count++;
	}
	cout<<count<<" bytes read"<<endl;
	in.close();
	return bRetVal;	
}

bool dumpMemoryFile(char *outputFile)
{
	ofstream out;
	long count = 0;
	char cdata;
	bool bRetVal = E_SUCCESS;

	out.open(outputFile,ios::out);
	if(!out)
	{
		cout<<"Error Opening File "<<outputFile<<endl;
		return E_FAIL;
	}
	while(count<(1024*1024))
	{	
		memcpy(&cdata,&mem[count],sizeof(char));
		out.write(&cdata,sizeof(char));
		count++;
	}
	cout<<"\nMemory Dump created successfully in \""<<outputFile<<"\""<<endl;
	out.close();
	return bRetVal;	
}
