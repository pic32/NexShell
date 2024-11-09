/*
    NexOS Kernel Version v1.02.01
    Copyright (c) 2023 brodie

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

#include <stdlib.h>

#include "Pipe.h"
#include "CriticalSection.h"
#include "RTOSConfig.h"

BOOL RAMAddressValid(void* Address)
{
	return TRUE;
}

static BOOL OS_TryPipeWrite(PIPE *Pipe, BYTE **Data, UINT32 BytesToWrite, UINT32 *TotalBytesWritten)
{
	UINT32 BytesWritten = GenericBufferWrite(&Pipe->GenericBuffer, BytesToWrite, *Data);

	*TotalBytesWritten = *TotalBytesWritten + BytesWritten;

	// increment their write pointer
	*Data += BytesWritten;

	return FALSE;
}

static BOOL OS_TryPipeRead(PIPE *Pipe, BYTE **Data, UINT32 *TotalBytesRead, UINT32 BytesToRead, UINT32 *BufferSize)
{
	UINT32 BytesRead = GenericBufferRead(&Pipe->GenericBuffer, BytesToRead, *Data, *BufferSize, FALSE);

	*TotalBytesRead = *TotalBytesRead + BytesRead;

	// increment their write pointer
	*Data += + BytesRead;

	// now update their buffer size since data was read from it.
	*BufferSize -= BytesRead;

	return FALSE;
}

PIPE *CreatePipe(PIPE *Pipe, BYTE *Buffer, UINT32 CapacityInBytes)
{
	PIPE *NewPipe;

    #if (USING_CHECK_PIPE_PARAMETERS == 1)
        if (Pipe != (PIPE*)NULL)
            if (RAMAddressValid((OS_WORD)Pipe) == FALSE)
                return (PIPE*)NULL;

        if (Buffer != (BYTE*)NULL)
            if (RAMAddressValid((OS_WORD)Buffer) == FALSE)
                return (PIPE*)NULL;
    #endif // end of #if (USING_CHECK_PIPE_PARAMETERS == 1)

	// if they did not allocate a pipe, allocate one
	if (Pipe == (PIPE*)NULL)
	{
		NewPipe = (PIPE*)malloc(sizeof(PIPE));

		// the allocation failed
		if (NewPipe == (PIPE*)NULL)
			return (PIPE*)NULL;
	}
	else
	{
		// just point to the pipe
		NewPipe = Pipe;
	}

	// now we know we have a valid pipe handle, lets see about the generic buffer
	if (CreateGenericBuffer(&NewPipe->GenericBuffer, CapacityInBytes, Buffer) == (GENERIC_BUFFER*)NULL)
	{
		// we failed for whatever reason, now we have to free the pipe if it was allocated on the heap
		if (Pipe == (PIPE*)NULL)
			free((void*)NewPipe);

		return (PIPE*)NULL;
	}

	return NewPipe;
}

OS_RESULT PipeWrite(PIPE *Pipe, BYTE *Data, UINT32 BytesToWrite, UINT32 *BytesWritten)
{
	UINT32 BytesWrittenLocal = 0;

    #if (USING_CHECK_PIPE_PARAMETERS == 1)
        if (RAMAddressValid((OS_WORD)Pipe) == FALSE)
            return OS_INVALID_ARGUMENT_ADDRESS;

        if (RAMAddressValid((OS_WORD)Data) == FALSE)
            return OS_INVALID_ARGUMENT_ADDRESS;

        if (RAMAddressValid((OS_WORD)BytesWritten) == FALSE)
            return OS_INVALID_ARGUMENT_ADDRESS;

        if (BytesToWrite == 0)
            return OS_INVALID_ARGUMENT;
    #endif // end of #if (USING_CHECK_PIPE_PARAMETERS == 1)

	BytesWrittenLocal = 0;

	EnterCritical();

    // proceed to write to the pipe
	OS_TryPipeWrite(Pipe, &Data, BytesToWrite - BytesWrittenLocal, &BytesWrittenLocal);

	// this means do not wait for the PIPE to have space
    if (BytesWrittenLocal == BytesToWrite)
	{
		// they do not want to wait for the data to be available
		// we are free to leave now regardless of how many bytes were written
		SurrenderCPU();

		ExitCritical();

		if(BytesWritten != NULL)
			*BytesWritten = BytesWrittenLocal;

		if (BytesWrittenLocal == BytesToWrite)
		{
			return OS_SUCCESS;
		}

		return OS_RESOURCE_INSUFFICIENT_SPACE;
	}

	// otherwise, they either want to delay, or wait forever when the pipe is full
	while (BytesToWrite != BytesWrittenLocal)
	{
		// give up the CPU until the pipe has some data read from it
		SurrenderCPU();

		// proceed to try and write to the PIPE
		OS_TryPipeWrite(Pipe, &Data, BytesToWrite - BytesWrittenLocal, &BytesWrittenLocal);
	}

	SurrenderCPU();

	ExitCritical();

	if (BytesWritten != NULL)
		*BytesWritten = BytesWrittenLocal;

	return OS_SUCCESS;
}

OS_RESULT PipeRead(PIPE *Pipe, BYTE *Data, UINT32 BufferSizeInBytes, UINT32 BytesToRead, UINT32 *BytesRead)
{
	UINT32 BytesReadLocal;

    #if (USING_CHECK_PIPE_PARAMETERS == 1)
        if (RAMAddressValid((OS_WORD)Pipe) == FALSE)
            return OS_INVALID_ARGUMENT_ADDRESS;

        if (RAMAddressValid((OS_WORD)Data) == FALSE)
            return OS_INVALID_ARGUMENT_ADDRESS;

        if (RAMAddressValid((OS_WORD)BytesRead) == FALSE)
            return OS_INVALID_ARGUMENT_ADDRESS;

        if (BytesToRead == 0)
            return OS_INVALID_ARGUMENT;

        if (BufferSizeInBytes < BytesToRead)
            return OS_INVALID_ARGUMENT;
    #endif // end of #if (USING_CHECK_PIPE_PARAMETERS == 1)

	BytesReadLocal = 0;

	EnterCritical();
    
	// proceed to try and read from the PIPE
	OS_TryPipeRead(Pipe, &Data, &BytesReadLocal, (BytesToRead - BytesReadLocal), &BufferSizeInBytes);

	// this means do not wait for the PIPE to have space
    if (*BytesRead == BytesToRead)
	{
		SurrenderCPU();

		// they do not want to wait for the data to be available
		// we are free to leave now regardless of how many bytes were written
		ExitCritical();

		if (BytesRead != NULL)
			*BytesRead = BytesReadLocal;

		if (BytesReadLocal == BytesToRead)
			return OS_SUCCESS;

		return OS_RESOURCE_INSUFFICIENT_DATA;
	}

	// otherwise, they either want to delay, or wait forever when the pipe is full
	while (BytesReadLocal != BytesToRead)
	{
		// give up the CPU until the pipe has some data read from it
		SurrenderCPU();

		// proceed to try and write to the PIPE
		OS_TryPipeRead(Pipe, &Data, &BytesReadLocal, (BytesToRead - BytesReadLocal), &BufferSizeInBytes);
	}

	SurrenderCPU();

	if (BytesRead != NULL)
		*BytesRead = BytesReadLocal;

	ExitCritical();

	return OS_SUCCESS;
}

#if (USING_PIPE_FLUSH_METHOD == 1)
	OS_RESULT PipeFlush(PIPE *Pipe)
	{
		OS_RESULT Result;

		EnterCritical();

		Result = PipeFlushFromISR(Pipe);

		ExitCritical();

		return Result;
	}
#endif // end of #if (USING_PIPE_FLUSH_METHOD == 1)

#if (USING_PIPE_PEEK_METHOD == 1)
	OS_RESULT PipePeek(PIPE *Pipe, BYTE *Data, UINT32 BufferSizeInBytes, UINT32 BytesToRead, UINT32 *BytesRead)
	{
		OS_RESULT Result;

		EnterCritical();

		Result = PipePeekFromISR(Pipe, Data, BufferSizeInBytes, BytesToRead, BytesRead);

		ExitCritical();

		return Result;
	}
#endif // end of #if (USING_PIPE_PEEK_METHOD == 1)

#if (USING_PIPE_CONTAINS_METHOD == 1)
	UINT32 PipeContains(PIPE *Pipe, BYTE Value)
	{
		UINT32 ReturnValue;

		EnterCritical();

		ReturnValue = PipeContainsFromISR(Pipe, Value);

		ExitCritical();

		return ReturnValue;
	}
#endif // end of #if (USING_PIPE_CONTAINS_METHOD == 1)

#if (USING_PIPE_CONTAINS_SEQUENCE_METHOD == 1)
	OS_RESULT PipeContainsSequence(PIPE *Pipe, BYTE *Sequence, UINT32 SequenceSize)
	{
		OS_RESULT Result;

		EnterCritical();

		Result = PipeContainsSequenceFromISR(Pipe, Sequence, SequenceSize);

		ExitCritical();

		return Result;
	}
#endif // end of #if (USING_PIPE_CONTAINS_SEQUENCE_METHOD == 1)

#if (USING_PIPE_SEEK_METHOD == 1)
	UINT32 PipeSeek(PIPE *Pipe, BYTE Value)
	{
		OS_RESULT ReturnValue;

		EnterCritical();

		ReturnValue = PipeSeekFromISR(Pipe, Value);

		ExitCritical();

		return ReturnValue;
	}
#endif // end of #if (USING_PIPE_SEEK_METHOD == 1)

#if (USING_PIPE_GET_SIZE_METHOD == 1)
	UINT32 PipeGetSize(PIPE *Pipe)
	{
		OS_RESULT ReturnValue;

		EnterCritical();

		ReturnValue = PipeGetSizeFromISR(Pipe);

		ExitCritical();

		return ReturnValue;
	}
#endif // end of #if (USING_PIPE_GET_SIZE_METHOD == 1)

#if (USING_PIPE_GET_CAPACITY_METHOD == 1)
	UINT32 PipeGetCapacity(PIPE *Pipe)
	{
		UINT32 ReturnValue;

		EnterCritical();

		ReturnValue = PipeGetCapacityFromISR(Pipe);

		ExitCritical();

		return ReturnValue;
	}
#endif // end of #if (USING_PIPE_GET_CAPACITY_METHOD == 1)

#if (USING_PIPE_GET_REMAINING_BYTES_METHOD == 1)
	UINT32 PipeGetRemainingBytes(PIPE *Pipe)
	{
		UINT32 ReturnValue;

		EnterCritical();

		ReturnValue = PipeGetRemainingBytesFromISR(Pipe);

		ExitCritical();

		return ReturnValue;
	}
#endif // end of #if (USING_PIPE_GET_REMAINING_BYTES_METHOD == 1)

#if (USING_PIPE_DELETE_METHOD == 1)
	OS_RESULT PipeDelete(PIPE *Pipe, BOOL FreeBufferSpace)
	{
		OS_RESULT Result;

		EnterCritical();

		Result = PipeDeleteFromISR(Pipe, FreeBufferSpace);

		ExitCritical();

		return Result;
	}
#endif // end of #if (USING_PIPE_DELETE_METHOD == 1)

OS_RESULT PipeWriteFromISR(PIPE *Pipe, BYTE *Data, UINT32 BytesToWrite, UINT32 *BytesWritten, BOOL *HigherPriorityTask)
{
    #if (USING_CHECK_PIPE_PARAMETERS == 1)
        if (RAMAddressValid((OS_WORD)Pipe) == FALSE)
            return OS_INVALID_ARGUMENT_ADDRESS;

        if (RAMAddressValid((OS_WORD)Data) == FALSE)
            return OS_INVALID_ARGUMENT_ADDRESS;

        if (RAMAddressValid((OS_WORD)HigherPriorityTask) == FALSE)
            return OS_INVALID_ARGUMENT_ADDRESS;

        if (RAMAddressValid((OS_WORD)BytesWritten) == FALSE)
            return OS_INVALID_ARGUMENT_ADDRESS;

        if (BytesToWrite == 0)
            return OS_INVALID_ARGUMENT;
    #endif // end of #if (USING_CHECK_PIPE_PARAMETERS == 1)

	*BytesWritten = 0;

	// proceed to try and write to the PIPE
	*HigherPriorityTask = OS_TryPipeWrite(Pipe, &Data, BytesToWrite, BytesWritten);

	if (*BytesWritten == BytesToWrite)
		return OS_SUCCESS;

	if (*BytesWritten != 0)
		return OS_RESOURCE_INSUFFICIENT_SPACE;

	return OS_RESOURCE_FULL;
}

OS_RESULT PipeReadFromISR(PIPE *Pipe, BYTE *Data, UINT32 BufferSizeInBytes, UINT32 BytesToRead, UINT32 *BytesRead, BOOL *HigherPriorityTask)
{
    #if (USING_CHECK_PIPE_PARAMETERS == 1)
        if (RAMAddressValid((OS_WORD)Pipe) == FALSE)
            return OS_INVALID_ARGUMENT_ADDRESS;

        if (RAMAddressValid((OS_WORD)Data) == FALSE)
            return OS_INVALID_ARGUMENT_ADDRESS;

        if (RAMAddressValid((OS_WORD)HigherPriorityTask) == FALSE)
            return OS_INVALID_ARGUMENT_ADDRESS;

        if (RAMAddressValid((OS_WORD)BytesRead) == FALSE)
            return OS_INVALID_ARGUMENT_ADDRESS;

        if (BytesToRead == 0)
            return OS_INVALID_ARGUMENT;
    #endif // end of #if (USING_CHECK_PIPE_PARAMETERS == 1)

	*BytesRead = 0;

	// proceed to try and write to the PIPE
	*HigherPriorityTask = OS_TryPipeRead(Pipe, &Data, BytesRead, BytesToRead, &BufferSizeInBytes);

	if (*BytesRead == BytesToRead)
		return OS_SUCCESS;

	if (*BytesRead != 0)
		return OS_RESOURCE_INSUFFICIENT_DATA;

	return OS_RESOURCE_FULL;
}

#if (USING_PIPE_FLUSH_FROM_ISR_METHOD == 1)
	OS_RESULT PipeFlushFromISR(PIPE *Pipe)
	{
		BOOL ClearBlockedList;

        #if (USING_CHECK_PIPE_PARAMETERS == 1)
            if (RAMAddressValid((OS_WORD)Pipe) == FALSE)
                return OS_INVALID_ARGUMENT_ADDRESS;
        #endif // end of #if (USING_CHECK_PIPE_PARAMETERS == 1)

		// is there any data in the PIPE prior to the flush?
		if (GenericBufferGetSize(&Pipe->GenericBuffer) != 0)
			ClearBlockedList = TRUE;
		else
			ClearBlockedList = FALSE;

		// now perform the actual flushing
		if (GenericBufferFlush(&Pipe->GenericBuffer) == FALSE)
			return OS_INVALID_OBJECT_USED;

		// if there was data in there, we will say that any blocking TASK should run
		// assuming that they were trying to write.
		//if (ClearBlockedList == TRUE)
			//if (OS_AddTaskListToReadyQueue(&Pipe->PipeBlockedListHead) == TRUE)
				SurrenderCPU();

		return OS_SUCCESS;
	}
#endif // end of #if (USING_PIPE_FLUSH_FROM_ISR_METHOD == 1)

#if (USING_PIPE_PEEK_FROM_ISR_METHOD == 1)
	OS_RESULT PipePeekFromISR(PIPE *Pipe, BYTE *Data, UINT32 BufferSizeInBytes, UINT32 BytesToRead, UINT32 *BytesRead)
	{
        #if (USING_CHECK_PIPE_PARAMETERS == 1)
            if (RAMAddressValid((OS_WORD)Pipe) == FALSE)
                return OS_INVALID_ARGUMENT_ADDRESS;

            if (RAMAddressValid((OS_WORD)Data) == FALSE)
                return OS_INVALID_ARGUMENT_ADDRESS;

            if (RAMAddressValid((OS_WORD)BytesRead) == FALSE)
                return OS_INVALID_ARGUMENT_ADDRESS;

            if (BytesToRead == 0)
                return OS_INVALID_ARGUMENT;
        #endif // end of #if (USING_CHECK_PIPE_PARAMETERS == 1)

		*BytesRead = 0;

		// proceed to peek at the buffer
		*BytesRead = GenericBufferPeek(&Pipe->GenericBuffer, 0, BytesToRead, Data, BufferSizeInBytes, FALSE);

		if (*BytesRead == BytesToRead)
			return OS_SUCCESS;

		if (*BytesRead != 0)
			return OS_RESOURCE_INSUFFICIENT_DATA;

		return OS_RESOURCE_FULL;
	}
#endif // end of #if (USING_PIPE_PEEK_FROM_ISR_METHOD == 1)

#if (USING_PIPE_CONTAINS_FROM_ISR_METHOD == 1)
	UINT32 PipeContainsFromISR(PIPE *Pipe, BYTE Value)
	{
        #if (USING_CHECK_PIPE_PARAMETERS == 1)
            if (RAMAddressValid((OS_WORD)Pipe) == FALSE)
                return INVALID_PIPE_SIZE;
        #endif // end of #if (USING_CHECK_PIPE_PARAMETERS == 1)

        return GenericBufferContains(&Pipe->GenericBuffer, Value);
	}
#endif // end of #if (USING_PIPE_CONTAINS_FROM_ISR_METHOD == 1)

#if (USING_PIPE_CONTAINS_SEQUENCE_FROM_ISR_METHOD == 1)
	OS_RESULT PipeContainsSequenceFromISR(PIPE *Pipe, BYTE *Sequence, UINT32 SequenceSize)
	{
        #if (USING_CHECK_PIPE_PARAMETERS == 1)
            if (RAMAddressValid((OS_WORD)Pipe) == FALSE)
                return OS_INVALID_ARGUMENT_ADDRESS;

            if (RAMAddressValid((OS_WORD)Sequence) == FALSE)
                return OS_INVALID_ARGUMENT_ADDRESS;

            if (SequenceSize == 0)
                return OS_INVALID_ARGUMENT;
        #endif // end of #if (USING_CHECK_PIPE_PARAMETERS == 1)

		if (GenericBufferContainsSequence(&Pipe->GenericBuffer, Sequence, SequenceSize, NULL) != TRUE)
			return OS_INVALID_OBJECT_USED;

		return OS_SUCCESS;
	}
#endif // end of #if (USING_PIPE_CONTAINS_SEQUENCE_FROM_ISR_METHOD == 1)

#if (USING_PIPE_SEEK_FROM_ISR_METHOD == 1)
	UINT32 PipeSeekFromISR(PIPE *Pipe, BYTE Value)
	{
        #if (USING_CHECK_PIPE_PARAMETERS == 1)
            if (RAMAddressValid((OS_WORD)Pipe) == FALSE)
                return INVALID_PIPE_SIZE;
        #endif // end of #if (USING_CHECK_PIPE_PARAMETERS == 1)

		// get the answer of how many bytes deep Value is
		return GenericBufferSeek(&Pipe->GenericBuffer, Value);
	}
#endif // end of #if (USING_PIPE_SEEK_FROM_ISR_METHOD == 1)

#if (USING_PIPE_GET_SIZE_FROM_ISR_METHOD == 1)
	UINT32 PipeGetSizeFromISR(PIPE *Pipe)
	{
        #if (USING_CHECK_PIPE_PARAMETERS == 1)
            if (RAMAddressValid((OS_WORD)Pipe) == FALSE)
                return INVALID_PIPE_SIZE;
        #endif // end of #if (USING_CHECK_PIPE_PARAMETERS == 1)

		return GenericBufferGetSize(&Pipe->GenericBuffer);
	}
#endif // end of #if (USING_PIPE_GET_SIZE_FROM_ISR_METHOD == 1)

#if (USING_PIPE_GET_CAPACITY_FROM_ISR_METHOD == 1)
	UINT32 PipeGetCapacityFromISR(PIPE *Pipe)
	{
        #if (USING_CHECK_PIPE_PARAMETERS == 1)
            if (RAMAddressValid((OS_WORD)Pipe) == FALSE)
                return INVALID_PIPE_SIZE;
        #endif // end of #if (USING_CHECK_PIPE_PARAMETERS == 1)

		return GenericBufferGetCapacity(&Pipe->GenericBuffer);
	}
#endif // end of #if (USING_PIPE_GET_CAPACITY_FROM_ISR_METHOD == 1)

#if (USING_PIPE_GET_REMAINING_BYTES_FROM_ISR_METHOD == 1)
	UINT32 PipeGetRemainingBytesFromISR(PIPE *Pipe)
	{
        #if (USING_CHECK_PIPE_PARAMETERS == 1)
            if (RAMAddressValid((OS_WORD)Pipe) == FALSE)
                return INVALID_PIPE_SIZE;
        #endif // end of #if (USING_CHECK_PIPE_PARAMETERS == 1)

		return GenericBufferGetRemainingBytes(&Pipe->GenericBuffer);
	}
#endif // end of #if (USING_PIPE_GET_REMAINING_BYTES_FROM_ISR_METHOD == 1)

#if (USING_PIPE_DELETE_FROM_ISR_METHOD == 1)
	OS_RESULT PipeDeleteFromISR(PIPE *Pipe, BOOL FreeBufferSpace)
	{
		GENERIC_BUFFER *GenericBuffer;

        #if (USING_CHECK_PIPE_PARAMETERS == 1)
            // are we in the heap?
            //if (AddressInHeap((OS_WORD)Pipe) == FALSE)
                //return OS_RESOURCE_NOT_IN_OS_HEAP;
        #endif // end of #if (USING_CHECK_PIPE_PARAMETERS == 1)
        
        // is anyone using it?
        //if(Pipe->DeleteBlockCount != 0)
            //return OS_RESOURCE_IN_USE;

		// lets get a handle to the buffer first because we will
		// be deleting the PIPE first to see if it was successful
		GenericBuffer = &Pipe->GenericBuffer;

        // now release the space back to the OS
		free((void*)Pipe);
			//return OS_RESOURCE_NOT_IN_OS_HEAP;

		// the PIPE was in the heap, so lets try and free the GENERIC_BUFFER
		if (GenericBufferDelete(GenericBuffer, FreeBufferSpace) == FALSE)
			return OS_INVALID_OBJECT_USED;

		return OS_SUCCESS;
	}
#endif // end of #if (USING_PIPE_DELETE_FROM_ISR_METHOD == 1)
