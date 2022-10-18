#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <map>
#include <bitset>
#include "sortreduce.h"
#include "types.h"
#include <fstream>

const char filename[12][100] = { "/mnt/hdd0/data/AlliumCepa/ERR5262394.fastq",
    "/mnt/hdd0/data/AlliumCepa/ERR5263020.fastq",
    "/mnt/hdd0/data/AlliumCepa/ERR5265251.fastq",
    "/mnt/hdd0/data/AlliumCepa/ERR5296222.fastq",
    "/mnt/hdd0/data/AlliumCepa/ERR5321933.fastq",
    "/mnt/hdd0/data/AlliumCepa/ERR5327657.fastq",
    "/mnt/hdd0/data/AlliumCepa/ERR5262488.fastq",
    "/mnt/hdd0/data/AlliumCepa/ERR5263190.fastq",
    "/mnt/hdd0/data/AlliumCepa/ERR5279463.fastq",
    "/mnt/hdd0/data/AlliumCepa/ERR5321879.fastq",
    "/mnt/hdd0/data/AlliumCepa/ERR5324469.fastq",
    "/mnt/hdd0/data/AlliumCepa/ERR5333199.fastq"
};

SortReduceTypes::Count update_function(SortReduceTypes::Count a, SortReduceTypes::Count b) {
	return (a + b);
}

std::map<char,int> acid = {{'A',0}, {'C',1}, {'G',2}, {'T', 3}};
int fl[4] = {0, 0, 1, 1};
int ll[4] = {0, 1, 0, 1};
int length = 126;
int m = 64;
int c = length - m;

int main(int argc, char** argv) {
	
	SortReduceTypes::Config<SortReduceTypes::uint128_t,SortReduceTypes::Count>* conf =
		new SortReduceTypes::Config<SortReduceTypes::uint128_t,SortReduceTypes::Count>("./", "out.sr", -1);
	conf->SetUpdateFunction(&update_function);
	conf->SetManagedBufferSize(1024*1024*8, 256); // 4 GiB
	SortReduce<SortReduceTypes::uint128_t,SortReduceTypes::Count>* sr = new SortReduce<SortReduceTypes::uint128_t,SortReduceTypes::Count>(conf);

	// Read files
	char * buf;
    FILE * fp = fopen(filename[0], "r");
    size_t len = 0;
    ssize_t read;
	uint64_t element_count = 0;
    if (fp == NULL){
        printf("no file");
        exit(EXIT_FAILURE);
    }
	read = getline(&buf, &len, fp);

	printf( "Started!\n" ); fflush(stdout);
	bool bits[252];
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	while ((read = getline(&buf, &len, fp)) != -1) {
		//encode
        for (int j = 0; j < 126; j++){
            int n = j*2;
            bits[n] = fl[acid[buf[j]]];
            bits[n+1] = ll[acid[buf[j]]];
        }
		char * tp = &buf[64];
		for (int i=0; i<c; i++){
			uint64_t d[2] = {0};
			int idx = 64; int addr = i*2;
			while(idx-- > 0 ){
				if(bits[addr]){ d[0] |= 1 << idx;}
				if(bits[(addr+64)]){ d[1] |= 1 << idx;}
				addr++;
			}
			SortReduceTypes::uint128_t key(d);
			while ( !sr->Update(key, SortReduceTypes::Count(acid[*tp])) ) {
			}
			tp++;
			element_count++;
		}
		getline(&buf, &len, fp);
        getline(&buf, &len, fp);
        getline(&buf, &len, fp);
		break;
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	sr->Finish();
	fflush(stdout);
	sleep(2); // If not it seg. faults
	fflush(stdout);

	std::cout << "Insertion = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
	std::cout << "Input done, added. total of " << element_count << " items.\n";
	fflush(stdout);

	begin = std::chrono::steady_clock::now();

	SortReduceTypes::Status status = sr->CheckStatus();
	while ( status.done_external == false ) {
		sleep(1);
		status = sr->CheckStatus();
		printf( "%s %s:%d-%d %s:%d-%d\n",
			status.done_input ? "yes":"no",
			status.done_inmem ? "yes":"no",
			status.internal_count, status.sorted_count,
			status.done_external ? "yes":"no",
			status.external_count, status.file_count);
		fflush(stdout);
	}
	printf( "All done!\n" );

	uint64_t total_count = 0;
	std::tuple<SortReduceTypes::uint128_t,SortReduceTypes::Count,bool> kvp = sr->Next();
	std::ofstream ofs("output");
	while ( std::get<2>(kvp) ) {
		SortReduceTypes::uint128_t key = std::get<0>(kvp);
		SortReduceTypes::Count val = std::get<1>(kvp);
	 	ofs << key << " " << val << std::endl;
		kvp = sr->Next();
	 	total_count++;
	}
	ofs.close();
	
	end = std::chrono::steady_clock::now();

	std::cout << "Others = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

	printf( "Total: %lu \n", total_count);
}
