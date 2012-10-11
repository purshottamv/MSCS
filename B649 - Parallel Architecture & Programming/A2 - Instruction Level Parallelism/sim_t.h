/* ============================================================================
 Name        : sim_t.h
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : Program simulating Tomasulo Algorithm for Dynamic Scheduling
 ============================================================================
*/
#ifndef TOMASULO_H
#define TOMASULO_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <iomanip>

#define E_SUCCESS	1
#define E_FAIL		0

#define MAXCHARS 200

using namespace std;

typedef enum
{
	NONE = 0,
	ISSUE,
	EXEC,
	WB
}InstructionStage;

typedef enum
{
	LD = 0,
	ST,
}LoadStore;

typedef enum
{
	NOOP = 0,   
	ADD = 1,  
	ADD_D = 2,  
	DIV = 3,
	DIV_D = 4,  
	SUB = 5,    
	SUB_D = 6,  
	MUL = 7,    
	MUL_D = 8,  
	LOADIMMEDIATE = 9,
	LOAD = 10,
	STORE = 11,     
	LOAD_D = 12,    
	STORE_D = 13,    
	BEQ =14,    
	BNEQ = 15,   
	BGTZ = 16,   
	BLTZ = 17,   
	MOVE = 18,   
	BEQ_D = 19,  
	BNEQ_D = 20, 
	BGTZ_D = 21,
	BLTZ_D = 22, 
	MOVE_D = 23, 
}OPCODE;

class IntegerRegisterFS
{
	int val;
	int waitOnFUid;  // on which functional unit is this waiting
	public:
		IntegerRegisterFS(){
			val = 0;
			waitOnFUid = -1;
		}
		int getval(){
			return val;
		}
		void setval(int v){
			val = v;
		}
		void setWaitOnFUid(int i){
			waitOnFUid = i;
		}
		int getWaitOnFUid(){
			return waitOnFUid;
		}
		void resetRegister(){
			//val = 0;
			waitOnFUid = -1;
		}
		string getstatus()
		{
			string str;
			char c[6];
			if(waitOnFUid == -1)
				sprintf(c,"%d",val);
			else
				sprintf(c,"W-%d",waitOnFUid);
			str.assign(c);
			return str;
		}
};
class FloatRegisterFS
{
	double val;
	int waitOnFUid;	// on which functional unit is this waiting
	public:
		FloatRegisterFS(){
			val = 0;
			waitOnFUid = -1;
		}
		double getval(){
			return val;
		}
		void setval(double v){
			val = v;
		}
		int getWaitOnFUid(){
			return waitOnFUid;
		}
		void setWaitOnFUid(int x){
			waitOnFUid = x;;
		}
		void resetRegister(){
			//val = 0;
			waitOnFUid = -1;
		}
		string getstatus()
		{
			string str;
			char c[6];
			if(waitOnFUid == -1)
				sprintf(c,"%.3f",val);
			else
				sprintf(c,"W-%d",waitOnFUid);
			str.assign(c);
			return c;
		}
};
typedef union
{
	int ival;
	double dval;
}final_val;

class WriteBack
{
		string targetReg;
		bool isDouble;
		bool bBusy;
		final_val val;
		void *target;
	public:
		WriteBack();
		void run();
		bool sendtowriteback(bool b,final_val &v,void *t,string r);
		void reset();
		string getTargetReg();
		bool getBusyStatus();
};

extern map<string,OPCODE> OpCodeMap;
extern map<OPCODE,int> opcode_cycles;
extern map<string,int> NoOfFunctionalUnits;
extern map<string,IntegerRegisterFS> integer_registers;
extern map<string,FloatRegisterFS> float_registers;
void ltrim(string&);
void rtrim(string&);
extern bool endProgram, bStallIF, bStallDueToBranch;
extern int programCounter;
extern map<string,long> mlabels;
extern char *mem;
extern bool triggerend;
extern bool stallDueToRSid[8];  // 8 is the number of types of reservation stations
extern WriteBack wbobject;

