#ifndef SIMPLEFS_HPP
#define SIMPLEFS_HPP

// stdlib.h c
#include <cstdlib>

// string c, memcpy / memset
#include <cstring>

// string c++
#include <string>

// file
#include <fstream>

// time
#include <ctime>

using std::string; using std::fstream;

/** beberapa konstanta simple.fs **/

#define HEADER_SIZE 8
#define DATA_START HEADER_SIZE
#define SLOT_SIZE 128
#define SLOT_NUM 255

/**
	Slot
	Kelas yang memodelkan data slot pada simple.fs
*/
class Slot{
	public:
	
	string name; 		// nama file
	int size;			// ukuran file
	char content[100];	// isi file. karena total jumlah isi file tidak terlalu besar (sekitar 32 kB), maka bisa disimpan di memori
	
	/* konstruktor */
	Slot();
	
	/* copy buffer ke content pada posisi ke-offset, sebanyak length */  
	void setContent(const char* buffer, int length, int offset);
	
	/* copy content ke buffer, mulai dari posisi ke-offset, sebanyak length */
	void getContent(char* buffer, int length, int offset);
	
	/* tulis data slot ke file simple.fs */
	void writeToFile(fstream& handle);
	
	/* tulis baca data slot ke file simple.fs */
	void readFromFile(fstream& handle);
	
	/* cek apakah slot kosong */
	bool isEmpty();
	
};

/**
	SimpleFS
	Kelas yang memodelkan file simple.fs
*/
class SimpleFS{
	private:
	fstream handle;			// file simple.fs
	
	public:
	Slot files[SLOT_NUM]; 	// data seluruh slot file yang ada
	
	int available;			// jumlah slot yang masih kosong
	int firstEmpty;			// slot pertama yang masih kosong
	time_t mount_time;		// waktu mounting, diisi di konstruktor
	
	/* konstruktor & destruktor */
	SimpleFS();
	~SimpleFS();
	
	/* buat file simple.fs baru */
	void create(string filename);
	
	/* baca file simple.fs */
	void load(string filename);

	/* tulis header filesystem ke file simple.fs */
	void writeHeader();
	
	/* tulis data slot ke-index pada file simple.fs di posisi yang sesuai */
	void writeFile(int index);
	
	
};

#endif
