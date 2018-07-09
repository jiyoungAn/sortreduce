#include "types.h"

template <class K, class V>
SortReduceTypes::Config<K,V>::Config(std::string temporary_directory, std::string output_filename) {
	this->temporary_directory = temporary_directory;
	this->output_filename = output_filename;

	this->maximum_threads = 4;
	this->update = NULL;
	
	this->buffer_size = 0;
	this->buffer_count = 0;
	this->max_bytes_inflight = 0;
}

template <class K, class V>
void 
SortReduceTypes::Config<K,V>::SetManagedBufferSize(size_t buffer_size, int buffer_count) {
	this->buffer_size = buffer_size;
	this->buffer_count = buffer_count;
}

template <class K, class V>
void 
SortReduceTypes::Config<K,V>::SetMaxBytesInFlight(size_t buffer_size) {
	this->max_bytes_inflight = buffer_size;
}

template <class K, class V>
void
SortReduceTypes::Config<K,V>::SetUpdateFunction(V (*update)(V,V)) {
	this->update = update;
}

/*
template <class K, class V>
void
SortReduceTypes::Config<K,V>::SetUpdateFunction(uint64_t (*update64)(uint64_t,uint64_t)) {
	this->update64 = update64;
}
*/

SortReduceTypes::Status::Status() {
	this->done_input = false;
	this->done_inmem = false;
	this->done_external = false;
	this->done_file = NULL;
}

template class SortReduceTypes::Config<uint32_t,uint32_t>;
template class SortReduceTypes::Config<uint32_t,uint64_t>;
template class SortReduceTypes::Config<uint64_t,uint32_t>;
template class SortReduceTypes::Config<uint64_t,uint64_t>;