class Instruction
{
	string sInstruction;	// Contains the complete Instruction
	string sLine;	// Contains the remaining part of the instruction except the OpCode
	OPCODE iOpCode;	// Contains the Numeric Value of OpCode
	string sTempA;
	string sTempB;
	string sImmediate;
	long branchTarget;
	bool isDouble;
	char sourceReg1[20];
	char sourceReg2[20];
	string targetReg;
	void *target;
	InstructionStage istage;
	char cImmediate[20];
	int iImmediate;
	public:
		Instruction()
		{
			isDouble = 0;
			iOpCode = NOOP;
			sInstruction = "";
			sLine = "";
			memset(sourceReg1,'\0',20);
			memset(sourceReg2,'\0',20);
			targetReg = "";
			target = NULL;
			istage = NONE;
			memset(cImmediate,'\0',20);
			iImmediate = 0;
			branchTarget = -1;
		}
		void setTempA(char *c){
			sTempA.assign(c);
		}
		void setTempB(char *c){
			sTempB.assign(c);
		}
		void setImmediate(char *c){
			sImmediate.assign(c);
		}
		string getsLine()
		{ return sLine;}
		InstructionStage getInstructionStage(){
			return istage;
		}
		string getInstruction(){
			return sInstruction;
		}
		void *getTarget(){
			return target;
		}
		string getTargetReg(){
			return targetReg;
		}
		void setbranchTarget(long v)
		{ branchTarget = v;		
		}
		long getbranchTarget()
		{	return branchTarget;  }
		void instructionReset()
		{
			isDouble = 0;
			iOpCode = NOOP;
			sInstruction = "";
			memset(sourceReg1,'\0',20);
			memset(sourceReg2,'\0',20);
			targetReg = "";
			istage = NONE;
			sLine = "";
			target = NULL;
			iImmediate = 0;
			memset(cImmediate,'\0',20);
		}
		bool setInstruction(string& s){
			int i;
			sInstruction = s;
			string sOpCode;
			if(!(s.length() > 0 && (i = s.find(" ")) > 0))
			{
				if(s.compare(0,4,"NOOP") == 0)
					i = 4;
				else
				{
					cout<<"Error: No Opcode\n";
					return E_FAIL;
				}
			}
			sOpCode.assign(s,0,i);
			if(sOpCode != "NOOP")
			{
				sLine.assign(s,i,s.length()-i);
				ltrim(sLine);
				rtrim(sLine);
			}
			else
				sLine = "";

			if(sOpCode.find(".D") < sOpCode.length())
				isDouble = 1;
			else
				isDouble = 0;
			
			if(OpCodeMap.count(sOpCode) != 1)
			{
				cout<<"Invalid OPCODE."<<endl;
				return E_FAIL;
			}
			else
				iOpCode = OpCodeMap[sOpCode];
			return E_SUCCESS;
		}
		OPCODE getopCode(){
			return iOpCode;
		}
		string getLine(){
			return sLine;
		}
		bool getisDouble(){
			return isDouble;
		}
		void settargetRegister(char *r){
			targetReg.assign(r);
			if(iOpCode != MOVE && iOpCode != MOVE_D)
			{
				if(isDouble)
					target = (void *)&float_registers[r];
				else
					target = (void *)&integer_registers[r];
			}
			else
			{
				if(isDouble)
					target = (void *)&integer_registers[r];
				else
					target = (void *)&float_registers[r];
			}
		}
		void setsourceReg1(char *r){
			memcpy(sourceReg1,r,strlen(r));
//			if(isDouble)
//				sourceReg1 = (void *)&float_registers[r];
//			else
//				sourceReg2 = (void *)&integer_registers[r];
		}
		void setsourceReg2(char *r){
			memcpy(sourceReg2,r,strlen(r));
//			if(isDouble)
//				sourceReg2 = (void *)&float_registers[r];
//			else
//				sourceReg2 = (void *)&integer_registers[r];
		}
		char *getsourceReg1()
		{
			return sourceReg1;
		}
		char *getsourceReg2()
		{
			return sourceReg2;
		}
		void setsImmediate(char *imm){
			memcpy(&cImmediate,imm,strlen(imm));
		}
		string getTempA(){
			return sTempA;
		}
		string getTempB(){
			return sTempB;
		}
		string getsImmediate(){
			return sImmediate;
		}
		void setiImmediate(int v)
		{ iImmediate = v;	}
		int getiImmediate()
		{  return iImmediate;  }

};
class InstructionDecode
{
	Instruction inst;
	char cTempA[20];
	char cTempB[20];
	char cImmediate[20];
	bool isDouble;
	public:
		InstructionDecode(Instruction &i);
		bool decode();
		bool decodeNOOP();
		bool decodeArithmatic();
		bool decodeLoadStore();
		bool decodeLI();
		bool decodeS();
		bool decodeBEQ();
		bool decodeBNEQ();
		bool decodeBGTZ();
		bool decodeBLTZ();
		bool decodeMOVE();
		bool validateRegister(bool isDouble,char* reg);
		bool detecthazard(char *r);
		Instruction getInstruction();
};
class LoadReservationStation
{
	Instruction inst;
	void *target;
	bool isDouble;
	public:
		LoadReservationStation(Instruction &i);
		bool run();
};

class FunctionalUnit
{
	protected:
		Instruction ins;
		static int FUid;
		int myFUid;
		bool bIsBusy;
		final_val finalvalue;
		bool bDoNotExecute;
		int iCount, execCountRemaining;
		bool bWaitToWriteBack;
	public:
		FunctionalUnit(){
			bIsBusy = 0;
			myFUid = FUid++;
			bDoNotExecute = 0;
			iCount = execCountRemaining = 0;
			finalvalue.dval = 0;
			bWaitToWriteBack = false;
		}
		void assignInstruction(Instruction i)
		{
			ins = i;
			iCount = execCountRemaining = opcode_cycles[ins.getopCode()];
			bDoNotExecute = true;
		}
		void resetFU(){
			bIsBusy = 0;
			bDoNotExecute = iCount = execCountRemaining = 0;
			bWaitToWriteBack = false;
			finalvalue.dval = 0;
			ins.instructionReset();
		}
		void setBusyStatus(){
			bIsBusy = true;
		}
		bool getBusyStatus()
		{
			return bIsBusy;
		}
		int getmyFUid()
		{
			return myFUid;
		}
		virtual bool execute() = 0;
};

