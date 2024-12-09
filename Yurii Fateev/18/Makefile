all:
	gcc 18.c -o compld
	mkdir testDir
	mkdir testDirReadExecOnly
	chmod 555 testDirReadExecOnly
	touch txtFileTest.txt
	touch readOnlyFile.txt
	chmod 444 readOnlyFile.txt
	touch writeOnlyFile.txt
	chmod 222 writeOnlyFile.txt
	touch executionOnlyFile.txt
	chmod 111 executionOnlyFile.txt
	ln -s txtFileTest.txt txtLinkTest
	./compld testDir testDirReadExecOnly txtFileTest.txt readOnlyFile.txt writeOnlyFile.txt executionOnlyFile.txt txtLinkTest