/* ============================================================================
 Name        : sim_t.cpp
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : Program simulating Tomasulo Algorithm for Dynamic Scheduling
 ============================================================================
*/
/**********************************************************************
1. Parse the arch_params_file and store all the Op-Codes into a map structure
2. Parse the assembly code file and execute it.
**********************************************************************/
#include "sim_t.h"

map<string,OPCODE> OpCodeMap;
map<OPCODE,int> opcode_cycles;
map<string,int> NoOfFunctionalUnits;
map<string,IntegerRegisterFS> integer_registers;
map<string,FloatRegisterFS> float_registers;
void ltrim(string&);
void rtrim(string&);
bool endProgram;
int programCounter;
map<string,long> mlabels;
bool triggerend;
vector<Instruction> vprogram;
bool stallDueToRSid[8];  // 8 is the number of types of reservation stations
WriteBack wbobject;      // Create the Write back object responsible for write back to registers
int FunctionalUnit::FUid = 1;

char *mem;
long lCycleCount, lStallCount, lInstructionCount;
bool bStallIF,bFreeze,bStallDueToBranch;
Instruction IR;

// Pointers to the Reservation Stations
vector<LSReservationStation> LoadRS;	// Vector of Load Reservation Stations
vector<LSReservationStation> StoreRS;	// Vector of Store Reservation Stations
vector<IntegerAdd> IAddRS;
vector<IntegerMul> IMulRS;
vector<IntegerDiv> IDivRS;
vector<FloatAdd> FAddRS;
vector<FloatMul> FMulRS;
vector<FloatDiv> FDivRS;

int main(int argc, char *argv[])
{
	int i;
	char *archParamFile, *initMemFile, *assemblyFile, *outputFile;
	int parseArchParams(char *);
	void initializeOPCODE();
	void initializeRegisters();
	void executeProgram();
	int parseAssemblyFile(char *);
	lCycleCount = programCounter = 0;
	bStallIF = bFreeze = bStallDueToBranch = false;
	bool copyMemoryFile(char *);
	bool dumpMemoryFile(char *);
	if(argc < 5)
	{
		cout<<"Error : Parameters missing : sim_t <arch_params_file> <init_mem_file> <assembly_file> <output_file>\n";
		exit(1);
	}

	archParamFile = argv[1];
	initMemFile = argv[2];	
	assemblyFile = argv[3];
	outputFile = argv[4];
	initializeOPCODE();
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
	if(lInstructionCount > 0)
		cout<<"\nCPI = "<<(double)lCycleCount/lInstructionCount<<endl;
	else
		cout<<"\nNo. of Instructions = 0\n";
	if(!dumpMemoryFile(outputFile))
		exit(1);

	return 1;
}

void executeProgram()
{
	bool Issue();
	bool execute();
	void displayRegsiterFileStatus();
	triggerend = false;
	for(int j =0 ; j < 8 ; j++)
		stallDueToRSid[j] = false;
//	while(!endProgram)
	while(triggerend == false)
	{
		triggerend = true;
		cout<<"\nCycle "<<++lCycleCount<<endl;
		cout<<"---------"<<endl;
		wbobject.run();						// The Write Back Stage

		if(!execute())						// The execute Stage
			exit(1);

		if(!bStallDueToBranch && !Issue())	// The issue Stage
			exit(1);
		
		displayRegsiterFileStatus();
//		sleep(1);
	}
}


