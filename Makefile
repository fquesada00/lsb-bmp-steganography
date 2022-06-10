#The Target Binary Program
TARGET      := stegobmp

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
HEADERS     := include
BUILDDIR    := obj

#Flags, Libraries and Includes
LDFLAGS=-fsanitize=address
CFLAGS=-std=c11 -Wall -g -pedantic
CPPFLAGS := -I$(HEADERS)

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -name *.c)
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.c=.o))

#Defauilt Make
all: $(TARGET)

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
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
	@$(CC) $(CFLAGS) $(CPPFLAGS) -MM $(SRCDIR)/$*.c > $(BUILDDIR)/$*.d
	@cp -f $(BUILDDIR)/$*.d $(BUILDDIR)/$*.d.tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.o:|' < $(BUILDDIR)/$*.d.tmp > $(BUILDDIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.d
	@rm -f $(BUILDDIR)/$*.d.tmp

#Non-File Targets
.PHONY: all remake clean cleaner resources