#include <iostream>
#include "fuse_impl.hpp"

struct fuse_operations simple_op;
SimpleFS filesystem;

/* inisialisasi mapping fungsi-fungsi fuse */
void init_fuse(){
	simple_op.getattr	= simple_getattr;
	simple_op.open		= simple_open;
	simple_op.read		= simple_read;
	simple_op.write		= simple_write;
	simple_op.truncate	= simple_truncate;
	simple_op.rename	= simple_rename;
	simple_op.readdir	= simple_readdir;
	simple_op.mknod		= simple_mknod;
	simple_op.unlink	= simple_unlink;
}

using namespace std;

int main(int argc, char** argv){
	
	// jika terdapat argumen --new, buat baru
	if (argc > 2 && string(argv[2]) == "--new")
		filesystem.create("test.fs");
	else
		filesystem.load("test.fs");
		
	
	// inisialisasi fuse
	int fuse_argc = 2; 
	char* fuse_argv[2] = {argv[0], argv[1]};
	
	init_fuse();
	
	// serahkan ke fuse
	return fuse_main(fuse_argc, fuse_argv, &simple_op, NULL);
}
