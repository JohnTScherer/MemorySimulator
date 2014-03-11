// proto.h
// John Scherer

void parse_args( int argc, char ** argv );
void display_usage();
void check_valid_percentage( char * buf );
void print_memory( char * memory );
void exit_processes( char * memory );
void advance_memcode( char * memory );
bool memcode_exists_in_memory( char * memory );
bool at_least_one_available_memcode( char * memory );
void defrag( char * memory );
void set_cells_free( char * memory, int start, int len );
int get_mem_start_pos( char * memory, int p_size, int start );
void new_processes( char * memory );
void get_next_block_sizes( char * memory, int start, int & free_block_size, int & application_block_size );
bool is_fragmented( char * memory );
void defrag_step( char * memory );
int get_num_compact_processes( char * memory );
int get_tot_num_processes( char * memory );
bool sufficient_memory_for_new_process( char * memory, int process_size );
int get_num_free_cells( char * memory );