void initializeOPCODE()
{
	OpCodeMap.insert(make_pair("NOOP",NOOP));
	OpCodeMap.insert(make_pair("ADD",ADD));
	OpCodeMap.insert(make_pair("ADD.D",ADD_D));
	OpCodeMap.insert(make_pair("DIV",DIV));
	OpCodeMap.insert(make_pair("DIV.D",DIV_D));
	OpCodeMap.insert(make_pair("SUB",SUB));
	OpCodeMap.insert(make_pair("SUB.D",SUB_D));
	OpCodeMap.insert(make_pair("MUL",MUL));
	OpCodeMap.insert(make_pair("MUL.D",MUL_D));
	OpCodeMap.insert(make_pair("LI",LOADIMMEDIATE));
	OpCodeMap.insert(make_pair("L",LOAD));
	OpCodeMap.insert(make_pair("S",STORE));
	OpCodeMap.insert(make_pair("L.D",LOAD_D));
	OpCodeMap.insert(make_pair("S.D",STORE_D));
	OpCodeMap.insert(make_pair("BEQ",BEQ));
	OpCodeMap.insert(make_pair("BNEQ",BNEQ));
	OpCodeMap.insert(make_pair("BGTZ",BGTZ));
	OpCodeMap.insert(make_pair("BLTZ",BLTZ));
	OpCodeMap.insert(make_pair("MOVE",MOVE));
	OpCodeMap.insert(make_pair("BEQ.D",BEQ_D));
	OpCodeMap.insert(make_pair("BNEQ.D",BNEQ_D));
	OpCodeMap.insert(make_pair("BGTZ.D",BGTZ_D));
	OpCodeMap.insert(make_pair("BLTZ.D",BLTZ_D));
	OpCodeMap.insert(make_pair("MOVE.D",MOVE_D));
}

void initializeRegisters()
{
	IntegerRegisterFS iReg;
	FloatRegisterFS fReg;
	integer_registers.insert(make_pair("R0",iReg));
	integer_registers.insert(make_pair("R1",iReg));
	integer_registers.insert(make_pair("R2",iReg));
	integer_registers.insert(make_pair("R3",iReg));
	integer_registers.insert(make_pair("R4",iReg));
	integer_registers.insert(make_pair("R5",iReg));
	integer_registers.insert(make_pair("R6",iReg));
	integer_registers.insert(make_pair("R7",iReg));
	integer_registers.insert(make_pair("R8",iReg));
	integer_registers.insert(make_pair("R9",iReg));
	integer_registers.insert(make_pair("R10",iReg));
	integer_registers.insert(make_pair("R11",iReg));
	integer_registers.insert(make_pair("R12",iReg));
	integer_registers.insert(make_pair("R13",iReg));
	integer_registers.insert(make_pair("R14",iReg));
	integer_registers.insert(make_pair("R15",iReg));

	float_registers.insert(make_pair("F0",fReg));
	float_registers.insert(make_pair("F1",fReg));
	float_registers.insert(make_pair("F2",fReg));
	float_registers.insert(make_pair("F3",fReg));
	float_registers.insert(make_pair("F4",fReg));
	float_registers.insert(make_pair("F5",fReg));
	float_registers.insert(make_pair("F6",fReg));
	float_registers.insert(make_pair("F7",fReg));
	float_registers.insert(make_pair("F8",fReg));
	float_registers.insert(make_pair("F9",fReg));
	float_registers.insert(make_pair("F10",fReg));
	float_registers.insert(make_pair("F11",fReg));
	float_registers.insert(make_pair("F12",fReg));
	float_registers.insert(make_pair("F13",fReg));
	float_registers.insert(make_pair("F14",fReg));
	float_registers.insert(make_pair("F15",fReg));
}

