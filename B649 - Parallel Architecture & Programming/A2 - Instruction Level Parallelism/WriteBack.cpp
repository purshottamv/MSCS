#include "sim_t.h"

WriteBack::WriteBack()
{
			isDouble = false;
			bBusy = false;
			target = NULL;
			targetReg = "";
			val.dval = 0;
}
void WriteBack::reset()
{
			isDouble = false;
			bBusy = false;
			target = NULL;
			targetReg = "";
			val.dval = 0;
}

string WriteBack::getTargetReg()
{
	return targetReg;
}

bool WriteBack::getBusyStatus()
{
	return bBusy;
}

void WriteBack::run()
{
	if(bBusy) 
	{
		// We have something to write into register.
		if(isDouble)
		{
			cout<<"\nWriteBack : "<<val.dval<<" -> "<<targetReg<<endl;
			((FloatRegisterFS *)target)->setval(val.dval);
			((FloatRegisterFS *)target)->resetRegister();
		}
		else
		{
			cout<<"\nWriteBack : "<<val.ival<<" -> "<<targetReg<<endl;
			((IntegerRegisterFS *)target)->setval(val.ival);
			((IntegerRegisterFS *)target)->resetRegister();
		}
		reset();
	}
}

bool WriteBack::sendtowriteback(bool d,final_val &v,void *t,string r)
{
	bool bRetVal = E_SUCCESS;
	if(bBusy)
		bRetVal = E_FAIL; // this means write back already has something to be written.
	else
	{
		isDouble = d;
		target = t;
		targetReg = r;
		val = v;		
		bBusy = true;
	}
	return bRetVal;
}