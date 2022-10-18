#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

#include <atomic>
#include <queue>
#include <string>
#include <thread>
#include <iostream>
#include <bitset>
#ifdef KVTYPES1
#define TEMPLATE_EXPLICIT_INSTANTIATION1(X) template class X<KVTYPES1>;
#else 
#define TEMPLATE_EXPLICIT_INSTANTIATION1(X) ;
#endif

#ifdef KVTYPES2
#define TEMPLATE_EXPLICIT_INSTANTIATION2(X) template class X<KVTYPES2>;
#else 
#define TEMPLATE_EXPLICIT_INSTANTIATION2(X) 
#endif

#ifdef KVTYPES3
#define TEMPLATE_EXPLICIT_INSTANTIATION3(X) template class X<KVTYPES3>;
#else 
#define TEMPLATE_EXPLICIT_INSTANTIATION3(X) 
#endif


#ifdef KVTYPES4
#define TEMPLATE_EXPLICIT_INSTANTIATION4(X) template class X<KVTYPES4>;
#else 
#define TEMPLATE_EXPLICIT_INSTANTIATION4(X) 
#endif

#define TEMPLATE_EXPLICIT_INSTANTIATION(X) \
TEMPLATE_EXPLICIT_INSTANTIATION1(X) \
TEMPLATE_EXPLICIT_INSTANTIATION2(X) \
TEMPLATE_EXPLICIT_INSTANTIATION3(X) \
TEMPLATE_EXPLICIT_INSTANTIATION4(X)


namespace SortReduceTypes {
	typedef struct {
		void* buffer = NULL;
		size_t bytes = 0;
		size_t valid_bytes = 0;

		// Buffers are allocated in the library and reused, instead of free'd
		int managed_idx = -1;
		bool managed = false;

		//when used, do not use buffer and bytes
		bool last = false;

		// Used to indicate null values (e.g., return from empty queue)
		// Blocks with no data should be indicated with bytes=0
		bool valid = false;
	} Block;

	class File {
	public:
		int fd;
		size_t bytes;
		std::string path;
	};
	class CompareFileSize {
	public:
		bool operator() (File* a, File* b) {
			return (a->bytes > b->bytes); // This ordering feels wrong, but this is correct

		}
	};

	typedef struct {
		std::atomic<size_t> bytes_inflight;
	} ComponentStatus;


	template <class K, class V>
	class Config {
	public:
		Config(std::string temporary_directory, std::string output_filename = "", int max_threads = -1);
		void SetUpdateFunction(V (*update)(V,V) );
		//void SetUpdateFunction(uint64_t (*update64)(uint64_t,uint64_t) );
		void SetManagedBufferSize(size_t buffer_size, int buffer_count);
		void SetMaxBytesInFlight(size_t bytes);
		void SetMaxStorageBytes(size_t bytes) {max_storage_allocatd_bytes = bytes;};
	//private:

		// If not set, file is not created, and can only be read via "Next"
		std::string temporary_directory;
		std::string output_filename;

		int maximum_threads;

		V (*update)(V,V);

		size_t buffer_size;
		int buffer_count;

		size_t max_bytes_inflight;

		size_t max_storage_allocatd_bytes = 0;

		bool quiet;
	};

	template <class K, class V>
	struct KvPair {
		K key;
		V val;
	};
	template <class K, class V>
	struct KvPairSrc {
		K key;
		V val;
		int src;
	};

	class Status {
	public:
		Status();
		bool done_input;
		bool done_inmem;
		bool done_external;

		int external_count;
		int internal_count;

		int sorted_count;
		int file_count;
		File* done_file;
	private:
	};

	class uint128_t
	{
	private:
		uint64_t data[2] = {0};
	public:
		uint128_t(){}
		uint128_t(int d) {data[1] = d;}
		uint128_t(uint64_t d[2]) { data[0] = d[0]; data[1] = d[1];}
		friend std::ostream& operator << (std::ostream &out, const uint128_t &d) { 
			// std::bitset<64> t0 = d.data[0];
			// std::bitset<64> t1 = d.data[1];
			// out << t0 << t1;
			out << d.data[0] << " " << d.data[1]; return out; 
			return out;
		}

		friend bool operator < (const uint128_t &d1, const uint128_t &d2) {
			if (d1.data[0] < d2.data[0]) return true;
			else if (d1.data[0] > d2.data[0]) return false;
			else return (d1.data[1] < d2.data[1]);
		}

		friend bool operator > (const uint128_t &d1, const uint128_t &d2) {
			if (d1.data[0] > d2.data[0]) return true;
			else if (d1.data[0] < d2.data[0]) return false;
			else return (d1.data[1] > d2.data[1]);
		}

		friend bool operator == (const uint128_t &d1, const uint128_t &d2) {
			return (d1.data[0] == d2.data[0]) && (d1.data[1] == d2.data[1]);
		}

		uint128_t operator = (uint128_t other){
			this->data[0] = other.data[0];
			this->data[1] = other.data[1];
			return *this;
		}
	};

	class Count {
	private:
		uint32_t d[4] = {0};
	public:
		Count(){}
		Count(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4){
			d[0] = d1; d[1]  = d2; d[2] = d3; d[3] = d4;
		}
		Count(int l){ d[l] = 1; }

		Count operator = (Count other)
		{
			this->d[0] = other.d[0];
			this->d[1] = other.d[1];
			this->d[2] = other.d[2];
			this->d[3] = other.d[3];
			return *this;
		}
		friend std::ostream& operator << (std::ostream &out, const Count &data)
		{
			out << data.d[0] << " " <<  data.d[1] << " " << data.d[2] << " " << data.d[3];
			return out;
		}
		friend Count operator + (Count a, Count b)
		{
			return Count(a.d[0] + b.d[0], a.d[1] + b.d[1], a.d[2] + b.d[2], a.d[3] + b.d[3]);
		}
	};
}

#endif
