PROGRAM_NAME=unzip
TEST_ZIP_FILE=test.zip
OBJECT_FILE= deflate.o unzip.o zip_info.o
CONTENT_1_TXT='niha niha niha niwatorigairu'
CONTENT_2_TXT='hello'

CC=cc
CXX=g++


all:$(PROGRAM_NAME)

test:all
	echo $(CONTENT_1_TXT) > 1.txt
	echo $(CONTENT_2_TXT) > 2.txt
	zip $(TEST_ZIP_FILE) 1.txt 2.txt
	./$(PROGRAM_NAME) $(TEST_ZIP_FILE)
	(test $(CONTENT_1_TXT)  = "`cat 1.txt`" && test $(CONTENT_2_TXT)  = "`cat 2.txt`" && echo 'test pass.') || echo 'test not pass.'

clean:
	rm $(OBJECT_FILE)
	rm $(PROGRAM_NAME)
	rm $(TEST_ZIP_FILE)

$(PROGRAM_NAME):$(OBJECT_FILE)
	$(CC) -o $@ $^

.SUFFIXES :
.SUFFIXES : .cpp .c .o

.cpp.o:
	$(CXX) -c -o $@ $^