int parseArchParams(char *archParamFile)
{
	bool isOpCode = true;
	ifstream in;
	char cCommand[MAXCHARS];
	string sLine;
	char cOpCode[10],cCycles[4];
	int iCycles, iRetVal = E_SUCCESS;
	int i,j=0;
	OPCODE op;

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
			isOpCode = false;
			continue;
		}
		if((i=sLine.find(" ")) < 0)
		{
			iRetVal = E_FAIL;
			break;
		}
		sLine.copy(cOpCode,i,0);
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
		if(isOpCode == true)
		{
			if(OpCodeMap.count(cOpCode) != 1)
			{
				iRetVal = E_FAIL;
				break;
			}
			else
				op = OpCodeMap[cOpCode];
			opcode_cycles.insert(make_pair(op,iCycles));
		}
		else
		{
			NoOfFunctionalUnits.insert(make_pair(cOpCode,iCycles));
			if(strcmp(cOpCode,"loads") == 0)
			{
				for(j=0;j<iCycles;j++)
				{
					LSReservationStation obj(LD);
					LoadRS.push_back(obj);
				}
			}
			else if(strcmp(cOpCode,"stores") == 0)
			{
				for(j=0;j<iCycles;j++)
				{
					LSReservationStation obj(ST);
					StoreRS.push_back(obj);
				}
			}
			else if(strcmp(cOpCode,"intadds") == 0)
			{
				for(j=0;j<iCycles;j++)
				{
					IntegerAdd obj;
					IAddRS.push_back(obj);
				}
			}
			else if(strcmp(cOpCode,"intmuls") == 0)
			{
				for(j=0;j<iCycles;j++)
				{
					IntegerMul obj;
					IMulRS.push_back(obj);
				}
			}
			else if(strcmp(cOpCode,"intdivs") == 0)
			{
				for(j=0;j<iCycles;j++)
				{
					IntegerDiv obj;
					IDivRS.push_back(obj);
				}
			}
			else if(strcmp(cOpCode,"fpadds") == 0)
			{
				for(j=0;j<iCycles;j++)
				{
					FloatAdd obj;
					FAddRS.push_back(obj);
				}
			}
			else if(strcmp(cOpCode,"fpmuls") == 0)
			{
				for(j=0;j<iCycles;j++)
				{
					FloatMul obj;
					FMulRS.push_back(obj);
				}
			}
			else if(strcmp(cOpCode,"fpdivs") == 0)
			{
				for(j=0;j<iCycles;j++)
				{
					FloatDiv obj;
					FDivRS.push_back(obj);
				}
			}
			else
			{
				iRetVal = E_FAIL;
				break;
			}
		}
	}
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
	Instruction inst;
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

		if(!inst.setInstruction(sLine))
			return E_FAIL;
		vprogram.push_back(inst);
		//cout<<iLineNumber<<". "<<inst.getInstruction()<<endl;
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

bool Issue()
{
	// Fetch the current instruction
	// Update the PC to next available PC

	bool bRetVal = E_SUCCESS, bnewPC = false;
	Instruction inst;
	if(bStallIF == 0 && programCounter < vprogram.size() && !bFreeze)
	{
		IR = vprogram[programCounter++];
		bnewPC = true;
		lInstructionCount++;
		triggerend = false;
	}
	else if(programCounter == vprogram.size())
	{
		if(bStallIF == 1 || bFreeze)
			IR = vprogram[programCounter-1];
		else
		{
			IR.instructionReset();
			programCounter++;
		}
	}
	else if(programCounter < vprogram.size() && bStallIF)
	{
		IR = vprogram[programCounter-1];
	}

	// Decode the Instruction
	InstructionDecode objDecode(IR);	
	if(objDecode.decode())
		inst = objDecode.getInstruction();
	// Load the Reservation Stations
	LoadReservationStation objRS(inst);
	objRS.run();
//	else if(programCounter <= vprogram.size())
	if(bStallIF)
		lStallCount++;

	if((inst.getInstruction()).length() > 0)
		cout<<"Stage Issue  : "<<inst.getInstruction()<<"\tPC = "<<programCounter-1<<endl;
	else
		cout<<"Stage Issue  : "<<endl;
	return bRetVal;
}

bool execute()
{
	bool bRetVal = E_SUCCESS;
	Instruction inst;
	int i = 0;
	FunctionalUnit *fu;
	// Execute all the reservation stations
	
//	cout<<"Inside Execute Stage\n";
//	cout<<"LoadRS.size() = "<<LoadRS.size()<<endl;
	for(i=0; i < LoadRS.size() ;i++)
		LoadRS[i].execute();

	for(i=0; i < StoreRS.size() ;i++)
		StoreRS[i].execute();

	for(i=0; i < IAddRS.size() ;i++)
		IAddRS[i].execute();

	for(i=0; i < IMulRS.size() ;i++)
		IMulRS[i].execute();

	for(i=0; i < IDivRS.size() ;i++)
		IDivRS[i].execute();

	for(i=0; i < FAddRS.size() ;i++)
		FAddRS[i].execute();

	for(i=0; i < FMulRS.size() ;i++)
		FMulRS[i].execute();

	for(i=0; i < FDivRS.size() ;i++)
		FDivRS[i].execute();

	return bRetVal;
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
		cout<<setw(10)<<left<<integer_registers[reg].getstatus();
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
		cout<<setw(10)<<left<<float_registers[reg].getstatus();
	}
	cout<<endl;
}