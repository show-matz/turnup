# make MAJOR_VER=0 MINOR_VER=800 DRAFT_LV=0 clean release-build

DEFINES = 

#DEFINES += -DKLUDGE_4_CYGWIN

# COMSPEC 環境変数が定義されていれば Windows(cygwin) 環境とみなす
ifdef COMSPEC
	TARGET_SUFFIX=.exe
	DEPLOYDIR = ../../../user/bin
#	DEPLOYDIR = /cygdrive/D/user/bin
	DEPLOYCMD = cp -p
else
	TARGET_SUFFIX =
	DEPLOYDIR = /usr/local/bin
	DEPLOYCMD = sudo cp -p
endif

# debug/release
ifeq ($(BUILD_MODE), debug)
	TARGETDIR = debug
	BASICOPTS = -g
	TARGET    = turnupDB
else
	TARGETDIR = release
	BASICOPTS = -O
	TARGET    = turnup
	DEFINES   += -DNDEBUG
endif

# バージョン番号関連のシンボル定義
#MEMO : DRAFT_LV は 0 だと正式版、それ以外はレベル表記になる
ifdef MAJOR_VER
	DEFINES += -DTURNUP_MAJOR_VERSION=$(MAJOR_VER)
endif
ifdef MINOR_VER
	DEFINES += -DTURNUP_MINOR_VERSION=$(MINOR_VER)
endif
ifdef DRAFT_LV
	DEFINES += -DTURNUP_DRAFT_VERSION=$(DRAFT_LV)
endif

CC = gcc
CXX = g++
MOREOPTS = -xCC -Xc
CFLAGS = $(BASICOPTS) $(MOREOPTS) $(DEFINES)
CXXFLAGS = $(BASICOPTS) $(DEFINES)

#LDLIBS=/usr/lib/libm.a
LDLIBS=

# 拡張関連の変数定義
EXT_INCS =
EXT_LIBS =
EXT_ARCS =
OBJS =

.SUFFIXES: .cxx .o .hxx .h

OBJS +=	$(TARGETDIR)/CRC64.o					\
		$(TARGETDIR)/File.o						\
		$(TARGETDIR)/Filters.o					\
		$(TARGETDIR)/FilterBuffer.o				\
		$(TARGETDIR)/TextSpan.o					\
		$(TARGETDIR)/TextMaker.o				\
		$(TARGETDIR)/InputFile.o				\
		$(TARGETDIR)/InputData.o				\
		$(TARGETDIR)/HtmlHeader.o				\
		$(TARGETDIR)/Config.o					\
		$(TARGETDIR)/ToC.o						\
		$(TARGETDIR)/Glossary.o					\
		$(TARGETDIR)/Footnotes.o				\
		$(TARGETDIR)/DocumentInfo.o				\
		$(TARGETDIR)/PreProcessor.o				\
		$(TARGETDIR)/InternalFilter.o			\
		$(TARGETDIR)/InternalFilter4Diff.o		\
		$(TARGETDIR)/InternalFilter4CandCxx.o	\
		$(TARGETDIR)/InternalFilter4CommonLISP.o	\
		$(TARGETDIR)/InternalFilter4Shell.o		\
		$(TARGETDIR)/InternalFilter4Raw.o		\
		$(TARGETDIR)/Operators.o				\
		$(TARGETDIR)/Operator4Comment.o			\
		$(TARGETDIR)/Operator4Details.o			\
		$(TARGETDIR)/Operator4EmbedFootnotes.o	\
		$(TARGETDIR)/Operator4EmbedPageBreak.o	\
		$(TARGETDIR)/Operator4EmbedTOC.o		\
		$(TARGETDIR)/Operator4EmptyLine.o		\
		$(TARGETDIR)/Operator4Error.o			\
		$(TARGETDIR)/Operator4FigureAndTable.o	\
		$(TARGETDIR)/Operator4Header.o			\
		$(TARGETDIR)/Operator4HorizontalLine.o	\
		$(TARGETDIR)/Operator4Image.o			\
		$(TARGETDIR)/Operator4NormalList.o		\
		$(TARGETDIR)/Operator4NumberedList.o	\
		$(TARGETDIR)/Operator4Paragraph.o		\
		$(TARGETDIR)/Operator4PreBlock.o		\
		$(TARGETDIR)/Operator4Quote.o			\
		$(TARGETDIR)/Operator4RawHTML.o			\
		$(TARGETDIR)/Operator4StylePalette.o	\
		$(TARGETDIR)/Operator4StyleStack.o		\
		$(TARGETDIR)/Operator4Table.o			\
		$(TARGETDIR)/Operator4TermDefine.o		\
		$(TARGETDIR)/StyleStack.o				\
		$(TARGETDIR)/StylePalette.o				\
		$(TARGETDIR)/Utilities.o				\
		$(TARGETDIR)/Parameters.o				\
		$(TARGETDIR)/main.o						\


all:
	make build
	make versionlog
	make TAGS

build:
	$(MAKE) BUILD_MODE=debug    build-imp
	$(MAKE) BUILD_MODE=release  build-imp

debug-build:
	$(MAKE) BUILD_MODE=debug    build-imp

release-build:
	$(MAKE) BUILD_MODE=release  build-imp

deploy:
	$(MAKE) BUILD_MODE=release  deploy-imp

clean:
	$(MAKE) BUILD_MODE=debug    clean-imp
	$(MAKE) BUILD_MODE=release  clean-imp

debug-clean:
	$(MAKE) BUILD_MODE=debug    clean-imp

release-clean:
	$(MAKE) BUILD_MODE=release  clean-imp

errchk: $(OBJS:.o=.chk)

$(TARGETDIR)/%.chk: %.cxx
	$(COMPILE.cc) -std=c++11 -Wall -Wextra -pedantic $(OPTINC) $(EXT_INCS) -fsyntax-only $<

versionlog:
	cd .. ; git log --pretty=oneline | perl -pe 's/^[a-z0-9]+ (.+)/\1/' > ./gitversion.log

TAGS: *.hxx *.cxx
	etags *.[hc]xx

depend:
	makedepend -Y -fmakefile `ls *.[hc]xx` 1> /dev/null 2> /dev/null

build-imp: $(TARGETDIR) $(TARGETDIR)/$(TARGET)$(TARGET_SUFFIX)

$(TARGETDIR)/$(TARGET)$(TARGET_SUFFIX): $(TARGETDIR) $(OBJS) $(EXT_ARCS)
	$(LINK.cc) -pthread $(OBJS) $(EXT_ARCS) $(EXT_LIBS) -o $@

# ソースファイルを .o ファイルにコンパイル
$(TARGETDIR)/%.o: %.cxx
	$(COMPILE.cc) -std=c++11 -Wall -Wextra -pedantic $< $(OPTINC) $(EXT_INCS) -o $@

# (必要な場合) ターゲットディレクトリを作成
$(TARGETDIR):
	mkdir -p $(TARGETDIR)


deploy-imp:
	strip $(TARGETDIR)/$(TARGET)$(TARGET_SUFFIX)
	$(DEPLOYCMD) $(TARGETDIR)/$(TARGET)$(TARGET_SUFFIX)  $(DEPLOYDIR)/$(TARGET)$(TARGET_SUFFIX)

clean-imp:
	rm -f $(TARGETDIR)/*.a
	rm -f $(TARGETDIR)/*.o
	rm -f $(TARGETDIR)/$(TARGET)$(TARGET_SUFFIX)
	rm -f *~



# DO NOT DELETE

