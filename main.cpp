// main.cpp
// John Scherer

#include "_aux.h"
#include "proto.h"

#define MEMSIZE 2400

using namespace std;

// some global variables
int p_term_prob, p_new_prob; // process-termination-probability, new-process-probability
char alloc_algorithm; // 'f' = first, 'b' = best, 'n' = next, 'w' = worst, 'y' = non-contiguous
int memcell; // holds current memory cell location
char memcode; // holds the current memcode in the range [A-Za-z]
double percent_free; // defrag statistics
int num_moved_processes; // defrag statistics
int num_free_cells; // defrag statistics
bool was_just_defragged;

int main( int argc, char ** argv )
{
    cout << "welcome to memsim\n";
	parse_args( argc, argv );
	
	char memory[MEMSIZE]; // this char array holds main memory
	srand( time( NULL ) ); // seed the random number generator
	
	// initialize all memory locations to free
	for ( int i = 0; i < MEMSIZE; ++i )
		memory[i] = '.';
	
	// fill memory locations 0-79 with operating system processes
	for ( int i = 0; i < 80; ++i )
		memory[i] = '#';
	
	// assign 20 user processes to memory
	memcode = 'A';
	memcell = 79;
	for ( int i = 0; i < 20; ++i )
	{
		int p_mem_size = 10 + rand() % 91; // each process gets a random amount of memory from 10 to 100 cells
		
		// check for valid memcode
		
		// fill memory for this process
		for ( int j = 0; j < p_mem_size; ++j )
			memory[++memcell] = memcode;

		advance_memcode( memory );
	}
	
	print_memory( memory );
	
	// begin user-controlled loop
	while ( 1 )
	{
		string user_input;
		cout << "enter a command: ";
		cin >> user_input;
		bool suppress_print = false;
		if ( user_input == "c" )
		{
			// continue
			exit_processes( memory );
			was_just_defragged = false;
			new_processes( memory );
		}
		else if ( user_input == "q" )
		{
			// quit
			cout << "quitting simulation..." << endl;
			exit( 0 );
		}
		else if ( user_input == "d" )
		{
			defrag( memory );
		}
/***************************************************
		else if ( user_input == "z" )
		{
			cout << "# compact processes = " << get_num_compact_processes( memory ) << endl;
		}
 *****************************************************/
		else if ( user_input == "i" )
		{
			if ( is_fragmented( memory ) )
				cout << "---memory is fragmented" << endl;
			else
				cout << "---NOT fragmented" << endl;
		}
 		else
		{
			// invalid user input
			cout << "you entered something invalid" << endl;
			suppress_print = true;
		}
		
		if ( was_just_defragged )
		{
			// print defrag statistics
			cout << "Defragmentation Completed." << endl;
			cout << "Relocated " << num_moved_processes << " processes to create free memory block of "
			<< num_free_cells << " units (";
			printf( "%.2f", percent_free );
			cout << "% of total memory)." << endl;
		}
		
		// print memory map at each iteration in user loop
		if ( ! suppress_print )
			print_memory( memory );
		
	}
	
    return 0;
}

// parse the program args and check that they are valid
// if invalid args are detected, display program usage and exit
void parse_args( int argc, char ** argv )
{
	alloc_algorithm = '-';
	if ( argc != 4 )
	{
		// wrong number of arguments, display usage
		display_usage();
	}
	else
	{
		if ( 0 == strcmp( argv[1], "first" ) )
			alloc_algorithm = 'f';
		else if ( 0 == strcmp( argv[1], "best" ) )
			alloc_algorithm = 'b';
		else if ( 0 == strcmp( argv[1], "next" ) )
			alloc_algorithm = 'n';
		else if ( 0 == strcmp( argv[1], "worst" ) )
			alloc_algorithm = 'w';
		else if ( 0 == strcmp( argv[1], "noncontiguous" ) )
			alloc_algorithm = 'y';
		
		if ( alloc_algorithm == '-' )
			display_usage();
		
		// at this point, we have already verified that the second argument is valid
		// now, let's check that arguments (3) and (4) are integers from 0 to 99
		check_valid_percentage( argv[2] );
		check_valid_percentage( argv[3] );
		p_term_prob = atoi( argv[2] );
		p_new_prob = atoi( argv[3] );
	}
}

// displays program usage, indicating the number and nature of the expected program args
void display_usage()
{
	cout << "USAGE: memsim  { noncontiguous | first | best | next | worst }  <process-termination-probability> <new-process-probability>" << endl;
	exit( 0 );
}