class LSReservationStation : public FunctionalUnit
{
	LoadStore lsType;
	double memAddress;
	public:
		LSReservationStation(LoadStore v){
			lsType = v;
			memAddress = -1;
		}
		void resetFU(){
			FunctionalUnit::resetFU();
			memAddress = -1;
		}
		void setAvailable(){
			bIsBusy = false;
		}
		bool execute(){
//			cout<<"Inside LS execute: bDoNotExecute = "<<bDoNotExecute<<" bIsBusy = "<<bIsBusy<<endl;
			bool bRetVal = true;
			if(bIsBusy)
			{
				triggerend = false;
				if(lsType == LD)
					cout<<"\n"<<setw(16)<<left<<"Load  Unit "<<myFUid<<": Busy = "<<bIsBusy<<" Address = "<<ins.getsLine();
				else
					cout<<"\n"<<setw(16)<<left<<"Store Unit "<<myFUid<<": Busy = "<<bIsBusy<<" Address = "<<ins.getsLine();
			}
			if(bDoNotExecute || !bIsBusy){
				bDoNotExecute = bDoNotExecute?false:bDoNotExecute;
			}
			else
			{
				cout<<" CyclesPending = "<<execCountRemaining;
				if(execCountRemaining > 0)
				{
					// Any Instruction should start executing only after the register values are available
					if(integer_registers[ins.getsourceReg1()].getWaitOnFUid() == -1 && execCountRemaining == iCount)
					{
						char temp[20];
						memset(temp,'\0',20);
						string str = ins.getTempA();
						str.copy(temp,str.length(),0);
						memAddress = (double)(atoi(temp) + integer_registers[ins.getsourceReg1()].getval());
						if(ins.getTarget() != NULL)
						{
							if(ins.getisDouble())
							{
								if((float_registers[ins.getTargetReg()]).getWaitOnFUid() == -1 || 
									(float_registers[ins.getTargetReg()]).getWaitOnFUid() == myFUid)
								{
									(float_registers[ins.getTargetReg()]).setWaitOnFUid(myFUid);
									execCountRemaining--;
								}
							}
							else
							{ 
								if((integer_registers[ins.getTargetReg()]).getWaitOnFUid() == -1 ||
									(integer_registers[ins.getTargetReg()]).getWaitOnFUid() == myFUid  )
								{
									(integer_registers[ins.getTargetReg()]).setWaitOnFUid(myFUid);
									execCountRemaining--;
								}
							}
						}
					}
					else if(execCountRemaining < iCount)
						execCountRemaining--;
				}
 				else
				{
					if(!bWaitToWriteBack)
					{
						double dval;
						char output[20];
						memset(output,'\0',20);
						if(lsType == LD)
						{
							if(ins.getisDouble())
							{
								memcpy(output,&mem[long(memAddress)],8);
								//((FloatRegisterFS *)ins.getTarget())->setval(strtod(output,NULL));
								//((FloatRegisterFS *)ins.getTarget())->resetRegister();
								finalvalue.dval = strtod(output,NULL);

							}
							else
							{
								memcpy(output,&mem[long(memAddress)],4);
								//((IntegerRegisterFS *)ins.getTarget())->setval(atoi(output));
								//((IntegerRegisterFS *)ins.getTarget())->resetRegister();
								finalvalue.ival = strtod(output,NULL);
							}
							cout<<" Loaded "<<output<<" from "<<memAddress;
						}
						else if(lsType == ST)
						{
							if(ins.getisDouble()){
								sprintf(&mem[long(memAddress)],"%g",((FloatRegisterFS *)ins.getTarget())->getval());							
								//mem[long(memAddress)] = ((FloatRegisterFS *)ins.getTarget())->getval();
								((FloatRegisterFS *)ins.getTarget())->resetRegister();
							}
							else{
								sprintf(&mem[long(memAddress)],"%d",((IntegerRegisterFS *)ins.getTarget())->getval());
								//mem[long(memAddress)] = ((IntegerRegisterFS *)ins.getTarget())->getval();
								((IntegerRegisterFS *)ins.getTarget())->resetRegister();
							}
							cout<<" Stored "<<((FloatRegisterFS *)ins.getTarget())->getval()<<" to "<<memAddress<<endl;
							resetFU();
						}
					}
					if(lsType == LD)
					{
						if(wbobject.sendtowriteback(ins.getisDouble(),finalvalue,ins.getTarget(),ins.getTargetReg()) )
							resetFU();
						else
						{
							cout<<" Waiting to Write Back "<<finalvalue.ival;
							bWaitToWriteBack = true;
						}
					}
				}
			}
			//cout<<endl;
			return bRetVal;
		}
		void assignLoad(Instruction i){

		}
		void assignStore(Instruction i){

		}
};

class IntegerAdd : public FunctionalUnit
{
	public:
	void *Vj,*Vk,*Qj,*Qk;
	bool StallFU;
	int s1,s2;

	IntegerAdd(){
		Vj = Vk = Qj = Qk = NULL;
		s1 = s2 = 0;
		iCount = 0;
		StallFU = false;
	}
	void resetFU(){
		FunctionalUnit::resetFU();
		Vj = Vk = Qj = Qk = NULL;
		s1 = s2 = 0;
	}

