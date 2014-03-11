// aux.cpp
// John Scherer

#include "_aux.h"
#include <iostream>

#define MEMSIZE 2400

// determine the correct memory cell to begin allocating for the new process
int get_mem_start_pos( char * memory, int p_size, int start )
{
  bool avail = false;
  int counter = 0, j = 0, excess = INT_MAX;
  int start_pos = -1;
  if ( alloc_algorithm == 'f' )//********FIRST********//
    {
      for( int i=80; i<MEMSIZE; i++ )
	{
	  
	  if( memory[i] == '.' )//if its an empty process
	    {
	      for(j=0; j<p_size; j++)//search this process up till the size of the new process
		{
		  if( memory[i+j] == '.' )  counter++;//increment counter if process is still free
		  
		  if(counter == p_size)//if the amount of consecutive processes is the size of the new process 
		    {
		      avail = true;//bool to check if the space is available
		      start_pos = i;//sets the start position to the position of the first '.'
		    }
		}
	    }
	  if(avail == true) break;//if the first spot large enough to hold the new process is found
	  counter = 0;
	  i = i+j;
	}
      
      if(avail == false)//didn't find a space big enough
	return -1;
      else
	return start_pos;
      
    }
  else if ( alloc_algorithm == 'b' )//*******BEST********//
    {
      for( int i=80; i<MEMSIZE; i++ )
	{
	  j = 0;
	  counter = 0;
	  avail = false;
	  while(memory[i+j] == '.')
	    {
	      counter++;
	      if(counter == p_size)//this empty memory space has enough to store the new process
		{
		  avail = true;
		}
	      j++;
	    }
	  if(avail == true)//a spot in memory has been found
	    {//LOW WATER MARK
	      if(counter < excess){//if it has less than the previously smallest memory space
		excess = counter;//mark this the new best size
		start_pos = i;//i is the new best starting postion
	      }
	    }
	  i = i+j;//increment i by however long the empty memory space was, 0 if none was found
	}

      return start_pos;
    }
  else if ( alloc_algorithm == 'n' )//********NEXT********//
    {
      for(int i=start; i<MEMSIZE; i++)
	{
	  if( memory[i] == '.' )//if we reach an empty memory loc
	    {
	      for(j=0; j<p_size; j++)//for the size of the new process
		{
		  if( memory[i+j] == '.' )  counter++;//incremnt this counter every time there is an open space
		  
		  if(counter == p_size)//if there is enough space to fill with new process
		    {
		      avail = true;//flag for the space being good
		      start_pos = i;// setting the initial position == i
		    }
		}
	    }
	  if(avail == true) break;
	  counter = 0;
	  i = i+j;
	}
      if(avail == true) return start_pos;
      
      for(int i=80; i<start; i++)
	{
	  j=0;
	  counter = 0;
	  while(memory[i+j] == '.')//for as long as there is an empty memory space
		{
		  counter++;	  //increment this counter
		  if(counter == p_size)// if/when it reaches the size of the new process
		    {
		      avail = true;//mark flag for avail space
		      start_pos = i;// start position for new process was the initial 'i'
		    }
		  j++;
		}
	  if(avail == true) break;
	  counter = 0;
	  i = i+j;
	}
      if(avail == true) return start_pos;
      else
	return -1;	  
    }
  else if ( alloc_algorithm == 'w' )//**********WORST**********//
    {
      excess = 0;
      for( int i=80; i<MEMSIZE; i++ )
	{
	  j = 0;
	  counter = 0;
	  avail = false;
	  while(memory[i+j] == '.')//for as long as there is an empty memory space
	    {
	      counter++;
	      if(counter == p_size)//this empty memory space has enough to store the new process
		{
		  avail = true;
		}
	      j++;
	    }
	  if(avail == true)
	    {//HIGH WATER MARK
	      if(counter > excess){//if the current set of memory has more available then the last
		excess = counter;//set this as the new available worst mem space
		start_pos = i;
	      }
	    }
	  i = i+j;
	}
      
      return start_pos;
    }
  else if ( alloc_algorithm == 'y' ){//******Non-CONTIGUOUS*************//
    for( int i=80; i<MEMSIZE; i++ )
      {
	if(memory[i] == '.'){
	  avail = true;
	  return i;
	}
      }
    if(avail == false) return -1;
  }
	return -1;
}

// this function performs defragmentation on the current memory map
void defrag_step( char * memory )
{
	int free_block_size, application_block_size;
	for ( int i = 80; i < MEMSIZE; ++i )
	{
		if ( memory[i] == '.' )
		{
			// encountered a free memory cell
			
			// get length of free block
			free_block_size = 0;
			application_block_size = 0;
			get_next_block_sizes( memory, i, free_block_size, application_block_size );
			
			// move the application block back to the the free block region
			for ( int j = i; j < (i + application_block_size); ++j )
			{
				//				cout << j << "(" << memory[j] << ") = " << j + free_block_size  << "(" << memory[j + free_block_size] << ")" << endl;
				memory[j] = memory[j + free_block_size];
			}
			
			// set stale application memory to free memory
			int j;
			if ( free_block_size > application_block_size )
				j = i + free_block_size;
			else
				j = i + application_block_size;
			//			cout << "freeing stale app memory, starting at " << j << " (" << memory[j] << ")" << endl;
			//			cout << "--- or should I start here: " << i + free_block_size + application_block_size
			//			<< "(" << memory[i + free_block_size + application_block_size] << ") ???" << endl;
			while ( memory[j] != '.' )
			{
				memory[j] = '.';
				++j;
			}
			return;
		}
	}
}

void defrag( char * memory )
{
  while(is_fragmented( memory ))
    {
      defrag_step( memory );
    }

}

//gets the number of processes that didn't need to be compressed
int get_num_compact_processes( char * memory )
{
  int compactProcesses = 0;
  char currentProcess, lastProcess;
  lastProcess = memory[79];
  for(int i=80; i<MEMSIZE; i++)
    {
      currentProcess = memory[i];
      if(currentProcess == '.') return compactProcesses;
      else if(lastProcess != currentProcess)
	{
	  lastProcess = currentProcess;
	  compactProcesses++;
	}
    }
	return -1;
}

//gets the total number of processes that should be compact at the beginning of memory
int get_tot_num_processes( char * memory )
{
  int numProcesses = 0;
  char currentProcess, lastProcess;
  lastProcess = memory[79];
   for(int i=80; i<MEMSIZE; i++)
    {
      currentProcess = memory[i];
      if(currentProcess == '.') return numProcesses;
      else if(lastProcess != currentProcess){
	lastProcess = currentProcess;
	numProcesses++;
      }
    }
	return -1;
}

bool sufficient_memory_for_new_process( char * memory, int process_size )
{
  int temp, j;
  for( int i=80; i<MEMSIZE; i++ )
    {
      j=0;
      temp=0;
      while(memory[i+j] == '.'){
	temp++;
	if(temp == process_size) return true;
	j++;
      }
      i=i+j;
    }
  return false;
}

int get_num_free_cells( char * memory )
{
  int freeCells = 0;
   for( int i=80; i<MEMSIZE; i++ )
      if(memory[i] == '.') freeCells++;
   return freeCells;
}