// given a char pointer, determines if it is a valid percentage (an integer from 0 to 100)
// if it is not a valid percentage, program usage is displayed, and the program exits
void check_valid_percentage( char * buf )
{
	for ( int i = 0; i < (int) strlen( buf ); ++i )
	{
		if ( ((int) buf[i]) >= 48 && ((int) buf[i]) <= 57 )
			continue;
		display_usage();
	}
	int temp = atoi( buf );
	if ( (temp > 100) || (temp < 0) )
		display_usage();
}

// pretty print the current system memory to stdout
void print_memory( char * memory )
{
	cout << "------BEGIN CURRENT MEMORY MAP------" << endl;
	for ( int i = 0; i < MEMSIZE / 80; ++i )
	{
		string line( &(memory[80 * i]), 80 );
		cout << line << endl;
	}
	cout << "-------END CURRENT MEMORY MAP-------" << endl;
}

// exit existing processes at rate according to p_term_prob percentage
void exit_processes( char * memory )
{
	char last_memcode = '-';
	// discover all current processes
	for ( int i = 80; i < MEMSIZE; ++i )
	{
		if ( memory[i] == last_memcode )
			continue;
		else if ( memory[i] == '.' )
			continue;
		
		// discovered a current process
		last_memcode = memory[i];
		// p_term_prob gives probability of a new process being spawned
		int x = rand() % 100;
		if ( x >= p_term_prob )
			continue; // this is the event that a current process is not exited, due to the outcome of the random sampling
		
		// random sampling yeilds exiting of discovered process
		int cur_cell = i;
		while ( memory[cur_cell] == last_memcode )
		{
			// current memory cell belongs to the process to be exited -- free it
//			cout << memory[cur_cell] << " -> '.' (" << cur_cell << ")" << endl;
			memory[cur_cell++] = '.';
		}
		cout << "Process exited, memcode = " << last_memcode << ", p_mem_size = " << (cur_cell - i) << endl;
		i = cur_cell - 1;
	}
	
}


// advance memcode to next valid ASCII value
void advance_memcode( char * memory )
{
	// choose next ASCII value in range [A-Za-z]
	if ( memcode == 'Z' )
		memcode = 'a'; // reached ASCII gap between 'Z' and 'a', skip to 'a'
	else if ( memcode == 'z' )
		memcode = 'A'; // reached end of memcodes, loop back to 'A'
	else
		++memcode; // continue to next adjacent memcode
		
	// ensure that memcode is not already used somewhere in memory
	if ( memcode_exists_in_memory(memory) )
		advance_memcode( memory );
}

// ensure that memcode is not already used somewhere in memory
// return true if memcode is used, else return false
bool memcode_exists_in_memory( char * memory )
{
	// inspect each application memory element and accumulate histogram
	for ( int i = 80; i < MEMSIZE; ++i )
		if ( memory[i] == memcode )
			return true;
	
	return false;
}

// scans memory for available memcodes
// return true if at least one memcode is availabe
// if no available memcodes exist, display an error message and quit the simulation
bool at_least_one_available_memcode( char * memory )
{
	// allocate histogram where elements 0-25 are A-Z, resp. and 26-51 are a-z, resp.
	// this way, we can keep track of which memcodes are used, and how many cells each one occupies
	int memcode_histogram[52];
	for ( int i = 0; i < 52; ++i )
		memcode_histogram[i] = 0;
	
	// inspect each application memory element and accumulate histogram
	for ( int i = 80; i < MEMSIZE; ++i )
	{
		if ( (int) memory[i] >= 65 && (int) memory[i] <= 90 )
		{
			// memcode is one of [A-Z]
			++memcode_histogram[ memory[i] - 65 ];
		}
		else if ( (int) memory[i] >= 97 && (int) memory[i] <= 122 )
		{
			// memcode is one of [a-z]
			++memcode_histogram[ 26 + memory[i] - 97 ];
		}
	}
	
	// find the first available memcode, if one exists
	for ( int i = 0; i < 52; ++i )
		if ( memcode_histogram[i] == 0 )
			return true;
	
	// no available memcodes, return false
	return false;
}

// starting at memory cell denoted by start, set n cells free, where n = len
void set_cells_free( char * memory, int start, int len )
{
	cout << "set_free_cells(): start = " << start << " len = " << len << endl;
	for ( int i = start; i < (start + len); ++i )
		memory[i] = '.';
}

