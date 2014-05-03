#include <stdexcept>

#include "simplefs.hpp"

using std::runtime_error;

/***** kelas Slot *****/

/* konstruktor */
Slot::Slot() : name(""), size(0){
	memset(content, 0, 100);
}

/* copy buffer ke content pada posisi ke-offset, sebanyak length */ 
void Slot::setContent(const char* buffer, int length, int offset){
	memcpy(content + offset, buffer, (size_t) length);
}

/* copy content ke buffer, mulai dari posisi ke-offset, sebanyak length */
void Slot::getContent(char* buffer, int length, int offset){
	memcpy(buffer, content + offset, (size_t) length);
}


/* tulis data slot ke file simple.fs */
void Slot::writeToFile(fstream& handle){

	// maksimum panjang nama file 26 karakter
	int fname_len = name.length() < 26 ? name.length() : 26;
	
	// inisialisasi buffer untuk penulisan slot
	char data[SLOT_SIZE];
	memset(data, 0, SLOT_SIZE);
	
	// set nama file
	memcpy(data, name.c_str(), fname_len);
	
	// set ukuran file
	data[27] = (unsigned char) size;
	
	// set isi file
	memcpy(data + 28, content, 100);
	
	// tulis slot ke simple.fs
	handle.write(data, SLOT_SIZE);
	handle.flush();
}

/* tulis baca data slot ke file simple.fs */
void Slot::readFromFile(fstream& handle){
	char buffer[SLOT_SIZE], strbuf[27];
	
	// baca 128 byte data ke buffer
	handle.read(buffer, SLOT_SIZE);
	
	// ambil string nama file dari buffer
	memcpy(strbuf, buffer, 27);
	name = string(strbuf);
	
	// ambil ukuran file dari buffer
	size = (unsigned char) buffer[27];
	
	// ambil isi file dari buffer
	setContent(buffer + 28, 100, 0);
}

/* cek apakah slot kosong */
bool Slot::isEmpty(){
	return name.length() == 0;
}

/***** kelas SimpleFS *****/

/* konstruktor */
SimpleFS::SimpleFS(){
	time(&mount_time);
}

/* destruktor */
SimpleFS::~SimpleFS(){
	handle.close();
}

/* buat file simple.fs baru */
void SimpleFS::create(string filename){
	
	/* buka file dengan mode input-output, binary dan truncate (untuk membuat file baru) */
	handle.open(filename.c_str(), fstream::in | fstream::out | fstream::binary | fstream::trunc);
	
	/* inisialisasi header */
	available = SLOT_NUM;
	firstEmpty = 0;
	writeHeader();
	
	/* inisialisasi slot dengan data kosong */
	char buffer[SLOT_SIZE];
	memset(buffer, 0, SLOT_SIZE);
	for (int i = 0; i < SLOT_NUM; i++){
		handle.write(buffer, SLOT_SIZE);
	}
	
	handle.flush();
}

/* baca file simple.fs */
void SimpleFS::load(string filename){

	/* buka file dengan mode input-output, dan binary */
	handle.open(filename.c_str(), fstream::in | fstream::out | fstream::binary);
	
	/* cek apakah file ada */
	if (!handle.is_open()){
		handle.close();
		throw runtime_error("File not found: " + filename);
	}
	
	char header[HEADER_SIZE];
	
	/* baca buffer header */
	handle.read(header, HEADER_SIZE);
	
	/* cek magic string */
	if (string(header, 6) != "SIMPLE"){
		handle.close();
		throw runtime_error("File not valid");
	}
	
	/* baca available dan firstEmpty */
	available = (unsigned char) header[6];
	firstEmpty = (unsigned char) header[7];
	
	/* baca seluruh slot */
	for (int i = 0; i < SLOT_NUM; i++){
		files[i].readFromFile(handle);
	}
}

/* tulis header filesystem ke file simple.fs */
void SimpleFS::writeHeader(){
	char buffer[HEADER_SIZE];
	
	/* tulis magic string ke buffer */
	memcpy(buffer, "SIMPLE", 6);
	
	/* tulis available dan firstEmpty ke buffer */
	buffer[6] = (unsigned char) available;
	buffer[7] = (unsigned char) firstEmpty;
	
	/* geser pointer penulisan file ke awal */
	handle.seekp(0);
	
	/* tulis buffer */
	handle.write(buffer, HEADER_SIZE);
	handle.flush();
}

/* tulis data slot ke-index pada file simple.fs di posisi yang sesuai */
void SimpleFS::writeFile(int index){

	/* hitung posisi awal penulisan slot */
	int offset = DATA_START + (index * SLOT_SIZE);
	
	/* geser pointer penulisan file sesuai offset */
	handle.seekp(offset);

	/* tulis data slot */
	files[index].writeToFile(handle);
}
