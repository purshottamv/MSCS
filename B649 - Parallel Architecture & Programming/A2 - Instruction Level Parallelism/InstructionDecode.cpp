#include <iostream>
#include "sim_t.h"

InstructionDecode::InstructionDecode(Instruction& i)
{
	inst = i;
	memset(cTempA,'\0',20);
	memset(cTempB,'\0',20);
	memset(cImmediate,'\0',20);
	isDouble = inst.getisDouble();
	cout<<"\n\nDecoding "<<inst.getInstruction()<<endl;
}
Instruction InstructionDecode::getInstruction()
{
	return inst;
}
bool InstructionDecode::decode()
{

	bool bRetVal = E_SUCCESS;
	int i;
	OPCODE OpCode = inst.getopCode();

	if(OpCode == NOOP)
		bRetVal = decodeNOOP();
	else if( (OpCode >= ADD) && (OpCode <= MUL_D) )
		bRetVal = decodeArithmatic();
	else if(OpCode == LOADIMMEDIATE)
		bRetVal = decodeLI();
	else if(OpCode == LOAD || OpCode == STORE || OpCode == LOAD_D || OpCode == STORE_D )
		bRetVal = decodeLoadStore();
	else if( OpCode == BEQ || OpCode == BEQ_D)
		bRetVal = decodeBEQ();
	else if( OpCode == BNEQ || OpCode == BNEQ_D)
		bRetVal = decodeBNEQ();
	else if( OpCode == BGTZ || OpCode == BGTZ_D)
		bRetVal = decodeBGTZ();
	else if( OpCode == BLTZ || OpCode == BLTZ_D)
		bRetVal = decodeBLTZ();
	else if( OpCode == MOVE || OpCode == MOVE_D)
		bRetVal = decodeMOVE();
	else
	{
		cout<<"Invalid OpCode : "<<OpCode<<endl;
		bRetVal = E_FAIL;
	}
//	cout<<"Stage ID  : "<<IRBuffered<<" \tA = "<<cTempA<<" B = "<<cTempB<<endl;
	
	return bRetVal;
}

bool InstructionDecode::decodeNOOP()
{
	bool bRetVal = E_SUCCESS;
	return bRetVal;
}

bool InstructionDecode::decodeArithmatic()
{
	int i,icount=0;
	char reg[4];
	int *is1,*is2, *itarget;
	double *ds1,*ds2,*dtarget;
	string sLine;

	sLine = inst.getLine();	
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
		if(!validateRegister(inst.getisDouble(),reg))
		{
			cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the target register
			inst.settargetRegister(reg);
		else if(icount == 1)	// store the source 1 register
		{
			if(detecthazard(reg))
				return E_SUCCESS;
			inst.setsourceReg1(reg);
		}
		else if(icount == 2)	// store the source 2 register
		{
			if(detecthazard(reg))
				return E_SUCCESS;
			inst.setsourceReg2(reg);
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}
	return E_SUCCESS;
}

bool InstructionDecode::decodeLoadStore()
{
	// L   t.I, o(s.I)  //Integer Load
	// L.D t.D, o(s.I)  //Floating Point Load
	// S   t.I, o(s2.I) //Integer store
	// S.D t.D, o(s2.I) //Floating point store

	bool bRetVal = E_SUCCESS;
	int i,i2;
	char reg[4],offset[10];
	int *is, *itarget;
	double *dtarget;
	string sLine;

	sLine = inst.getLine();	
	memset(reg,'\0',4);
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
	if(!validateRegister(inst.getisDouble(),reg))
	{
		cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
		return E_FAIL;
	}
	sLine.assign(sLine,i+1,sLine.length()-i-1);
	
//	if(inst.getopCode() == LOAD)
	inst.settargetRegister(reg); // Store the target register

	if((inst.getopCode() == STORE) && detecthazard(reg))
		return E_SUCCESS;

	if((i = sLine.find("(")) > 0)
	{
		sLine.copy(cTempA,i,0);
		inst.setTempA(cTempA);
		if((i2 = sLine.find(")",i)) > 0)
		{
			sLine.copy(reg,i2-i-1,i+1);
			reg[i2-i] = '\0';
			if(!validateRegister(0,reg)) // Overriding the first argument
			{
				cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
				return E_FAIL;
			}
//			sprintf(cTempB,"%d",integer_registers[reg]);
			if(detecthazard(reg))
				return E_SUCCESS;
			inst.setsourceReg1(reg);
		}
		else
		{	
			cout<<"Syntax Error. Line No. "<<endl;
			return E_FAIL;
		}
	}
	else
	{
		cout<<"Syntax Error. Line No. "<<endl;
		return E_FAIL;
	}
	return bRetVal;
}

bool InstructionDecode::decodeLI()
{
	// LI t.I, o // Integer load immediate

	int i;
	char reg[4],immediate[20];
	int *itarget;
	string sLine;

	sLine = inst.getLine();	
	memset(reg,'\0',4);
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
		cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
		return E_FAIL;
	}
	sLine.copy(cImmediate,sLine.length()-i,i+1);
	inst.setImmediate(cImmediate);
	inst.setiImmediate(atoi(cImmediate));
	inst.settargetRegister(reg);

	return E_SUCCESS;
}

