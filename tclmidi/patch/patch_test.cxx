#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#if HAVE_UNISTD_H
#include <sys/types.h>
#include <unistd.h>
#endif

#include "GusPatch.h"

int
main(int argc, char *argv[])
{
	int fd;
	GusPatchFile p;
	ostrstream e;
	char *estr;

	if (argc != 2) {
		cerr <<"Usage: patch_test filename\n";
		exit(1);
	}

	fd = open(argv[1], O_RDONLY, 0);
	if (fd == -1) {
		cerr << "Couldn't open " << argv[1] << ": " <<
		    strerror(errno) << "\n";
		exit(1);
	}

	if (!p.Read(fd, e)) {
		estr = e.str();
		cerr << "Couldn't load patch: " << estr << "\n";
		delete estr;
		exit(1);
	}

	cout << p << endl;

	return (0);
}
