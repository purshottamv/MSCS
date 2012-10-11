/* ============================================================================
 Name        : sim_p.c
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : Program simulating Instruction Level Parallelism (5 Stage Pipeline)
 ============================================================================
*/
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <iomanip>

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
string IR,sOpCode,IRBuffered;
char cTempA[20], cTempB[20], cImmediate[20];
char cTempABuffered[20], cTempBBuffered[20], cImmediateBuffered[20];

char *mem;
long lCycleCount, lStallCount, lInstructionCount;
int programCounter=0;

void ltrim(string&);
void rtrim(string&);
bool validateRegister(bool, char *);
bool bStallIF = false,bFreeze = false, bPrintWB = true;
bool endProgram;
struct DECODE_EXECUTE
{
	void *target;
	string targetRegister;
	bool isDouble;
	string sOpCode;
	string sInstruction;
};
DECODE_EXECUTE decodeExecute,decodeExecuteBuffered;

struct EXECUTE_MEM
{
	void *target;
	string targetRegister;
	bool isDouble;
	string sOpCode;
	string sInstruction;
	char ALUOutput[20];
};
EXECUTE_MEM executeMem,executeMemBuffered;

struct MEM_WB
{
	void *target;
	string targetRegister;
	bool isDouble;
	string sOpCode;
	string sInstruction;
	char output[20];
};
MEM_WB MemWB,MemWBBuffered;

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
		cout<<"Error : Parameters missing : sim_p <arch_params_file> <init_mem_file> <assembly_file> <output_file>\n";
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
	cout<<"\nTotal Stalls = "<<lStallCount<<endl;
	cout<<"\nTotal Instructions = "<<lInstructionCount<<endl;
	cout<<"\nCPI = "<<(double)lCycleCount/lInstructionCount<<endl;
	if(!dumpMemoryFile(outputFile))
		exit(1);

	return 1;
}

void initializeTempRegisters()
{
	memset(cTempA,'\0',20);
	memset(cTempB,'\0',20);
	memset(cImmediate,'\0',20);
	decodeExecute.targetRegister = "";
}