bool InstructionDecode::decodeBEQ()
{
	int i,icount=0;
	char reg[20];
	IntegerRegisterFS *is1,*is2;
	FloatRegisterFS *ds1,*ds2;
	long *branchTarget;

	string sLine;

	sLine = inst.getLine();	
	memset(reg,'\0',20);
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
			cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the source 1 register
		{
			if(detecthazard(reg))
				return E_SUCCESS;
			inst.setsourceReg1(reg);
		}
		else if(icount == 1)	// store the source 2 register
		{
			inst.setsourceReg2(reg);
		}
		else if(icount == 2)	// Check for Branch
		{
			if(mlabels.count(reg) != 1)
			{
				cout<<"Invalid Label. Program Line No."<<programCounter<<endl;
				return E_FAIL;
			}
			inst.setbranchTarget(mlabels[reg]);
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}

	return E_SUCCESS;
}

bool InstructionDecode::decodeBNEQ()
{
	int i,icount=0;
	char reg[20];
	IntegerRegisterFS *is1,*is2;
	FloatRegisterFS *ds1,*ds2;
	long *branchTarget;
	string sLine;

	sLine = inst.getLine();	
	memset(reg,'\0',20);

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
			cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the source 1 register
		{
			if(detecthazard(reg))
				return E_SUCCESS;

			inst.setsourceReg1(reg);
		}
		else if(icount == 1)	// store the source 2 register
		{
			inst.setsourceReg2(reg);
		}
		else if(icount == 2)	// Check for Branch
		{
			if(mlabels.count(reg) != 1)
			{
				cout<<"Invalid Label. Program Line No."<<programCounter<<endl;
				return E_FAIL;
			}
			inst.setbranchTarget(mlabels[reg]);
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}
	return E_SUCCESS;
}

bool InstructionDecode::decodeBGTZ()
{
	int i,icount=0;
	char reg[20];
	IntegerRegisterFS *is1,*is2;
	FloatRegisterFS *ds1,*ds2;
	long *branchTarget;
	string sLine;

	sLine = inst.getLine();	
	memset(reg,'\0',20);
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
			cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the source 1 register
		{
			if(detecthazard(reg))
				return E_SUCCESS;

			inst.setsourceReg1(reg);
		}
		else if(icount == 1)	// Check for Branch
		{
			if(mlabels.count(reg) != 1)
			{
				cout<<"Invalid Label. Program Line No."<<programCounter<<endl;
				return E_FAIL;
			}
			inst.setbranchTarget(mlabels[reg]);
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}
	return E_SUCCESS;
}

bool InstructionDecode::decodeBLTZ()
{
	int i,icount=0;
	char reg[20];
	IntegerRegisterFS *is1,*is2;
	FloatRegisterFS *ds1,*ds2;
	long *branchTarget;

	string sLine;

	sLine = inst.getLine();	
	memset(reg,'\0',20);
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
			cout<<"Invalid register. Program Line No. "<<programCounter+1<<endl;
			return E_FAIL;
		}
		if(icount == 0)	// store the source 1 register
		{
			if(detecthazard(reg))
				return E_SUCCESS;
			inst.setsourceReg1(reg);
		}
		else if(icount == 1)	// Check for Branch
		{
			if(mlabels.count(reg) != 1)
			{
				cout<<"Invalid Label. Program Line No."<<programCounter<<endl;
				return E_FAIL;
			}
			inst.setbranchTarget(mlabels[reg]);
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}
	return E_SUCCESS;
}

bool InstructionDecode::decodeMOVE()
{
	int i,icount=0;
	char reg[4];
	string sLine;
	sLine = inst.getLine();
	while(icount < 2)
	{
		memset(reg,'\0',4);
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
			cout<<"reg (icount = 1) = "<<reg<<endl;
			cout<<"Invalid register. "<<reg<<" isDouble="<<isDouble<<endl;
			return E_FAIL;
		}
		else if(icount == 0 && (!validateRegister(isDouble,reg)) )
		{
			cout<<"reg (icount = 0) = "<<reg<<endl;
			cout<<"Invalid register. "<<reg<<" isDouble="<<isDouble<<endl;
			return E_FAIL;
		}
		if(icount == 1)	// store the target register
			inst.settargetRegister(reg);
		else if(icount == 0)	// store the source 1 register
		{
			if(detecthazard(reg))
				return E_SUCCESS;
			inst.setsourceReg1(reg);
		}
		sLine.assign(sLine,i+1,sLine.length()-i-1);
		icount++;
	}
	return E_SUCCESS;
}

bool InstructionDecode::validateRegister(bool isDouble,char* reg)
{
	bool bRetVal = E_SUCCESS;
	if((isDouble && float_registers.count(reg) == 0) ||
		(!isDouble && integer_registers.count(reg) == 0))
		bRetVal = E_FAIL;

	return bRetVal;
}
bool InstructionDecode::detecthazard(char *r)
{
	bool iRetVal = false;
	
	return iRetVal;
}