	bool execute(){
		bool bRetVal = true, bprintVQ = false;
		if(bIsBusy){	
			triggerend = false;
			bprintVQ = true;
			cout<<"\n"<<setw(16)<<left<<"Integer ADD Unit "<<myFUid<<": Busy = "<<bIsBusy<<" "<<ins.getInstruction();
		}
		if(bDoNotExecute || !bIsBusy){
			bDoNotExecute = bDoNotExecute?false:bDoNotExecute;
		}
		else
		{
			cout<<" CyclesPending = "<<execCountRemaining;
			if(execCountRemaining > 0)
			{
				if(ins.getopCode() != LOADIMMEDIATE && execCountRemaining == iCount)
				{
					if(Vj == NULL)
					{
						if(Qj != NULL && ((IntegerRegisterFS *)Qj)->getWaitOnFUid() == -1)
						{
							Vj = Qj;
							Qj = NULL;
							s1 = integer_registers[ins.getsourceReg1()].getval();
						}
					}
					else
						s1 = integer_registers[ins.getsourceReg1()].getval();

					if( ins.getopCode() != BLTZ && ins.getopCode() != BGTZ &&  ins.getopCode() != MOVE && Vk == NULL)
					{
						if(Qk != NULL && ((IntegerRegisterFS *)Qk)->getWaitOnFUid() == -1)
						{
							Vk = Qk;
							Qk = NULL;
							s2 = integer_registers[ins.getsourceReg2()].getval();
						}
					}
					else if( ins.getopCode() != BLTZ && ins.getopCode() != BGTZ &&  ins.getopCode() != MOVE && Vk != NULL)
						s2 = integer_registers[ins.getsourceReg2()].getval();
				}
				if( (ins.getopCode() != LOADIMMEDIATE && Vj != NULL && Vk != NULL) ||
					(ins.getopCode() == MOVE && Vj != NULL) ||
					(ins.getopCode() == BGTZ && Vj != NULL) ||
					(ins.getopCode() == BLTZ && Vj != NULL)  )
				{
					if(execCountRemaining == iCount)
					{
						if(ins.getopCode() == ADD || ins.getopCode() == SUB)
						{
							if( (ins.getTarget() != NULL ) && ((integer_registers[ins.getTargetReg()]).getWaitOnFUid() == -1 || 
								(integer_registers[ins.getTargetReg()]).getWaitOnFUid() == myFUid) )
							{
									(integer_registers[ins.getTargetReg()]).setWaitOnFUid(myFUid);
									execCountRemaining--;
							}
						}
						else
							execCountRemaining--;
					}
					else
						execCountRemaining--;
					
				}
				if(ins.getopCode() == LOADIMMEDIATE)
				{
					if(execCountRemaining == iCount)
					{ 
						if(ins.getTarget() != NULL &&
							( (integer_registers[ins.getTargetReg()]).getWaitOnFUid() == -1 || 
							  (integer_registers[ins.getTargetReg()]).getWaitOnFUid() == this->myFUid) )
						{
							(integer_registers[ins.getTargetReg()]).setWaitOnFUid(myFUid);
							execCountRemaining--;
						}
					}
					else
						execCountRemaining--;
				}
			}
			else
			{
				if(!bWaitToWriteBack)
				{
					if(ins.getopCode() == ADD){
						finalvalue.ival = s1 + s2;
						//cout<<" Added "<<finalvalue.ival<<" <- "<<ins.getsourceReg1()<<" + "<<ins.getsourceReg2();
						cout<<" Added "<<finalvalue.ival<<" <- "<<s1<<" + "<<s2;
					}
					else if(ins.getopCode() == SUB)
					{
						finalvalue.ival = s1 - s2;
						//cout<<" Subtracted "<<finalvalue.ival<<" <- "<<ins.getsourceReg1()<<" - "<<ins.getsourceReg2();
						cout<<" Subtracted "<<finalvalue.ival<<" <- "<<s1<<" - "<<s2;
					}
					else if(ins.getopCode() == LOADIMMEDIATE)
					{
						finalvalue.ival = ins.getiImmediate();
					}
					else if(ins.getopCode() == BNEQ || ins.getopCode() == BEQ)
					{
						if(bStallDueToBranch)
							bStallIF = bStallDueToBranch= false;
						
						if(ins.getopCode() == BNEQ && s1 != s2)
							programCounter = ins.getbranchTarget();
						else if(ins.getopCode() == BEQ && s1 == s2)
							programCounter = ins.getbranchTarget();
					}
					else if(ins.getopCode() == BLTZ || ins.getopCode() == BGTZ)
					{
						if(bStallDueToBranch)
							bStallIF = bStallDueToBranch= false;
						
						if(ins.getopCode() == BLTZ && s1 < 0)
							programCounter = ins.getbranchTarget();
						else if(ins.getopCode() == BGTZ && s1 > 0)
							programCounter = ins.getbranchTarget();					
					}
					else if(ins.getopCode() == MOVE)
					{
						cout<<"Moving "<<finalvalue.ival<<" to "<<ins.getTargetReg()<<" ";
						finalvalue.ival = s1;
					}
/*					if(ins.getopCode() == ADD || ins.getopCode() == SUB || ins.getopCode() == LOADIMMEDIATE)
					{
						((IntegerRegisterFS *)ins.getTarget())->setval(ival);
						((IntegerRegisterFS *)ins.getTarget())->resetRegister();
					}
					if(ins.getopCode() == MOVE )
					{
						cout<<"Moving "<<ival<<" to "<<ins.getTargetReg()<<endl;
						((FloatRegisterFS *)ins.getTarget())->setval(ival);
						((FloatRegisterFS *)ins.getTarget())->resetRegister();
					}
*/				}
				if(ins.getopCode() == ADD || ins.getopCode() == SUB || ins.getopCode() == LOADIMMEDIATE || ins.getopCode() == MOVE)
				{
					if(ins.getopCode() == MOVE && wbobject.sendtowriteback(1,finalvalue,ins.getTarget(),ins.getTargetReg()) )
						resetFU();
					else if(ins.getopCode() != MOVE && wbobject.sendtowriteback(0,finalvalue,ins.getTarget(),ins.getTargetReg()) )
						resetFU();
					else
					{
						cout<<" Waiting to Write Back "<<finalvalue.ival;
						bWaitToWriteBack = true;
					}
				}
				else
					resetFU();
			}
		}
		if(bprintVQ)
			printVQ();
		return bRetVal;
	}
	void printVQ(){
		if(Vj == NULL)
			cout<<" Vj=  ";
		else
			cout<<" Vj = "<<((IntegerRegisterFS *)Vj)->getval();
		if(Vk == NULL)
			cout<<" Vk=  ";
		else
			cout<<" Vk = "<<((IntegerRegisterFS *)Vk)->getval();
		if(Qj == NULL)
			cout<<" Qj=  ";
		else
			cout<<" Qj = "<<((IntegerRegisterFS *)Qj)->getval();
		if(Qk == NULL)
			cout<<" Qk=  ";
		else
			cout<<" Qk = "<<((IntegerRegisterFS *)Qk)->getval();
		cout<<" ";
	}
};
class IntegerMul : public FunctionalUnit
{
	public:
	void *Vj,*Vk,*Qj,*Qk;
	int s1,s2;
		IntegerMul(){
			Vj = Vk = Qj = Qk = NULL;
			s1 = s2 = 0;
		}
		void resetFU(){
			FunctionalUnit::resetFU();
			Vj = Vk = Qj = Qk = NULL;
			s1 = s2 = 0;
		}
		bool execute()
		{
			bool bRetVal = true, bprintVQ = false;
			if(bIsBusy){
				cout<<"\n"<<setw(16)<<left<<"Integer MUL Unit "<<myFUid<<": Busy = "<<bIsBusy<<" "<<ins.getInstruction();
				bprintVQ = true;
				triggerend = false;
			}
			if(bDoNotExecute || !bIsBusy){
				bDoNotExecute = bDoNotExecute?false:bDoNotExecute;
			}
			else
			{
				cout<<" CyclesPending = "<<execCountRemaining;
				if(execCountRemaining > 0)
				{
					if(execCountRemaining == iCount)
					{
						if(Vj == NULL)
						{
							if(Qj != NULL && ((IntegerRegisterFS *)Qj)->getWaitOnFUid() == -1)
							{
								Vj = Qj;
								Qj = NULL;
								s1 = integer_registers[ins.getsourceReg1()].getval();
							}
						}
						else
						{
							s1 = integer_registers[ins.getsourceReg1()].getval();
						}

						if(Vk == NULL)
						{
							if(Qk != NULL && ((IntegerRegisterFS *)Qk)->getWaitOnFUid() == -1)
							{
								Vk = Qk;
								Qk = NULL;
								s2 = integer_registers[ins.getsourceReg2()].getval();
							}
						}
						else
							s2 = integer_registers[ins.getsourceReg2()].getval();
					}
					if(Vj != NULL && Vk != NULL)
					{
						if(execCountRemaining == iCount)
						{ 
							if(ins.getTarget() != NULL &&
								( (integer_registers[ins.getTargetReg()]).getWaitOnFUid() == -1 || 
								  (integer_registers[ins.getTargetReg()]).getWaitOnFUid() == this->myFUid) )
							{
								(integer_registers[ins.getTargetReg()]).setWaitOnFUid(myFUid);
								execCountRemaining--;
							}
						}
						else
							execCountRemaining--;
					}
				}
				else
				{
					if(!bWaitToWriteBack)
					{
						finalvalue.ival = s1 * s2;
						//((IntegerRegisterFS *)ins.getTarget())->setval(ival);
						//((IntegerRegisterFS *)ins.getTarget())->resetRegister();
						//cout<<" Multiplied "<<finalvalue.ival<<" <- "<<ins.getsourceReg1()<<" * "<<ins.getsourceReg2();
						cout<<" Multiplied "<<finalvalue.ival<<" <- "<<s1<<" * "<<s2;
						//resetFU();
					}
					if(wbobject.sendtowriteback(0,finalvalue,ins.getTarget(),ins.getTargetReg()) )
						resetFU();
					else
					{
						cout<<" Waiting to Write Back "<<finalvalue.ival;
						bWaitToWriteBack = true;
					}
				}
			}
			if(bprintVQ)
				printVQ();
			return bRetVal;
	}
	void printVQ(){
		if(Vj == NULL)
			cout<<" Vj=  ";
		else
			cout<<" Vj = "<<((IntegerRegisterFS *)Vj)->getval();
		if(Vk == NULL)
			cout<<" Vk=  ";
		else
			cout<<" Vk = "<<((IntegerRegisterFS *)Vk)->getval();
		if(Qj == NULL)
			cout<<" Qj=  ";
		else
			cout<<" Qj = "<<((IntegerRegisterFS *)Qj)->getval();
		if(Qk == NULL)
			cout<<" Qk=  ";
		else
			cout<<" Qk = "<<((IntegerRegisterFS *)Qk)->getval();
		cout<<" ";
	}
};
class IntegerDiv : public FunctionalUnit
{
	public:
	void *Vj,*Vk,*Qj,*Qk;
	int s1 , s2;
		IntegerDiv(){
		Vj = Vk = Qj = Qk = NULL;
		s1 = s2 = 0;
	}
	void resetFU(){
		FunctionalUnit::resetFU();
		Vj = Vk = Qj = Qk = NULL;
		s1 = s2 = 0;
	}
		bool execute()
		{
			bool bRetVal = true, bprintVQ = false;
			if(bIsBusy){
				cout<<"\n"<<setw(16)<<left<<"Integer DIV Unit "<<myFUid<<": Busy = "<<bIsBusy<<" "<<ins.getInstruction();
				bprintVQ = true;
				triggerend = false;
			}
			if(bDoNotExecute || !bIsBusy){
				bDoNotExecute = bDoNotExecute?false:bDoNotExecute;
			}
			else
			{
				cout<<" CyclesPending = "<<execCountRemaining;
				if(execCountRemaining > 0)
				{
					if(execCountRemaining == iCount)
					{
						if(Vj == NULL)
						{
							if(Qj != NULL && ((IntegerRegisterFS *)Qj)->getWaitOnFUid() == -1)
							{
								Vj = Qj;
								Qj = NULL;
								s1 = integer_registers[ins.getsourceReg1()].getval();
							}
						}
						else
							s1 = integer_registers[ins.getsourceReg1()].getval();

						if(Vk == NULL)
						{
							if(Qk != NULL && ((IntegerRegisterFS *)Qk)->getWaitOnFUid() == -1)
							{
								Vk = Qk;
								Qk = NULL;
								s2 = integer_registers[ins.getsourceReg2()].getval();
							}
						}
						else
							s2 = integer_registers[ins.getsourceReg2()].getval();
					}

					if(Vj != NULL && Vk != NULL)
					{
						if(execCountRemaining == iCount)
						{ 
							if(ins.getTarget() != NULL &&
								( (integer_registers[ins.getTargetReg()]).getWaitOnFUid() == -1 || 
								  (integer_registers[ins.getTargetReg()]).getWaitOnFUid() == this->myFUid) )
							{
								(integer_registers[ins.getTargetReg()]).setWaitOnFUid(myFUid);
								execCountRemaining--;
							}
						}
						else
							execCountRemaining--;
					}
				}
				else
				{
					if(!bWaitToWriteBack)
					{
						finalvalue.ival = s1 / s2;
						//((IntegerRegisterFS *)ins.getTarget())->setval(ival);
						//((IntegerRegisterFS *)ins.getTarget())->resetRegister();
						//cout<<" Divided "<<finalvalue.ival<<" <- "<<ins.getsourceReg1()<<" / "<<ins.getsourceReg2();
						cout<<" Divided "<<finalvalue.ival<<" <- "<<s1<<" / "<<s2;
						//resetFU();
					}
					if(wbobject.sendtowriteback(0,finalvalue,ins.getTarget(),ins.getTargetReg()) )
						resetFU();
					else
					{
						cout<<" Waiting to Write Back "<<finalvalue.ival;
						bWaitToWriteBack = true;
					}
				}
			}
			if(bprintVQ)
				printVQ();
			return bRetVal;
	}
	void printVQ(){
		if(Vj == NULL)
			cout<<" Vj=  ";
		else
			cout<<" Vj = "<<((IntegerRegisterFS *)Vj)->getval();
		if(Vk == NULL)
			cout<<" Vk=  ";
		else
			cout<<" Vk = "<<((IntegerRegisterFS *)Vk)->getval();
		if(Qj == NULL)
			cout<<" Qj=  ";
		else
			cout<<" Qj = "<<((IntegerRegisterFS *)Qj)->getval();
		if(Qk == NULL)
			cout<<" Qk=  ";
		else
			cout<<" Qk = "<<((IntegerRegisterFS *)Qk)->getval();
		cout<<" ";
	}
};
class FloatAdd : public FunctionalUnit
{
	public:
	void *Vj,*Vk,*Qj,*Qk;
	double s1,s2;
		FloatAdd(){
		Vj = Vk = Qj = Qk = NULL;
		s1 = s2 = 0;
	}
	void resetFU(){
		FunctionalUnit::resetFU();
		Vj = Vk = Qj = Qk = NULL;
		s1 = s2 = 0;
	}
		bool execute()
		{
			bool bRetVal = true, bprintVQ = false;
			if(bIsBusy){
				cout<<"\n"<<setw(16)<<left<<"Float ADD Unit "<<myFUid<<": Busy = "<<bIsBusy<<" "<<ins.getInstruction();
				bprintVQ = true;
				triggerend = false;
			}
			if(bDoNotExecute || !bIsBusy){
				bDoNotExecute = bDoNotExecute?false:bDoNotExecute;
			}
			else
			{
				cout<<" CyclesPending = "<<execCountRemaining;
				if(execCountRemaining > 0){
					if(execCountRemaining == iCount)
					{
						if(Vj == NULL)
						{
							if(Qj != NULL && ((FloatRegisterFS *)Qj)->getWaitOnFUid() == -1)
							{
								Vj = Qj;
								Qj = NULL;
								s1 = float_registers[ins.getsourceReg1()].getval();
							}
						}
						else
							s1 = float_registers[ins.getsourceReg1()].getval();

						if( (ins.getopCode() == SUB_D || ins.getopCode() == ADD_D || ins.getopCode() == BNEQ_D || ins.getopCode() == BEQ_D) && Vk == NULL)
						{
							if(Qk != NULL && ((FloatRegisterFS *)Qk)->getWaitOnFUid() == -1)
							{
								Vk = Qk;
								Qk = NULL;
								s2 = float_registers[ins.getsourceReg2()].getval();
							}
						}
						else if( (ins.getopCode() == SUB_D || ins.getopCode() == ADD_D || ins.getopCode() == BNEQ_D || ins.getopCode() == BEQ_D) && Vk != NULL)
							s2 = float_registers[ins.getsourceReg2()].getval();
					}

					if( (ins.getopCode() == SUB_D || ins.getopCode() == ADD_D || ins.getopCode() == BNEQ_D || ins.getopCode() == BEQ_D) &&
						Vj != NULL && Vk != NULL)
					{
						if(execCountRemaining == iCount)
						{ 
							if(ins.getopCode() == ADD_D || ins.getopCode() == SUB_D)
							{
								if(ins.getTarget() != NULL &&
									( (float_registers[ins.getTargetReg()]).getWaitOnFUid() == -1 || 
									  (float_registers[ins.getTargetReg()]).getWaitOnFUid() == this->myFUid) )
								{
									(float_registers[ins.getTargetReg()]).setWaitOnFUid(myFUid);
									execCountRemaining--;
								}
							}
							else
								execCountRemaining--;
						}
						else
							execCountRemaining--;
					}

					if( (ins.getopCode() == BLTZ_D || ins.getopCode() == BGTZ_D || ins.getopCode() == MOVE_D) &&
						Vj != NULL)
					{
						if(execCountRemaining == iCount)
						{ 
							if(ins.getTarget() != NULL &&
								( (float_registers[ins.getTargetReg()]).getWaitOnFUid() == -1 || 
								  (float_registers[ins.getTargetReg()]).getWaitOnFUid() == this->myFUid) )
							{
								(float_registers[ins.getTargetReg()]).setWaitOnFUid(myFUid);
								execCountRemaining--;
							}
						}
						else
							execCountRemaining--;
					}
				}
				else
				{
					if(!bWaitToWriteBack)
					{
						if(ins.getopCode() == ADD_D){
							finalvalue.dval = s1 + s2;
							//cout<<" Added "<<dval<<" <- "<<ins.getsourceReg1()<<" + "<<ins.getsourceReg2();
							cout<<" Added "<<finalvalue.dval<<" <- "<<s1<<" + "<<s2;
						}
						else if(ins.getopCode() == SUB_D) {
							finalvalue.dval = s1 - s2;
							//cout<<" Subtracted "<<dval<<" <- "<<ins.getsourceReg1()<<" - "<<ins.getsourceReg2();
							cout<<" Subtracted "<<finalvalue.dval<<" <- "<<s1<<" - "<<s2;
						}
						else if(ins.getopCode() == BNEQ_D || ins.getopCode() == BEQ_D)
						{
							if(bStallDueToBranch)
								bStallIF = bStallDueToBranch = false;
							
							if(ins.getopCode() == BNEQ && s1 != s2)
								programCounter = ins.getbranchTarget();
							else if(ins.getopCode() == BEQ && s1 == s2)
								programCounter = ins.getbranchTarget();
						}
						else if(ins.getopCode() == BLTZ_D || ins.getopCode() == BGTZ_D)
						{
							if(bStallDueToBranch)
								bStallIF = bStallDueToBranch= false;
							
							if(ins.getopCode() == BLTZ && s1 < 0)
								programCounter = ins.getbranchTarget();
							else if(ins.getopCode() == BGTZ && s1 > 0)
								programCounter = ins.getbranchTarget();					
						}
						else if(ins.getopCode() == MOVE_D)
						{
							finalvalue.dval = s1;
							//((IntegerRegisterFS *)ins.getTarget())->setval((int)dval);
							//((IntegerRegisterFS *)ins.getTarget())->resetRegister();
						}
						if(ins.getopCode() == ADD_D || ins.getopCode() == SUB_D)
						{
							//((FloatRegisterFS *)ins.getTarget())->setval(dval);
							//((FloatRegisterFS *)ins.getTarget())->resetRegister();
						}
						//resetFU();
					}
					if(ins.getopCode() == ADD_D || ins.getopCode() == SUB_D || ins.getopCode() == MOVE_D)
					{
						if(ins.getopCode() == MOVE_D && wbobject.sendtowriteback(0,finalvalue,ins.getTarget(),ins.getTargetReg()) )
							resetFU();
						else if(ins.getopCode() != MOVE_D && wbobject.sendtowriteback(1,finalvalue,ins.getTarget(),ins.getTargetReg()) )
							resetFU();
						else
						{
							cout<<" Waiting to Write Back "<<finalvalue.dval<<" ";
							bWaitToWriteBack = true;
						}
					}
					else
						resetFU();
				}
			}
			if(bprintVQ)
				printVQ();

			return bRetVal;
	}
	void printVQ(){
		if(Vj == NULL)
			cout<<" Vj=  ";
		else
			cout<<" Vj = "<<((FloatRegisterFS *)Vj)->getval();
		if(Vk == NULL)
			cout<<" Vk=  ";
		else
			cout<<" Vk = "<<((FloatRegisterFS *)Vk)->getval();
		if(Qj == NULL)
			cout<<" Qj=  ";
		else
			cout<<" Qj = "<<((FloatRegisterFS *)Qj)->getval();
		if(Qk == NULL)
			cout<<" Qk=  ";
		else
			cout<<" Qk = "<<((FloatRegisterFS *)Qk)->getval();
		cout<<" ";
	}
};
class FloatMul : public FunctionalUnit
{
	public:
		void *Vj,*Vk,*Qj,*Qk;
		double s1,s2;
			FloatMul(){
			Vj = Vk = Qj = Qk = NULL;
			s1 = s2 = 0;
		}
		void resetFU(){
			FunctionalUnit::resetFU();
			Vj = Vk = Qj = Qk = NULL;
			s1 = s2 = 0;
		}
		void printVQ(){
			if(Vj == NULL)
				cout<<" Vj=  ";
			else
				cout<<" Vj = "<<((FloatRegisterFS *)Vj)->getval();
			if(Vk == NULL)
				cout<<" Vk=  ";
			else
				cout<<" Vk = "<<((FloatRegisterFS *)Vk)->getval();
			if(Qj == NULL)
				cout<<" Qj=  ";
			else
				cout<<" Qj = "<<((FloatRegisterFS *)Qj)->getval();
			if(Qk == NULL)
				cout<<" Qk=  ";
			else
				cout<<" Qk = "<<((FloatRegisterFS *)Qk)->getval();
			cout<<" ";
		}
		bool execute()
		{
			bool bRetVal = true, bprintVQ = false;
			if(bIsBusy)
			{
				cout<<"\n"<<setw(16)<<left<<"Float MUL Unit "<<myFUid<<": Busy = "<<bIsBusy<<" "<<ins.getInstruction();
				bprintVQ = true;
				triggerend = false;
			}
			if(bDoNotExecute || !bIsBusy){
				bDoNotExecute = bDoNotExecute?false:bDoNotExecute;
			}
			else
			{
				cout<<" CyclesPending = "<<execCountRemaining;
				if(execCountRemaining > 0){
					if(execCountRemaining == iCount)
					{
						if(Vj == NULL)
						{
							if(Qj != NULL && ((FloatRegisterFS *)Qj)->getWaitOnFUid() == -1)
							{
								Vj = Qj;
								Qj = NULL;
								s1 = float_registers[ins.getsourceReg1()].getval();
							}
						}
						else
							s1 = float_registers[ins.getsourceReg1()].getval();

						if(Vk == NULL)
						{
							if(Qk != NULL && ((FloatRegisterFS *)Qk)->getWaitOnFUid() == -1)
							{
								Vk = Qk;
								Qk = NULL;
								s2 = float_registers[ins.getsourceReg2()].getval();
							}
						}
						else
							s2 = float_registers[ins.getsourceReg2()].getval();
					}

					if(Vj != NULL && Vk != NULL)
					{
						if(execCountRemaining == iCount)
						{ 
							if(ins.getTarget() != NULL &&
								( (float_registers[ins.getTargetReg()]).getWaitOnFUid() == -1 || 
								  (float_registers[ins.getTargetReg()]).getWaitOnFUid() == this->myFUid) )
							{
								(float_registers[ins.getTargetReg()]).setWaitOnFUid(myFUid);
								execCountRemaining--;
							}
						}
						else
							execCountRemaining--;
					}
				}
				else
				{
					if(!bWaitToWriteBack)
					{	
						finalvalue.dval = s1 * s2;
						//((FloatRegisterFS *)ins.getTarget())->setval(dval);
						//((FloatRegisterFS *)ins.getTarget())->resetRegister();
	//					cout<<" Multiplied "<<dval<<" <- "<<ins.getsourceReg1()<<" * "<<ins.getsourceReg2();
						cout<<" Multiplied "<<finalvalue.dval<<" <- "<<s1<<" * "<<s2;
						//resetFU();
					}
					if(wbobject.sendtowriteback(1,finalvalue,ins.getTarget(),ins.getTargetReg()) )
						resetFU();
					else
					{
						cout<<" Waiting to Write Back "<<finalvalue.dval<<" ";
						bWaitToWriteBack = true;
					}
				}
			}
			if(bprintVQ)
				printVQ();
			//cout<<endl;
			return bRetVal;
	}
};
class FloatDiv : public FunctionalUnit
{
	public:
	double s1,s2;
	void *Vj,*Vk,*Qj,*Qk;
		FloatDiv(){
		Vj = Vk = Qj = Qk = NULL;
			s1 = s2 = 0;
	}
	void resetFU(){
		FunctionalUnit::resetFU();
		Vj = Vk = Qj = Qk = NULL;
			s1 = s2 = 0;
	}
		bool execute()
		{
			bool bRetVal = true, bprintVQ = false;
			if(bIsBusy)
			{
				cout<<"\n"<<setw(16)<<left<<"Float DIV Unit "<<myFUid<<": Busy = "<<bIsBusy<<" "<<ins.getInstruction();
				bprintVQ = true;
				triggerend = false;
			}
			if(bDoNotExecute || !bIsBusy){
				bDoNotExecute = bDoNotExecute?false:bDoNotExecute;
			}
			else
			{
				cout<<" CyclesPending = "<<execCountRemaining;
				if(execCountRemaining > 0){
					if(execCountRemaining == iCount)
					{
						if(Vj == NULL)
						{
							if(Qj != NULL && ((FloatRegisterFS *)Qj)->getWaitOnFUid() == -1)
							{
								Vj = Qj;
								Qj = NULL;
								s1 = float_registers[ins.getsourceReg1()].getval();
							}
						}
						else 
							s1 = float_registers[ins.getsourceReg1()].getval();

						if(Vk == NULL)
						{
							if(Qk != NULL && ((FloatRegisterFS *)Qk)->getWaitOnFUid() == -1)
							{
								Vk = Qk;
								Qk = NULL;
								s2 = float_registers[ins.getsourceReg2()].getval();
							}
						}
						else 
							s2 = float_registers[ins.getsourceReg2()].getval();
					}

					if(Vj != NULL && Vk != NULL)
					{
						if(execCountRemaining == iCount)
						{ 
							if(ins.getTarget() != NULL &&
								( (float_registers[ins.getTargetReg()]).getWaitOnFUid() == -1 || 
								  (float_registers[ins.getTargetReg()]).getWaitOnFUid() == this->myFUid) )
							{
								(float_registers[ins.getTargetReg()]).setWaitOnFUid(myFUid);
								execCountRemaining--;
							}
						}
						else
							execCountRemaining--;
					}
				}
				else
				{
					if(!bWaitToWriteBack)
					{
						finalvalue.dval = s1 / s2;
						//((FloatRegisterFS *)ins.getTarget())->setval(dval);
						//((FloatRegisterFS *)ins.getTarget())->resetRegister();
						//cout<<" Divided "<<finalvalue.dval<<" <- "<<ins.getsourceReg1()<<" / "<<ins.getsourceReg2();
						cout<<" Divided "<<finalvalue.dval<<" <- "<<s1<<" / "<<s2;
						//resetFU();
					}
					if(wbobject.sendtowriteback(1,finalvalue,ins.getTarget(),ins.getTargetReg()) )
						resetFU();
					else
					{
						cout<<" Waiting to Write Back "<<finalvalue.dval;
						bWaitToWriteBack = true;
					}
				}
			}
			if(bprintVQ)
				printVQ();

			//cout<<endl;
			return bRetVal;
	}
	void printVQ(){
		if(Vj == NULL)
			cout<<" Vj=  ";
		else
			cout<<" Vj = "<<((FloatRegisterFS *)Vj)->getval();
		if(Vk == NULL)
			cout<<" Vk=  ";
		else
			cout<<" Vk = "<<((FloatRegisterFS *)Vk)->getval();
		if(Qj == NULL)
			cout<<" Qj=  ";
		else
			cout<<" Qj = "<<((FloatRegisterFS *)Qj)->getval();
		if(Qk == NULL)
			cout<<" Qk=  ";
		else
			cout<<" Qk = "<<((FloatRegisterFS *)Qk)->getval();
		cout<<" ";
	}
};

extern vector<LSReservationStation> LoadRS;	// Vector of Load Reservation Stations
extern vector<LSReservationStation> StoreRS;	// Vector of Store Reservation Stations
extern vector<IntegerAdd> IAddRS;
extern vector<IntegerMul> IMulRS;
extern vector<IntegerDiv> IDivRS;
extern vector<FloatAdd> FAddRS;
extern vector<FloatMul> FMulRS;
extern vector<FloatDiv> FDivRS;


#endif