void executeProgram()
{
	lCycleCount = 0;
	lStallCount = 0;
	lInstructionCount = 0;
	endProgram = false;
	int i,icurrentLine=0;
	bool bRetVal = E_SUCCESS;
	char c = 'A';
	ifstream in;
	string sLine, slabel, sOpCode;
	char cOpCode[10],cCycles[4];
	bool instructionFetch();
	bool instructionDecode();
	bool instructionExecute();
	bool memoryAccessCycle();
	bool WriteBackCycle();
	void displayRegsiterFileStatus();

	IRBuffered = "";
	decodeExecuteBuffered.sOpCode = "";
	executeMemBuffered.sOpCode = "";
	MemWBBuffered.sOpCode = "";
	cout<<"Address of F3 = "<<&float_registers["F3"]<<endl;
	while(!endProgram)
	{
		cout<<"\nCycle "<<++lCycleCount<<endl;
		cout<<"---------"<<endl;
		if(!instructionFetch())
			exit(1);
		if(IRBuffered.length() > 0)
		{
			if(!instructionDecode())
				exit(1);
		}
		else
		{
			cout<<"Stage ID  : "<<endl;
			decodeExecute.sOpCode = "";
		}
		if(decodeExecuteBuffered.sOpCode.length() > 0)
		{
			if(!instructionExecute())
				exit(1);
		}
		else
		{
			cout<<"Stage EX  : "<<endl;
			executeMem.sOpCode = "";
		}
		if(executeMemBuffered.sOpCode.length() > 0)
		{
			if(!memoryAccessCycle())
				exit(1);
		}
		else
		{
			MemWB.sOpCode = "";
			cout<<"Stage MEM : "<<endl;
		}

		if(MemWBBuffered.sOpCode.length() > 0)
		{
			if(!WriteBackCycle())
				exit(1);
		}
		else
			cout<<"Stage WB  : "<<endl;

		if(!bStallIF && !bFreeze)
			IRBuffered = IR;
		else
			lStallCount++;
	
		if(!bFreeze)
		{
			memcpy(cTempABuffered,cTempA,20);
			memcpy(cTempBBuffered,cTempB,20);
			memcpy(cImmediateBuffered,cImmediate,20);
			decodeExecuteBuffered = decodeExecute;
		}
		executeMemBuffered = executeMem;
		MemWBBuffered = MemWB;

		if(IRBuffered.length() == 0 &&
			decodeExecuteBuffered.sOpCode.length() == 0 &&
			executeMemBuffered.sOpCode.length() == 0 &&
			MemWBBuffered.sOpCode.length() == 0)
		{
			endProgram = true;
		}

		// Initialize all intermediate registers
		if(!bFreeze)
		{
			memset(cTempA,'\0',20);
			memset(cTempB,'\0',20);
			memset(cImmediate,'\0',20);
			decodeExecute.sOpCode = "";
			decodeExecute.sInstruction = "";
			decodeExecute.isDouble = 0;
			decodeExecute.target = NULL;
			decodeExecute.targetRegister = "";
		}
		executeMem.sOpCode = "";
		executeMem.sInstruction = "";
		executeMem.isDouble = 0;
		executeMem.target = NULL;
		executeMem.targetRegister = "";
		memset(&executeMem.ALUOutput,'\0',20);
		
		MemWB.sOpCode = "";
		MemWB.isDouble = 0;
		MemWB.targetRegister = "";
		MemWB.sInstruction = "";
		MemWB.target = NULL;
		memset(&MemWB.output,'\0',20);
//		displayRegsiterFileStatus();
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
	cout<<"Initialize registers R0 = "<<integer_registers["R0"]<<endl;
	memset(cTempA,'\0',20);
	memset(cTempB,'\0',20);
	memset(cImmediate,'\0',20);
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
	if(iRetVal == E_FAIL)
	{	
		cout<<"File Format Error: "<<archParamFile<<"\nMake sure Opcode and Number of Cycles are separated by at least one space"<<endl;
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
	string sLine, slabel, sLine2;
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
		memset(cCommand,'\0',MAXCHARS);
		in.getline(cCommand,MAXCHARS-1);
		if(in.eof())
			sLine.assign(cCommand,strlen(cCommand));
		else
			sLine.assign(cCommand,strlen(cCommand)-1);
	// Step 1 : Trim down the comment part first
		if((i = sLine.find(";")) >=0)
			sLine.assign(sLine,0,i);
		sLine2 = sLine;
	// Step 2 : Check for the label. If found mark it and trim it
		if((i = sLine.find(":")) >=0)
		{	
			slabel.assign(sLine,0,i);
			sLine.assign(sLine,i+1,sLine.length()-i);
			mlabels.insert(make_pair(slabel,iLineNumber));
		}
		ltrim(sLine);
		rtrim(sLine);
	// Step 3 : Check for Opcode
		if(sLine.length() > 0)
		{
			ltrim(sLine);
			rtrim(sLine);
		}
		else
		{
				continue;
		}

	// Step 4 : Save the program in the vector
		vprogram.push_back(sLine);
		cout<<iLineNumber<<". "<<sLine2<<endl;
		iLineNumber++;
		slabel= "";
	}
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
	int i = sLine.length();
	while(i > 0 && sLine.compare(i-1,1," ") == 0)
		i--;
	sLine.assign(sLine,0,i);
}

bool decodeNOOP(string& sOpCode,string& sLine)
{
	bool bRetVal = E_SUCCESS;
	return bRetVal;
}

bool decodeArithmatic(string& sOpCode,string& sLine)
{
	int i,icount=0;
	char reg[4];
	int *is1,*is2, *itarget;
	double *ds1,*ds2,*dtarget;
	bool detecthazard(char *);
	memset(reg,'\0',4);
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
		if(!validateRegister(decodeExecute.isDouble,reg))
		{
			cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the target register
		{
			decodeExecute.targetRegister.assign(reg);
			if(decodeExecute.isDouble)
				decodeExecute.target = &float_registers[reg];
			else
				decodeExecute.target = &integer_registers[reg];
		}
		else if(icount == 1)	// store the source 1 register
		{
			if(detecthazard(reg))
				return E_SUCCESS;

			if(decodeExecute.isDouble)
			{
				ds1 = &float_registers[reg];
				sprintf(cTempA,"%g",*ds1);
			}
			else
			{
				is1 = &integer_registers[reg];
				sprintf(cTempA,"%d",*is1);
			}
		}
		else if(icount == 2)	// store the source 2 register
		{

			if(detecthazard(reg))
				return E_SUCCESS;
			
			if(decodeExecute.isDouble)
			{
				ds2 = &float_registers[reg];
				sprintf(cTempB,"%g",*ds2);
			}
			else
			{
				is2 = &integer_registers[reg];
				sprintf(cTempB,"%d",*is2);
			}
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}
	return E_SUCCESS;
}

bool decodeLoadStore(string& sOpCode,string& sLine)
{
	// L   t.I, o(s.I)  //Integer Load
	// L.D t.D, o(s.I)  //Floating Point Load
	// S   t.I, o(s2.I) //Integer store
	// S.D t.D, o(s2.I) //Floating point store
	bool detecthazard(char *);
	bool bRetVal = E_SUCCESS;
	int i,i2;
	char reg[4],offset[10];
	int *is, *itarget;
	double *dtarget;

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
	if(!validateRegister(decodeExecute.isDouble,reg))
	{
		cout<<"Invalid register. "<<reg<<". Program Line No. "<<programCounter+1<<endl;
		return E_FAIL;
	}
	sLine.assign(sLine,i+1,sLine.length()-i-1);
	
	if(sOpCode.compare(0,1,"L") == 0)
		decodeExecute.targetRegister.assign(reg);  // Store the target register only for Load

	if(decodeExecute.isDouble)
		decodeExecute.target = &float_registers[reg];
	else
		decodeExecute.target = &integer_registers[reg];

	if((sOpCode.compare(0,1,"S") == 0) && detecthazard(reg))
		return E_SUCCESS;

	if((i = sLine.find("(")) > 0)
	{
		sLine.copy(cTempA,i,0);
		if((i2 = sLine.find(")",i)) > 0)
		{
			memset(reg,'\0',4);
			sLine.copy(reg,i2-i-1,i+1);
			reg[i2-i] = '\0';
			if(!validateRegister(0,reg)) // Overriding the first argument
			{
				cout<<"Invalid register. "<<reg<<". Program Line No. "<<programCounter+1<<endl;
				return E_FAIL;
			}
			if(detecthazard(reg))
				return E_SUCCESS;
			sprintf(cTempB,"%d",integer_registers[reg]);
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
	return bRetVal;
}

bool decodeLI(string& sOpCode,string& sLine)
{
	// LI t.I, o // Integer load immediate

	bool isDouble = 0;
	int i;
	char reg[4],immediate[20];
	int *itarget;

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
	if(!validateRegister(decodeExecute.isDouble,reg))
	{
		cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
		return E_FAIL;
	}
	sLine.copy(cImmediate,sLine.length()-i,i+1);
	decodeExecute.targetRegister.assign(reg);
	decodeExecute.target = &integer_registers[reg];

	return E_SUCCESS;
}

bool decodeBEQ(string& sOpCode,string& sLine)
{
	int i,icount=0;
	char reg[20];
	int *is1,*is2;
	double *ds1,*ds2;
	long *branchTarget;
	bool detecthazard(char *);

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
		if(icount < 2 && !validateRegister(decodeExecute.isDouble,reg))
		{
			cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the target register
		{
			if(detecthazard(reg))
				return E_SUCCESS;

			if(decodeExecute.isDouble)
				ds1 = &float_registers[reg];
			else
				is1 = &integer_registers[reg];
		}
		else if(icount == 1)	// store the source 1 register
		{
			if(decodeExecute.isDouble)
				ds2 = &float_registers[reg];
			else
				is2 = &integer_registers[reg];
		}
		else if(icount == 2)	// store the source 2 register
		{
				if((decodeExecute.isDouble && (*ds1) == (*ds2)) ||
					(!decodeExecute.isDouble && (*is1) == (*is2)))
				{
					if(mlabels.count(reg) != 1)
					{
						cout<<"Invalid Label. Program Line No."<<programCounter<<endl;
						return E_FAIL;
					}
					branchTarget = &mlabels[reg];
					if(*branchTarget != programCounter)	// flush the pipeline if branched 
					{
						IR = "";
						lStallCount++;
					}
					programCounter = *branchTarget;
				}
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}

	return E_SUCCESS;
}

bool decodeBNEQ(string& sOpCode,string& sLine)
{
	int i,icount=0;
	char reg[20];
	int *is1,*is2;
	double *ds1,*ds2;
	long *branchTarget;
	bool detecthazard(char *);

	while(icount < 3)
	{
		memset(reg,'\0',20);
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
		if(icount < 2 && !validateRegister(decodeExecute.isDouble,reg))
		{
			cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the source 1 register
		{
			if(detecthazard(reg))
				return E_SUCCESS;

			if(decodeExecute.isDouble)
				ds1 = &float_registers[reg];
			else
				is1 = &integer_registers[reg];
		}
		else if(icount == 1)	// store the source 2 register
		{
			if(decodeExecute.isDouble)
				ds2 = &float_registers[reg];
			else
				is2 = &integer_registers[reg];
		}
		else if(icount == 2)	// store the source 2 register
		{
				// Before comparison check for any dependency. 
				// Save the name of the target registers in each stage for the relevant instructions.
				if((decodeExecute.isDouble && (*ds1) != (*ds2)) ||
					(!decodeExecute.isDouble && (*is1) != (*is2)))
				{
					if(mlabels.count(reg) != 1)
					{
						cout<<"Invalid Label. Program Line No."<<programCounter<<endl;
						return E_FAIL;
					}
					branchTarget = &mlabels[reg];
					if(*branchTarget != programCounter)	// flush the pipeline if branched 
					{
						IR = "";
						lStallCount++;
					}
					programCounter = *branchTarget;
				}
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}

	return E_SUCCESS;
}

bool decodeBGTZ(string& sOpCode,string& sLine)
{

	int i,icount=0;
	char reg[20];
	int *is1,*is2;
	double *ds1,*ds2;
	long *branchTarget;
	bool detecthazard(char *);

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
		if(icount < 1 && !validateRegister(decodeExecute.isDouble,reg))
		{
			cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the target register
		{
			if(detecthazard(reg))
				return E_SUCCESS;

			if(decodeExecute.isDouble)
				ds1 = &float_registers[reg];
			else
				is1 = &integer_registers[reg];
		}
		else if(icount == 1)	// store the source 2 register
		{
				if((decodeExecute.isDouble && (*ds1) > 0) ||
					(!decodeExecute.isDouble && (*is1) > 0))
				{
					if(mlabels.count(reg) != 1)
					{
						cout<<"Invalid Label. Program Line No."<<programCounter<<endl;
						return E_FAIL;
					}
					branchTarget = &mlabels[reg];
					if(*branchTarget != programCounter)	// flush the pipeline if branched 
					{
						IR = "";
						lStallCount++;
					}
					programCounter = *branchTarget;
				}
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}

	return E_SUCCESS;
}

bool decodeBLTZ(string& sOpCode,string& sLine)
{
	int i,icount=0;
	char reg[20];
	int *is1,*is2;
	double *ds1,*ds2;
	long *branchTarget;
	bool detecthazard(char *);

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
		if(icount < 1 && !validateRegister(decodeExecute.isDouble,reg))
		{
			cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the target register
		{
			if(detecthazard(reg))
				return E_SUCCESS;

			if(decodeExecute.isDouble)
				ds1 = &float_registers[reg];
			else
				is1 = &integer_registers[reg];
		}
		else if(icount == 1)	// store the source 2 register
		{
				if((decodeExecute.isDouble && (*ds1) < 0) ||
					(!decodeExecute.isDouble && (*is1) < 0))
				{
					if(mlabels.count(reg) != 1)
					{
						cout<<"Invalid Label. Program Line No."<<programCounter<<endl;
						return E_FAIL;
					}
					branchTarget = &mlabels[reg];
					if(*branchTarget != programCounter)	// flush the pipeline if branched 
					{
						IR = "";
						lStallCount++;
					}
					programCounter = *branchTarget;
				}
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}

	return E_SUCCESS;
}

bool decodeMOVE(string& sOpCode,string& sLine)
{
	int i,icount=0;
	char reg[4];
	bool detecthazard(char *);

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
		if(icount == 1 && (!validateRegister(decodeExecute.isDouble?0:1,reg)) ) 
		{
			cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		else if(icount == 0 && (!validateRegister(decodeExecute.isDouble,reg)) )
		{
			cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 1)	// store the target register
		{
			decodeExecute.targetRegister.assign(reg);
			if(decodeExecute.isDouble)
			{
				decodeExecute.target = &integer_registers[reg];
				cout<<"storing the target Integer register : "<<reg<<" Target Address : "<<decodeExecute.target<<endl;
			}
			else
			{
				decodeExecute.target = &float_registers[reg];
				cout<<"storing the target Float register : "<<reg<<" Target Address : "<<decodeExecute.target<<endl;
			}
		}
		else if(icount == 0)	// store the source 1 register
		{
			if(detecthazard(reg))
				return E_SUCCESS;

			if(decodeExecute.isDouble)
				sprintf(cTempA,"%f",float_registers[reg]);
			else
				sprintf(cTempA,"%d",integer_registers[reg]);
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}
	return E_SUCCESS;
}

bool validateRegister(bool isDouble,char* reg)
{
	bool bRetVal = E_SUCCESS;
	if((isDouble && float_registers.count(reg) == 0) ||
		(!isDouble && integer_registers.count(reg) == 0))
		bRetVal = E_FAIL;

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

bool instructionFetch()
{
	// Fetch the current instruction
	// Update the PC to next available PC
	bool bRetVal = E_SUCCESS;
	
	if(bStallIF == 0 && programCounter < vprogram.size() && !bFreeze)
	{
		IR = vprogram[programCounter++];
		lInstructionCount++;
	}
	else if(programCounter == vprogram.size())
	{
		if(bStallIF == 1 || bFreeze)
			IR = vprogram[programCounter-1];
		else
		{
			IR = "";
			programCounter++;
		}
	}

	if(IR.length() > 0)
		cout<<"Stage IF  : "<<IR<<"\tPC = "<<programCounter-1<<" "<<endl;
	else
		cout<<"Stage IF  : "<<endl;
	return bRetVal;
}

bool instructionDecode()
{

// 1. Decode the instruction
// 2. Access the register file to read the registers (Rs and Rt) into two temporary registers A and B
// 3. Load the Immediate field in the Immediate register
// 4. Do the equality test on the registers as they are read, for a possible branch. 
// 5. If the condition yielded true, compute the branch target address and store it in PC.

	bool decodeNOOP(string& sOpCode,string& sLine);
	bool decodeADD(string& sOpCode,string& sLine);
	bool decodeSUB(string& sOpCode,string& sLine);
	bool decodeMUL(string& sOpCode,string& sLine);
	bool decodeDIV(string& sOpCode,string& sLine);
	bool decodeLoadStore(string& sOpCode,string& sLine);
	bool decodeLI(string& sOpCode,string& sLine);
	bool decodeS(string& sOpCode,string& sLine);
	bool decodeBEQ(string& sOpCode,string& sLine);
	bool decodeBNEQ(string& sOpCode,string& sLine);
	bool decodeBGTZ(string& sOpCode,string& sLine);
	bool decodeBLTZ(string& sOpCode,string& sLine);
	bool decodeMOVE(string& sOpCode,string& sLine);

	bool bRetVal = E_SUCCESS;
	int i;
	string sLine;
	char cOpCode[10];

	void initializeTempRegisters();
	rtrim(IRBuffered);

	if(!(IRBuffered.length() > 0 && (i = IRBuffered.find(" ")) > 0))
	{
		if(IRBuffered.compare(0,4,"NOOP") == 0)
			i = 4;
		else
		{
			cout<<"Error: No Opcode\n";
			return E_FAIL;
		}
	}
	IRBuffered.copy(cOpCode,i,0);
	cOpCode[i] = '\0';
	sOpCode.assign(IRBuffered,0,i);
	if(sOpCode != "NOOP")
	{
		sLine.assign(IRBuffered,i,IRBuffered.length()-i);
		ltrim(sLine);
	}
	decodeExecute.sInstruction = IRBuffered;
	decodeExecute.sOpCode = sOpCode;
	if(sOpCode.find(".D") < sOpCode.length())
		decodeExecute.isDouble = 1;
	else
		decodeExecute.isDouble = 0;

	if(sOpCode.compare("NOOP") == 0)
		bRetVal = decodeNOOP(sOpCode,sLine);
	else if((sOpCode.compare(0,3,"ADD") == 0) || (sOpCode.compare(0,3,"SUB") == 0) || (sOpCode.compare(0,3,"MUL") == 0) || (sOpCode.compare(0,3,"DIV") == 0))
		bRetVal = decodeArithmatic(sOpCode,sLine);
	else if(sOpCode.compare(0,2,"LI") == 0)
		bRetVal = decodeLI(sOpCode,sLine);
	else if((sOpCode.compare(0,1,"L") == 0) || (sOpCode.compare(0,1,"S") == 0))
		bRetVal = decodeLoadStore(sOpCode,sLine);
	else if(sOpCode.compare(0,3,"BEQ") == 0)
		bRetVal = decodeBEQ(sOpCode,sLine);
	else if(sOpCode.compare(0,4,"BNEQ") == 0)
		bRetVal = decodeBNEQ(sOpCode,sLine);
	else if(sOpCode.compare(0,4,"BGTZ") == 0)
		bRetVal = decodeBGTZ(sOpCode,sLine);
	else if(sOpCode.compare(0,4,"BLTZ") == 0)
		bRetVal = decodeBLTZ(sOpCode,sLine);
	else if(sOpCode.compare(0,4,"MOVE") == 0)
		bRetVal = decodeMOVE(sOpCode,sLine);
	else
	{
		cout<<"Invalid OpCode. Line No. "<<programCounter+1<<endl;
		bRetVal = E_FAIL;
	}
	cout<<"Stage ID  : "<<IRBuffered<<" \tA = "<<cTempA<<" B = "<<cTempB<<endl;
	return bRetVal;
}

bool instructionExecute()
{
	static int iexecuteCycle = 1;
	string sOpCode = decodeExecuteBuffered.sOpCode;
	bool isDouble = decodeExecuteBuffered.isDouble;
	bool iRetVal = E_SUCCESS;
	// Copy from Decode-Execute to Execute-Mem
	char cOpCode[10];
	memset(cOpCode,'\0',10);
	sOpCode.copy(cOpCode,sOpCode.length(),0);	

	if(opcode_cycles[cOpCode] > 1 && bFreeze == false)
	{
		iexecuteCycle = opcode_cycles[cOpCode];
		bFreeze = true;
	}
	if(bFreeze)
	{
		iexecuteCycle--;
		if(iexecuteCycle == 0)
			bFreeze = false;
		else
		{
			cout<<"Stage EX  : "<<decodeExecuteBuffered.sInstruction<<"\tCycles Remaining = "<<iexecuteCycle<<endl;
			return iRetVal;
		}
	}
	
	executeMem.sOpCode = decodeExecuteBuffered.sOpCode;
	executeMem.isDouble = decodeExecuteBuffered.isDouble;
	executeMem.target = decodeExecuteBuffered.target;
	executeMem.targetRegister = decodeExecuteBuffered.targetRegister;
	executeMem.sInstruction = decodeExecuteBuffered.sInstruction;

	memset(executeMem.ALUOutput,'\0',20);
	if(sOpCode.compare("NOOP") == 0)
	{
		
	}
	else if(sOpCode.compare(0,3,"ADD") == 0)
	{
		if(isDouble)
			sprintf(executeMem.ALUOutput,"%f",atof(cTempABuffered) + atof(cTempBBuffered));
		else
			sprintf(executeMem.ALUOutput,"%d",atoi(cTempABuffered) + atoi(cTempBBuffered));
	}
	else if(sOpCode.compare(0,3,"SUB") == 0)
	{
		if(isDouble)
			sprintf(executeMem.ALUOutput,"%f",atof(cTempABuffered) - atof(cTempBBuffered));
		else
			sprintf(executeMem.ALUOutput,"%d",atoi(cTempABuffered) - atoi(cTempBBuffered));	
	}
	else if(sOpCode.compare(0,3,"MUL") == 0)
	{
		if(isDouble)
			sprintf(executeMem.ALUOutput,"%f",atof(cTempABuffered) * atof(cTempBBuffered));
		else
			sprintf(executeMem.ALUOutput,"%d",atoi(cTempABuffered) * atoi(cTempBBuffered));	
	}
	else if(sOpCode.compare(0,3,"DIV") == 0)
	{
		if(isDouble)
			sprintf(executeMem.ALUOutput,"%f",atof(cTempABuffered) / atof(cTempBBuffered));
		else
			sprintf(executeMem.ALUOutput,"%d",atoi(cTempABuffered) / atoi(cTempBBuffered));	
	}
	else if(sOpCode.compare(0,2,"LI") == 0)
	{
		sprintf(executeMem.ALUOutput,"%d",atoi(cImmediateBuffered));	
		// Do Nothing		
	}
	else if((sOpCode.compare(0,1,"L") == 0) || (sOpCode.compare(0,1,"S") == 0))
	{
		sprintf(executeMem.ALUOutput,"%d",atoi(cTempABuffered)+atoi(cTempBBuffered));
	}
	else if(sOpCode.compare(0,4,"MOVE") == 0)
	{
		if(isDouble)
			sprintf(executeMem.ALUOutput,"%f",atof(cTempABuffered));
		else
			sprintf(executeMem.ALUOutput,"%d",atoi(cTempABuffered));	
	}
	cout<<"Stage EX  : "<<executeMem.sInstruction<<" \tALUOutput "<<executeMem.ALUOutput<<endl;

	return iRetVal;
}

bool memoryAccessCycle()
{
	bool iRetVal = E_SUCCESS;
	// Copy from Execute-Mem to MemWB
	MemWB.sOpCode = executeMemBuffered.sOpCode;
	MemWB.isDouble = executeMemBuffered.isDouble;
	MemWB.target = executeMemBuffered.target;
	MemWB.targetRegister = executeMemBuffered.targetRegister;
	MemWB.sInstruction = executeMemBuffered.sInstruction;
	memcpy(&MemWB.output,&executeMemBuffered.ALUOutput,20);

	if(executeMemBuffered.sOpCode.compare(0,2,"LI") == 0)
	{
		// Do Nothing
		cout<<"Stage MEM : "<<executeMemBuffered.sInstruction<<endl;
	}
	else if((executeMemBuffered.sOpCode.compare(0,1,"L") == 0))
	{
		if(MemWB.isDouble)
		{
			memcpy(&MemWB.output,&mem[long(atoi(executeMemBuffered.ALUOutput))],8);
		}
		else
		{
			memcpy(&MemWB.output,&mem[long(atoi(executeMemBuffered.ALUOutput))],4);
		}
		cout<<"Stage MEM : "<<executeMemBuffered.sInstruction<<"\t"<<MemWB.output<<" Loaded from address "<<executeMemBuffered.ALUOutput<<endl;
	}
	else if(executeMemBuffered.sOpCode.compare(0,1,"S") == 0)
	{
		if(MemWB.isDouble)
		{
			double *temp = (double *)executeMemBuffered.target;
			sprintf(&mem[long(atoi(executeMemBuffered.ALUOutput))],"%g",*temp);
			//mem[long(atoi(executeMemBuffered.ALUOutput))] = *temp;
			cout<<"Stage MEM : "<<executeMemBuffered.sInstruction<<"\t"<<*temp<<" stored at address "<<executeMemBuffered.ALUOutput<<endl;
		}
		else
		{
			int *temp = (int *)executeMemBuffered.target;
			sprintf(&mem[long(atoi(executeMemBuffered.ALUOutput))],"%d",*temp);
			//mem[long(atoi(executeMemBuffered.ALUOutput))] = *temp;
			cout<<"Stage MEM : "<<executeMemBuffered.sInstruction<<"\t"<<*temp<<" stored at address "<<executeMemBuffered.ALUOutput<<endl;
		}	

	}
	else
		cout<<"Stage MEM : "<<executeMemBuffered.sInstruction<<endl;
	return iRetVal;
}

bool WriteBackCycle()
{
	bool iRetVal = E_SUCCESS;
	
	if( (MemWBBuffered.sOpCode.compare(0,3,"ADD") == 0) || 
		(MemWBBuffered.sOpCode.compare(0,3,"SUB") == 0) || 
		(MemWBBuffered.sOpCode.compare(0,3,"MUL") == 0) || 
		(MemWBBuffered.sOpCode.compare(0,3,"DIV") == 0) ||
		(MemWBBuffered.sOpCode.compare(0,2,"LI") == 0)  ||
	    (MemWBBuffered.sOpCode.compare(0,1,"L") == 0) )
	{
		if(MemWBBuffered.isDouble)
		{
			double *temp = (double *)MemWBBuffered.target;
			if(strlen(MemWBBuffered.output) > 0)
				*temp = strtod(MemWBBuffered.output,NULL);
			if(bPrintWB)
				cout<<"Stage WB  : "<<MemWBBuffered.sInstruction<<"\t"<<*temp<<" written to destination "<<MemWBBuffered.targetRegister<<endl;
		}
		else
		{
			int *temp = (int *)MemWBBuffered.target;
			if(strlen(MemWBBuffered.output) > 0)
				*temp = atoi(MemWBBuffered.output);
			if(bPrintWB)
				cout<<"Stage WB  : "<<MemWBBuffered.sInstruction<<"\t"<<*temp<<" written to destination "<<MemWBBuffered.targetRegister<<endl;
		}
	}
	else if(MemWBBuffered.sOpCode.compare(0,4,"MOVE") == 0)
	{
		if(!MemWBBuffered.isDouble)
		{
			double *temp = (double *)MemWBBuffered.target;
			if(strlen(MemWBBuffered.output) > 0)
				*temp = strtod(MemWBBuffered.output,NULL);
			if(bPrintWB)
				cout<<"Stage WB  : "<<MemWBBuffered.sInstruction<<"\t"<<*temp<<" written to destination "<<MemWBBuffered.targetRegister<<endl;
			if(MemWBBuffered.sOpCode.compare(0,4,"MOVE") == 0)
				cout<<" isDouble Storing in Target Address: "<<temp<<endl;
		}
		else
		{
			int *temp = (int *)MemWBBuffered.target;
			if(strlen(MemWBBuffered.output) > 0)
				*temp = atoi(MemWBBuffered.output);
			if(bPrintWB)
				cout<<"Stage WB  : "<<MemWBBuffered.sInstruction<<"\t"<<*temp<<" written to destination "<<MemWBBuffered.targetRegister<<endl;
			if(MemWBBuffered.sOpCode.compare(0,4,"MOVE") == 0)
				cout<<" Integer Storing in Target Address: "<<temp<<endl;
		}	
	}
	else
	{
		if(bPrintWB)
			cout<<"Stage WB  : "<<MemWBBuffered.sInstruction<<endl;
	}
	return iRetVal;
}

bool detecthazard(char *r)
{
	bool iRetVal = false;

	if( decodeExecuteBuffered.targetRegister.find(r) < 3 ||
	executeMemBuffered.targetRegister.find(r) < 3)
	{
		bStallIF = 1;

		decodeExecute.sOpCode = "";
		decodeExecute.isDouble = 0;
		decodeExecute.target = NULL;
		decodeExecute.targetRegister = "";
		cout<<"Hazard Detected : "<<r<<endl;
		iRetVal = true;
	}
	else
	{
		bStallIF = 0;
		iRetVal = false;
	}

	if(	MemWBBuffered.targetRegister.find(r) < 3)
	{
		bPrintWB = false;
		WriteBackCycle();
		bPrintWB = true;
	}
	
	return iRetVal;
}
void displayRegsiterFileStatus()
{
	int i=0;
	char reg[10];
	memset(reg,'\0',10);
	cout<<"\n\nInteger Register File System"<<endl;
	cout<<"----------------------------"<<endl;
	for(i=0;i<=15;i++)
	{
		sprintf(reg,"R%d",i);
		cout<<setw(10)<<left<<reg;
	}
	cout<<endl;
	for(i=0;i<=15;i++)
	{
		sprintf(reg,"R%d",i);
		cout<<setw(10)<<left<<integer_registers[reg];
	}
	memset(reg,'\0',10);
	cout<<"\nFloating Point Register File System"<<endl;
	cout<<"-----------------------------------"<<endl;
	for(i=0;i<=15;i++)
	{
		sprintf(reg,"F%d",i);
		cout<<setw(10)<<left<<reg;
	}
	cout<<endl;
	for(i=0;i<=15;i++)
	{
		sprintf(reg,"F%d",i);
		cout<<setw(10)<<left<<float_registers[reg];
	}
	cout<<endl;
}