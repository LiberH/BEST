#include "timingSignal.h"


// 0 -> 2 => ICache.fillBuffer->ICacheLineBuffer.fillBuffer 
// 1 -> 1 => ICacheLineBuffer.bufferData->ICache.bufferData 
// 2 -> 4 => ICacheLineBuffer.memStart->MemDev.memStart 
// 3 -> 3 => MemDev.memEnd->ICacheLineBuffer.memEnd 
const unsigned int signalLinksTo[NB_SIGNAL_EMIT] = {2, 1, 4, 3};
const unsigned int signalLinksFrom[NB_SIGNAL_WAIT] = {0, 1, 0, 3, 2};
