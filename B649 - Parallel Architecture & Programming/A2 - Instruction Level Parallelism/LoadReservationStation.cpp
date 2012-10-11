#include <iostream>
#include "sim_t.h"

LoadReservationStation::LoadReservationStation(Instruction& i)
{
	inst = i;
	this->target = i.getTarget();
	this->isDouble = i.getisDouble();
}
bool LoadReservationStation::run()
{
	bool bRetVal = true;
	OPCODE OpCode = inst.getopCode();
	int i = 0;

	// If the reservation station is not free then we have to stall
	// Update the register file system
	// Set the execution count for each instruction
	if(OpCode == NOOP)
	{
		//bRetVal = decodeNOOP();
	}
	else if(OpCode == LOAD || OpCode == LOAD_D )
	{
		for(i=0;i<NoOfFunctionalUnits["loads"];i++)
			if(!LoadRS[i].getBusyStatus())
			{
				LoadRS[i].setBusyStatus();	// Set the busy flag of the reservation station
				LoadRS[i].assignInstruction(inst);  // Initializes the execution count, and updates the register file system
				if(inst.getTarget() != NULL)
				{
					if(wbobject.getBusyStatus() == true && wbobject.getTargetReg() == inst.getTargetReg())
					{
						LoadRS[i].resetFU();
						i = NoOfFunctionalUnits["loads"];
						break;
					}
					if(inst.getisDouble())
					{
						// If the instruction cannot get a lock on the target Register then Stall.
						if((float_registers[inst.getTargetReg()]).getWaitOnFUid() == -1)
							(float_registers[inst.getTargetReg()]).setWaitOnFUid(LoadRS[i].getmyFUid());
						else	// Started Adding on 18-Nov
						{
							LoadRS[i].resetFU();
							i = NoOfFunctionalUnits["loads"];
							break;
						}		// Ended Adding on 18-Nov
					}
					else
					{ 
						if((integer_registers[inst.getTargetReg()]).getWaitOnFUid() == -1)
							(integer_registers[inst.getTargetReg()]).setWaitOnFUid(LoadRS[i].getmyFUid());
						else	// Started Adding on 18-Nov
						{
							LoadRS[i].resetFU();
							i = NoOfFunctionalUnits["loads"];
							break;
						}		// Ended Adding on 18-Nov
					}
				}
				if(stallDueToRSid[0] == 1)
				{
					bStallIF = false;
					stallDueToRSid[0] = false;
				}
				break;
			}
		if(i == NoOfFunctionalUnits["loads"])
		{
			cout<<"Load Units FULL. Stalling."<<endl;
			bStallIF = true;
			stallDueToRSid[0] = true;
		}
	}
	else if(OpCode == STORE || OpCode == STORE_D )
	{
		for(i=0;i<NoOfFunctionalUnits["stores"];i++)
			if(!StoreRS[i].getBusyStatus())
			{
				StoreRS[i].setBusyStatus();
				StoreRS[i].assignInstruction(inst);
				if(inst.getTarget() != NULL)
				{
					if(wbobject.getBusyStatus() == true && wbobject.getTargetReg() == inst.getTargetReg())
					{
						StoreRS[i].resetFU();
						i = NoOfFunctionalUnits["stores"];
						break;
					}
					if(inst.getisDouble())
					{
						if((float_registers[inst.getTargetReg()]).getWaitOnFUid() == -1)
							(float_registers[inst.getTargetReg()]).setWaitOnFUid(StoreRS[i].getmyFUid());
						else	// Started Adding on 18-Nov
						{
							LoadRS[i].resetFU();
							i = NoOfFunctionalUnits["loads"];
							break;
						}		// Ended Adding on 18-Nov
					}
					else
					{ 
						if((integer_registers[inst.getTargetReg()]).getWaitOnFUid() == -1)
							(integer_registers[inst.getTargetReg()]).setWaitOnFUid(StoreRS[i].getmyFUid());
						else	// Started Adding on 18-Nov
						{
							LoadRS[i].resetFU();
							i = NoOfFunctionalUnits["loads"];
							break;
						}		// Ended Adding on 18-Nov
					}
				}
				if(stallDueToRSid[1] == 1)
				{
					bStallIF = false;
					stallDueToRSid[1] = false;
				}
				break;
			}
		if(i == NoOfFunctionalUnits["stores"])
		{
			cout<<"Store Units FULL. Stalling."<<endl;
			bStallIF = true;
			stallDueToRSid[1] = true;
		}
	}
	else if(OpCode == ADD || OpCode == SUB)
	{
		int i;
		for(i=0;i<NoOfFunctionalUnits["intadds"];i++)
			if(!IAddRS[i].getBusyStatus())
			{
				IAddRS[i].setBusyStatus();
				if( (integer_registers[inst.getsourceReg1()]).getWaitOnFUid() == -1)
				{
					IAddRS[i].Vj = (void *)&integer_registers[inst.getsourceReg1()];
					IAddRS[i].Qj = NULL;
				}
				else
				{
					IAddRS[i].Vj = NULL;
					IAddRS[i].Qj = (void *)&integer_registers[inst.getsourceReg1()];
				}
				if( (integer_registers[inst.getsourceReg2()]).getWaitOnFUid() == -1)
				{
					IAddRS[i].Vk = (void *)&integer_registers[inst.getsourceReg2()];
					IAddRS[i].Qk = NULL;
				}
				else
				{
					IAddRS[i].Vk = NULL;
					IAddRS[i].Qk = (void *)&integer_registers[inst.getsourceReg2()];
				}
				IAddRS[i].assignInstruction(inst);
				if(inst.getTarget() != NULL)
				{
					if(wbobject.getBusyStatus() == true && wbobject.getTargetReg() == inst.getTargetReg())
					{
						IAddRS[i].resetFU();
						i = NoOfFunctionalUnits["intadds"];
						break;
					}
					if((integer_registers[inst.getTargetReg()]).getWaitOnFUid() == -1)
						(integer_registers[inst.getTargetReg()]).setWaitOnFUid(IAddRS[i].getmyFUid());
				}
				if(stallDueToRSid[2] == 1)
				{
					bStallIF = false;
					stallDueToRSid[2] = false;
				}
				break;
			}
		if(i == NoOfFunctionalUnits["intadds"])
		{
			cout<<"Integer ADD Units FULL. Stalling."<<endl;
			bStallIF = true;
			stallDueToRSid[2] = true;
		}
	}
	else if(OpCode == BEQ || OpCode == BNEQ || OpCode == BGTZ || OpCode == BLTZ || OpCode == MOVE  || OpCode == LOADIMMEDIATE)
	{
		int i;
		for(i=0;i<NoOfFunctionalUnits["intadds"];i++)
			if(!IAddRS[i].getBusyStatus())
			{
				IAddRS[i].setBusyStatus();
				if(OpCode == LOADIMMEDIATE)
				{
/*					if( (integer_registers[inst.getTargetReg()]).getWaitOnFUid() == -1)
					{
						IAddRS[i].Vj = (void *)&integer_registers[inst.getTargetReg()];
						IAddRS[i].Qj = NULL;
					}
					else
					{
						IAddRS[i].Vj = NULL;
						IAddRS[i].Qj = (void *)&integer_registers[inst.getTargetReg()];
					}
*/				}
				else
				{
					if( (integer_registers[inst.getsourceReg1()]).getWaitOnFUid() == -1)
					{
						IAddRS[i].Vj = (void *)&integer_registers[inst.getsourceReg1()];
						IAddRS[i].Qj = NULL;
					}
					else
					{
						IAddRS[i].Vj = NULL;
						IAddRS[i].Qj = (void *)&integer_registers[inst.getsourceReg1()];
					}
					if(OpCode == BEQ || OpCode == BNEQ)
					{
						if( (integer_registers[inst.getsourceReg2()]).getWaitOnFUid() == -1)
						{
							IAddRS[i].Vk = (void *)&integer_registers[inst.getsourceReg2()];
							IAddRS[i].Qk = NULL;
						}
						else
						{
							IAddRS[i].Vk = NULL;
							IAddRS[i].Qk = (void *)&integer_registers[inst.getsourceReg2()];
						}
					}
				}
				// All the instructions subsequent to branch intruction will be stalled until it is evaluated by the assigned Reservation Station.
				// "bStallDueToBranch"  is reset by the Reservation Station which is executing this branch instruction. 
				if( inst.getopCode() == BEQ || inst.getopCode() == BNEQ || inst.getopCode() == BGTZ || inst.getopCode() == BLTZ)
					bStallIF = bStallDueToBranch = true;

				IAddRS[i].assignInstruction(inst);
				if(inst.getTarget() != NULL)
				{
					// Started Adding on 18-Nov
					// Check if there is an immediate WriteBack into the target register. 
					// If yes then stall becuase locking the register would turn futile 
					// as Write Back would reset the registers.
					if(wbobject.getBusyStatus() == true && wbobject.getTargetReg() == inst.getTargetReg())
					{
						IAddRS[i].resetFU();
						i = NoOfFunctionalUnits["intadds"];
						break;
					}
					// Ended Adding on 18-Nov
					if((integer_registers[inst.getTargetReg()]).getWaitOnFUid() == -1)
						(integer_registers[inst.getTargetReg()]).setWaitOnFUid(IAddRS[i].getmyFUid());
				}
				if(stallDueToRSid[2] == true)
				{
					bStallIF = false;
					stallDueToRSid[2] = false;
				}
				break;
			}
		if(i == NoOfFunctionalUnits["intadds"])
		{
			bStallIF = true;
			stallDueToRSid[2] = true;
			cout<<"Integer ADD Units FULL. Stalling."<<endl;
		}
	}
	else if(OpCode == ADD_D || OpCode == SUB_D || OpCode == BEQ_D || OpCode == BNEQ_D || OpCode == BGTZ_D || OpCode == BLTZ_D || OpCode == MOVE_D)
	{
		for(i=0;i<NoOfFunctionalUnits["fpadds"];i++)
			if(!FAddRS[i].getBusyStatus())
			{
				FAddRS[i].setBusyStatus();
				if( (float_registers[inst.getsourceReg1()]).getWaitOnFUid() == -1)
				{
					FAddRS[i].Vj = (void *)&float_registers[inst.getsourceReg1()];
					FAddRS[i].Qj = NULL;
				}
				else
				{
					FAddRS[i].Vj = NULL;
					FAddRS[i].Qj = (void *)&float_registers[inst.getsourceReg1()];
				}
				if( (float_registers[inst.getsourceReg2()]).getWaitOnFUid() == -1)
				{
					FAddRS[i].Vk = (void *)&float_registers[inst.getsourceReg2()];
					FAddRS[i].Qk = NULL;
				}
				else
				{
					FAddRS[i].Vk = NULL;
					FAddRS[i].Qk = (void *)&float_registers[inst.getsourceReg2()];
				}
				FAddRS[i].assignInstruction(inst);
				if(inst.getTarget() != NULL)
				{
					// Started Adding on 18-Nov
					// Check if there is an immediate WriteBack into the target register. 
					// If yes then stall becuase locking the register would turn futile 
					// as Write Back would reset the registers.
					if(wbobject.getBusyStatus() == true && wbobject.getTargetReg() == inst.getTargetReg())
					{
						FAddRS[i].resetFU();
						i = NoOfFunctionalUnits["fpadds"];
						break;
					}
					// Ended Adding on 18-Nov
					if((float_registers[inst.getTargetReg()]).getWaitOnFUid() == -1)
						(float_registers[inst.getTargetReg()]).setWaitOnFUid(FAddRS[i].getmyFUid());
				}
				if(stallDueToRSid[3] == 1)
				{
					bStallIF = false;
					stallDueToRSid[3] = false;
				}
				break;
			}
		if(i == NoOfFunctionalUnits["fpadds"])
		{
			cout<<"Float ADD Units FULL. Stalling."<<endl;
			bStallIF = true;
			stallDueToRSid[3] = true;
		}
	}
	else if(OpCode == MUL )
	{
		for(i=0;i<NoOfFunctionalUnits["intmuls"];i++)
			if(!IMulRS[i].getBusyStatus())
			{
				IMulRS[i].setBusyStatus();
				if( (integer_registers[inst.getsourceReg1()]).getWaitOnFUid() == -1)
				{
					IMulRS[i].Vj = (void *)&integer_registers[inst.getsourceReg1()];
					IMulRS[i].Qj = NULL;
				}
				else
				{
					IMulRS[i].Vj = NULL;					
					IMulRS[i].Qj = (void *)&integer_registers[inst.getsourceReg1()];
				}
				if( (integer_registers[inst.getsourceReg2()]).getWaitOnFUid() == -1)
				{
					IMulRS[i].Vk = (void *)&integer_registers[inst.getsourceReg2()];
					IMulRS[i].Qk = NULL;
				}
				else
				{
					IMulRS[i].Vk = NULL;
					IMulRS[i].Qk = (void *)&integer_registers[inst.getsourceReg2()];
				}
				IMulRS[i].assignInstruction(inst);
				if(inst.getTarget() != NULL)
				{
					// Started Adding on 18-Nov
					// Check if there is an immediate WriteBack into the target register. 
					// If yes then stall becuase locking the register would turn futile 
					// as Write Back would reset the registers.
					if(wbobject.getBusyStatus() == true && wbobject.getTargetReg() == inst.getTargetReg())
					{
						IMulRS[i].resetFU();
						i = NoOfFunctionalUnits["intmuls"];
						break;
					}
					// Ended Adding on 18-Nov

					if((integer_registers[inst.getTargetReg()]).getWaitOnFUid() == -1)
						(integer_registers[inst.getTargetReg()]).setWaitOnFUid(IMulRS[i].getmyFUid());
				}
				if(stallDueToRSid[4] == 1)
				{
					bStallIF = false;
					stallDueToRSid[4] = false;
				}
				break;			
			}
		if(i == NoOfFunctionalUnits["intmuls"])
		{
			cout<<"Integer MUL Units FULL. Stalling."<<endl;
			bStallIF = true;
			stallDueToRSid[4] = true;
		}
	}
	else if(OpCode == MUL_D )
	{
		for(i=0;i<NoOfFunctionalUnits["fpmuls"];i++)
			if(!FMulRS[i].getBusyStatus())
			{
				FMulRS[i].setBusyStatus();
				if( (float_registers[inst.getsourceReg1()]).getWaitOnFUid() == -1)
				{
					FMulRS[i].Vj = (void *)&float_registers[inst.getsourceReg1()];
					FMulRS[i].Qj = NULL;
				}
				else
				{
					FMulRS[i].Vj = NULL;
					FMulRS[i].Qj = (void *)&float_registers[inst.getsourceReg1()];
				}
				if( (float_registers[inst.getsourceReg2()]).getWaitOnFUid() == -1)
				{
					FMulRS[i].Vk = (void *)&float_registers[inst.getsourceReg2()];
					FMulRS[i].Qk = NULL;
				}
				else
				{
					FMulRS[i].Vk = NULL;
					FMulRS[i].Qk = (void *)&float_registers[inst.getsourceReg2()];
				}
				FMulRS[i].assignInstruction(inst);
				if(inst.getTarget() != NULL)
				{
					// Started Adding on 18-Nov
					// Check if there is an immediate WriteBack into the target register. 
					// If yes then stall becuase locking the register would turn futile 
					// as Write Back would reset the registers.
					if(wbobject.getBusyStatus() == true && wbobject.getTargetReg() == inst.getTargetReg())
					{
						FMulRS[i].resetFU();
						i = NoOfFunctionalUnits["fpmuls"];
						break;
					}
					// Ended Adding on 18-Nov
					if((float_registers[inst.getTargetReg()]).getWaitOnFUid() == -1)
						(float_registers[inst.getTargetReg()]).setWaitOnFUid(FMulRS[i].getmyFUid());
				}
				if(stallDueToRSid[5] == 1)
				{
					bStallIF = false;
					stallDueToRSid[5] = false;
				}
				break;
			}
		if(i == NoOfFunctionalUnits["fpmuls"])
		{
			cout<<"Float MUL Units FULL. Stalling."<<endl;
			bStallIF = true;
			stallDueToRSid[5] = true;
		}
	}
	else if(OpCode == DIV )
	{
		for(i=0;i<NoOfFunctionalUnits["intdivs"];i++)
			if(!IDivRS[i].getBusyStatus())
			{
				IDivRS[i].setBusyStatus();
				if(isDouble)
				{
					if( (float_registers[inst.getsourceReg1()]).getWaitOnFUid() == -1)
					{
						IDivRS[i].Vj = (void *)&float_registers[inst.getsourceReg1()];
						IDivRS[i].Qj = NULL;
					}
					else
					{
						IDivRS[i].Vj = NULL;
						IDivRS[i].Qj = (void *)&float_registers[inst.getsourceReg1()];
					}
					if( (float_registers[inst.getsourceReg2()]).getWaitOnFUid() == -1)
					{
						IDivRS[i].Vk = (void *)&float_registers[inst.getsourceReg2()];
						IDivRS[i].Qk = NULL;
					}
					else
					{
						IDivRS[i].Vk = NULL;
						IDivRS[i].Qk = (void *)&float_registers[inst.getsourceReg2()];
					}
				}
				else
				{
					if( (integer_registers[inst.getsourceReg1()]).getWaitOnFUid() == -1)
					{
						IDivRS[i].Vj = (void *)&integer_registers[inst.getsourceReg1()];
						IDivRS[i].Qj = NULL;
					}
					else
					{
						IDivRS[i].Vj = NULL;
						IDivRS[i].Qj = (void *)&integer_registers[inst.getsourceReg1()];
					}
					if( (integer_registers[inst.getsourceReg2()]).getWaitOnFUid() == -1)
					{
						IDivRS[i].Vk = (void *)&integer_registers[inst.getsourceReg2()];
						IDivRS[i].Qk = NULL;
					}
					else
					{
						IDivRS[i].Vk = NULL;
						IDivRS[i].Qk = (void *)&integer_registers[inst.getsourceReg2()];
					}
				}
				IDivRS[i].assignInstruction(inst);
				if(inst.getTarget() != NULL)
				{
					// Started Adding on 18-Nov
					// Check if there is an immediate WriteBack into the target register. 
					// If yes then stall becuase locking the register would turn futile 
					// as Write Back would reset the registers.
					if(wbobject.getBusyStatus() == true && wbobject.getTargetReg() == inst.getTargetReg())
					{
						IDivRS[i].resetFU();
						i = NoOfFunctionalUnits["intdivs"];
						break;
					}
					// Ended Adding on 18-Nov

					if((integer_registers[inst.getTargetReg()]).getWaitOnFUid() == -1)
						(integer_registers[inst.getTargetReg()]).setWaitOnFUid(IDivRS[i].getmyFUid());
				}				
				if(stallDueToRSid[6] == 1)
				{
					bStallIF = false;
					stallDueToRSid[6] = false;
				}
				break;
			}
		if(i == NoOfFunctionalUnits["intdivs"])
		{
			cout<<"Integer DIV Units FULL. Stalling."<<endl;
			bStallIF = true;
			stallDueToRSid[6] = true;
		}
	}
	else if(OpCode == DIV_D )
	{
		for(i=0;i<NoOfFunctionalUnits["fpdivs"];i++)
			if(!FDivRS[i].getBusyStatus())
			{
				FDivRS[i].setBusyStatus();
				if(isDouble)
				{
					if( (float_registers[inst.getsourceReg1()]).getWaitOnFUid() == -1)
					{
						FDivRS[i].Vj = (void *)&float_registers[inst.getsourceReg1()];
						FDivRS[i].Qj = NULL;
					}
					else
					{
						FDivRS[i].Vj = NULL;
						FDivRS[i].Qj = (void *)&float_registers[inst.getsourceReg1()];
					}
					if( (float_registers[inst.getsourceReg2()]).getWaitOnFUid() == -1)
					{
						FDivRS[i].Vk = (void *)&float_registers[inst.getsourceReg2()];
						FDivRS[i].Qk = NULL;
					}
					else
					{
						FDivRS[i].Vk = NULL;
						FDivRS[i].Qk = (void *)&float_registers[inst.getsourceReg2()];
					}
				}
				else
				{
					if( (integer_registers[inst.getsourceReg1()]).getWaitOnFUid() == -1)
						FDivRS[i].Vj = (void *)&integer_registers[inst.getsourceReg1()];
					if( (integer_registers[inst.getsourceReg2()]).getWaitOnFUid() == -1)
						FDivRS[i].Vk = (void *)&integer_registers[inst.getsourceReg2()];
				}
				FDivRS[i].assignInstruction(inst);
				if(inst.getTarget() != NULL)
				{
					// Started Adding on 18-Nov
					// Check if there is an immediate WriteBack into the target register. 
					// If yes then stall becuase locking the register would turn futile 
					// as Write Back would reset the registers.
					if(wbobject.getBusyStatus() == true && wbobject.getTargetReg() == inst.getTargetReg())
					{
						FDivRS[i].resetFU();
						i = NoOfFunctionalUnits["fdivs"];
						break;
					}
					// Ended Adding on 18-Nov					
					
					if((float_registers[inst.getTargetReg()]).getWaitOnFUid() == -1)
						(float_registers[inst.getTargetReg()]).setWaitOnFUid(FDivRS[i].getmyFUid());
				}
				if(stallDueToRSid[7] == 1)
				{
					bStallIF = false;
					stallDueToRSid[7] = false;
				}
				break;
			}
		if(i == NoOfFunctionalUnits["fpdivs"])
		{
			cout<<"Float DIV Units FULL. Stalling."<<endl;
			bStallIF = true;
			stallDueToRSid[7] = true;
		}
	}
	else
	{
		cout<<"Invalid OpCode. Line No. "<<endl;
		bRetVal = E_FAIL;
	}
	return bRetVal;
}