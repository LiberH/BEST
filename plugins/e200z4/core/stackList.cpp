/**
 * generated by HARMLESS : 'Hardware ARchitecture Modeling Language
 *                          for Embedded Software Simulation'
 * model : e200z4
 *
 */

#ifdef GADL_SP_CHECK_ALLOWED

#include <iostream>
#include <string>
using namespace std;

#include "arch.h"
#include "action.h"
#include "stackSpyAction.h"
#include "stackList.h"
#include "stackController.h"


/*****************************
 *  stackInfo : functions    *
 *****************************/

stackInfo::stackInfo(string taskName, string functionSymbol,type_sp size,stackSpyController* controller){
	type_prgaddr v_addr;		// To be found (need elf file)
	m_taskName=taskName;		// By user
	m_symbol=functionSymbol;	// By user
	m_taskId=TASK_UNKNOWN;		// No ID
	m_controller=controller;	// By program
	m_taskAddr=0;				// To be found by program
	
	// -- Local data --
	arch *_arch = controller->getArch();
	bool functionFound = false;

	
	// -- Find program adress of task main function first instruction --
	if(_arch->getFunctionName(functionSymbol.c_str(), v_addr)){
		type_prgaddr p_addr = _arch->getPhysicalAddress(v_addr, functionFound);	// Get program adresse of our function
		m_taskAddr=p_addr;	// Write the programm adress found for first function of Task ===> !!! NEED TO BE FIX (FIX DONE)
		
		// DEBUG
		if(!functionFound){
			#ifdef GADL_TALKATIVE_STACK
			std::cerr << "symbol " << functionSymbol << " have a virtual address (0x" << hex << v_addr << "), but no physical address.";
			#endif // GADL_TALKATIVE_STACK
		}else{
			#ifdef GADL_TALKATIVE_STACK
			std::cout << "symbol " << functionSymbol << " have a virtual address (0x" << hex << v_addr << ").";
			#endif // GADL_TALKATIVE_STACK
		}
		
	// -- Can't find any symbol (unknown function) --
	}else{
		std::cerr << "no symbol found for " << functionSymbol << "\n";
	}

	m_spInit=-1;			// SP Init need to be find
	m_spInitFound=false;	// Flag to say that sp init need to be find
	m_spSize=size;			// Size of stack
	m_ptrNext=NULL;			// No next element for the time being
	
	// -- Stack Monitoring --
	m_underflow=false;
	m_overflow=false;
	m_stackUse=0;
	m_errorPC=0;
	
	
	#ifdef GADL_TALKATIVE_STACK
	this->printStackInfo();
	#endif // GADL_TALKATIVE_STACK
}

void stackInfo::printStackInfo(){
	//#ifdef GADL_TALKATIVE_STACK
	std::cout << "Task n°" << m_taskId << " (" << m_taskName << ") : Fct@" << hex << m_taskAddr << " Stack (size=" << dec << m_spSize << ") : init@"<< hex << m_spInit << " -" << m_spInitFound << "- StackRealUse="<< dec << m_stackUse;
	if (m_overflow) {
		cout<<" !! Overflow !!  "<<"at PC@"<<hex<<m_errorPC;
	}else if(m_underflow){
		cout<<" !! Underflow !! "<<"at PC@"<<hex<<m_errorPC;
	}
	cout<<endl;
	//#endif // GADL_TALKATIVE_STACK
}

/** STACK IS GROWING DOWN (Help Help we will underflow earth soon...) **/
bool stackInfo::isInThisStack(type_sp sp){
	return (m_spInit>=sp && sp>(m_spInit-m_spSize));	// Stack are supposed to be static so normally this is respected
}

/*********************************
 *  taskStackList : functions    *
 *********************************/

// Print taskStackList
void taskStackList::printStackList(){
	u32 watchdog=65535;					// Prevent any infinite loop
	bool endOfList=false;				// Print end
	stackInfo *currentItem=m_firstItem;	// Current item in list
	
	// Welcome
	cout<<endl;
	cout<<"********************************"<<endl;
	cout<<"* STACK/TASK List Informations *"<<endl;
	cout<<"********************************"<<endl;
	cout<<endl;
	
	// Printing loop
	if(m_nbItems>0){
		while(watchdog>0 && !endOfList){
			watchdog--;									// Decrement watchdog
			currentItem->printStackInfo();				// Print current item
			currentItem=currentItem->getPtrNext();		// Jump to next item
			if (currentItem==NULL) {					// No more item in list
				endOfList=true;	
			}
		}
	}
	cout<<endl;
}

// Add a new task/stack in list
void taskStackList::addStack(arch *_arch,stackInfo *m_st){
	/* Local */
	type_prgaddr addrTask;			// Use to recover addr of Task

	/* Corectly add the stack in the list */
	m_nbItems++;					// One more stack inside
	m_st->setPtrNext(m_firstItem);	// The next item is the actual first item
	m_st->setTaskId(m_nbItems);		// The task id is define by order of coming
	m_firstItem=m_st;				// The new item is the new firtItem 
									// and we need to know where he is ortherwhise we will lost each of our little sheeps in our list

	/* Add the associate static action */
	_arch->addExecuteActionAtAddress(m_st->getTaskAddr(), new stackSpyActionStatic(m_controller,m_st->getTaskId(),m_st));	// Add static action to function first instruction
	
	#ifdef	GADL_TALKATIVE_STACK
	//cerr<< "Function of Task @"<< hex <<m_st->getTaskAddr()<<endl;
	#endif // GADL_TALKATIVE_STACK


	
}

// Find Stack/Task ptr using SP value
stackInfo* taskStackList::findStackPtr(type_sp sp){
	
	// -- Local data --
	u32 watchdog=65535;				// Prevent while too go in an infinite loop
	bool isFind=false;				// Did I find what search (is it searching a pin in the whole universe)
	stackInfo* currentStack=m_firstItem;
	
	// -- Searching loop -- (try to find in witch task/stack is the Stack pointer)
	while (currentStack!=NULL && (watchdog>0 && !isFind)) {
		watchdog--;										// Prevent from inifinite loop (so borrrring...)
		
		// -- Ask the task if it's his own stack --
		if(currentStack->isInThisStack(sp)){	
			//taskId=currentStack->getTaskId();			// Yes it is store task ID !
			isFind=true;								// Used to end loop...
		
		// -- No it's not the right stack --
		}else {									
			currentStack=currentStack->getPtrNext();	// Jump to next item
		}

	}
	
	return currentStack;
}

// Secure way to get stack ID using myStack object (returning 0 when *myStack==NULL)
int taskStackList::getStackId(stackInfo *myStack){
	int taskId=TASK_UNKNOWN;				// Task supposed to be unknown (0)
	
	if(myStack!=NULL){						// One stack found
		taskId=myStack->getTaskId();		// Read stack ID
	}
	
	return taskId;
}

// Find TaskId using SP value
int taskStackList::findStack(type_sp sp){
	stackInfo *stackFound=findStackPtr(sp);	// Search wich stack contain this SP

	int taskId=getStackId(stackFound);
	
	return taskId;
}


#endif // GADL_SP_CHECK_ALLOWED