void get_next_block_sizes( char * memory, int start, int & free_block_size, int & application_block_size )
{
	bool reading_active_cells = false;
	for ( int j = start; j < MEMSIZE; ++j )
	{
		if ( memory[j] == '.' )
		{
			if ( reading_active_cells )
			{
				// already finished reading free block
				// now, we just discovered the end of the application block
				return;
			}
			else
			{
				// still reading the first free block
				++free_block_size;
			}
		}
		else
		{
			// reached next application memory block
			reading_active_cells = true;
			++application_block_size;
		}
	}
}

// scans memory for fragmentation
bool is_fragmented( char * memory )
{
	bool empty = false;
	bool process_detected = false;
	bool free_after_processes = false;
	if ( memory[80] == '.' )
		empty = true;
	for ( int i = 80; i < MEMSIZE; ++i )
	{
		if ( memory[i] != '.' )
		{
			if ( empty )
				return true;
			else if ( free_after_processes )
				return true;
			else
				process_detected = true;
		}
		else
		{
			if ( process_detected )
				free_after_processes = true;
		}
	}
	return false;
}

// create new processes at rate according to p_new_prob percentage
void new_processes( char * memory )
{
	int count = 0;
	// check for available memcode
	if ( ! at_least_one_available_memcode(memory) )
	{
		// no available memcodes exist, display an error message and quit
		cerr << "ERROR: no available memcodes in the range [A-Za-z]! Quitting simulation..." << endl;
		exit( 0 );
	}
	
	// p_new_prob gives probability of a new process being spawned
	int x = rand() % 100;
	if ( x >= p_new_prob )
		return; // this is the event that a new process is not created, due to the outcome of the random sampling
	
	// create a new process
	int p_mem_size = 10 + rand() % 91; // each process gets a random amount of memory from 10 to 100 cells
	
	
	// check for sufficient memory to load new process
	if ( ! sufficient_memory_for_new_process(memory, p_mem_size) )
	{
		// perform defragmentation
		int num_compact_processes_in_memory = get_num_compact_processes( memory );
		cout << "Performing defragmentation." << endl << "..." << endl;
		defrag( memory );
		was_just_defragged = true;
		int num_total_processes_in_memory = get_tot_num_processes( memory );
		num_moved_processes = num_total_processes_in_memory - num_compact_processes_in_memory;
		num_free_cells = get_num_free_cells( memory );
		percent_free = ((double) 100) * ((double) num_free_cells) / ((double) MEMSIZE);
		
		if ( ! sufficient_memory_for_new_process(memory, p_mem_size) )
		{
			// after defragmentation, still not enough memory, quit
			cerr << "ERROR: OUT-OUT-MEMORY" << endl;
			exit( 0 );
		}
	}

	
	memcell = get_mem_start_pos( memory, p_mem_size, memcell );
	
	if ( memcell == -1 )
	{
		memcell = get_mem_start_pos( memory, p_mem_size, 80 );
		if ( memcell == -1 )
		{
			// perform defragmentation
			int num_compact_processes_in_memory = get_num_compact_processes( memory );
			cout << "Performing defragmentation." << endl << "..." << endl;
			defrag( memory );
			was_just_defragged = true;
			int num_total_processes_in_memory = get_tot_num_processes( memory );
			num_moved_processes = num_total_processes_in_memory - num_compact_processes_in_memory;
			num_free_cells = get_num_free_cells( memory );
			percent_free = ((double) 100) * ((double) num_free_cells) / ((double) MEMSIZE);
			
			if ( ! sufficient_memory_for_new_process(memory, p_mem_size) )
			{
				// after defragmentation, still not enough memory, quit
				cerr << "ERROR: OUT-OUT-MEMORY" << endl;
				exit( 0 );
			}
		}
	}
	memcell = get_mem_start_pos( memory, p_mem_size, 80 );
	
	cout << "New process to be created, memcode = " << memcode << ", p_mem_size = " << p_mem_size << endl;
	
	// fill memory for this process
	if( alloc_algorithm == 'y' )
	{
	    while( count < p_mem_size )	  
		{
			if(memory[memcell] == '.')
			{
				memory[memcell] = memcode;
				count++;
			}
			memcell++;
		}
	}
	else{
		for ( int j = 0; j < p_mem_size; ++j )
			memory[memcell++] = memcode;
	}
	advance_memcode( memory );
}