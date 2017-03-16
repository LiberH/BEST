/** automata simulation
automatically generated using a2cpp.
DO NOT MODIFY DIRECTLY, IT CAN BE OVERWRITTEN.
 */

#ifndef __pipelineExample_H__
#define __pipelineExample_H__

//#define PIPELINE_DEBUG

class pipelineExample;
//These masks should be used to access a specific notification
#define ENTER_FETCH_MASK               0x1
#define EXITS_REGISTER_MASK            0x2
#define ENTER_DECODE_EA_MASK           0x4
#define ENTER_EXECUTE_MASK             0x8
#define ENTER_REGISTER_MASK            0x10

//These offsets should be used to determine the state of external resources
#define EXT_RES_DDCRES                 0
#define EXT_RES_MEMDEV_FETCH           1
#define EXT_RES_MEMDEV_LOADSTORE       2

class e200z4_instruction;
class arch;

typedef unsigned int nextType;
//tabular that give the next state (using current state, and condition)
extern const nextType nextState_pipelineExample[3456];

class pipelineExample{
public:
	//this function is the main function that should be called at each state.
	//\param extResState The external resources states. 
	//	0=>resource is free, 1=>resource is taken. 
	//	One bit for each resource. The order of resources is the same as in the .config file:
	//	the first external resource defined is the lowest significant bit
	//\param ic the instruction class code. 
	//	This is the instruction class code of the next instruction that will enter inside pipeline. 
	//\return notifications (same order than in .config file).
	unsigned int execOneState(const unsigned int extResState, const unsigned int ic);
	//this function init internal data structures.
	void init(arch *);
private:
	//store the current state id.
	unsigned int m_currentState;
	arch *m_arch;
};
#endif
