#The Target Binary Program
TARGET      := stegobmp

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
HEADERS     := include
BUILDDIR    := obj

#Flags, Libraries and Includes
LDFLAGS=#-fsanitize=address
CFLAGS=-std=c99 -Wall -pedantic -D_GNU_SOURCE #-Werror
CPPFLAGS := -I$(HEADERS)
LCRYPTO=-lcrypto

ifdef DEBUG
	CFLAGS += -g
endif

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -name *.c)
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.c=.o))



#Defauilt Make
all: $(TARGET)

debug:
	$(MAKE) all DEBUG=1

#Remake
remake: cleaner all

#Make the Directories
directories:
	@mkdir -p $(BUILDDIR)

#Clean only Objecst
clean:
	@$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -f $(TARGET)

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

#Link
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $^ $(LDFLAGS)

#Compile
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@) 
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $< $(LCRYPTO)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -MM $(SRCDIR)/$*.c > $(BUILDDIR)/$*.d
	@cp -f $(BUILDDIR)/$*.d $(BUILDDIR)/$*.d.tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.o:|' < $(BUILDDIR)/$*.d.tmp > $(BUILDDIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.d
	@rm -f $(BUILDDIR)/$*.d.tmp

embed: 
	./$(TARGET) -out out.bmp -p data/lado.bmp -embed -in data/logoITBA.png -steg LSB1
	@ls -lk data/logoITBA.png | cut -d " " -f5 | xargs printf "Hidden file data/logoITBA.png size is %ld bytes\n"
	@ls -lk out.bmp | cut -d " " -f5 | xargs printf "Embed file out.bmp size is %ld bytes\n"

extract:
	./$(TARGET) -out extractedOut -p out.bmp -extract -steg LSB1
	@ls -lk extractedOut.* | cut -d " " -f5,9 | xargs printf "The size is %ld bytes for the extracted file %s\n"
#Non-File Targets
.PHONY: all remake clean cleaner resources