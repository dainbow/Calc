CXXFLAGS = -g -std=c++17 -Wall -Wextra -Weffc++ -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Waggressive-loop-optimizations -Walloc-zero -Walloca -Walloca-larger-than=8192 -Warray-bounds -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wdangling-else -Wduplicated-branches -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Wformat-overflow=2 -Wformat-truncation=2 -Winline -Wlarger-than=8192 -Wvla-larger-than=8192 -Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wrestrict -Wshadow -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wstringop-overflow=4 -Wsuggest-attribute=noreturn -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wnarrowing -Wno-old-style-cast -Wvarargs -Waligned-new -Walloc-size-larger-than=1073741824 -Walloc-zero -Walloca -Walloca-larger-than=8192 -Wcast-align -Wdangling-else -Wduplicated-branches -Wformat-overflow=2 -Wformat-truncation=2 -Wmissing-attributes -Wmultistatement-macros -Wrestrict -Wshadow=global -Wsuggest-attribute=malloc -fcheck-new -fsized-deallocation -fstack-check -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer 

SrcDir = src
BinDir = bin

Noname.out: $(BinDir)/Frontend.o $(BinDir)/Grammar.o $(BinDir)/TreeDiskUtilities.o $(BinDir)/Graph.o $(BinDir)/Tree.o $(BinDir)/Differ.o $(BinDir)/Text.o $(BinDir)/Utilities.o
	g++   	$(BinDir)/Frontend.o $(BinDir)/Grammar.o $(BinDir)/TreeDiskUtilities.o $(BinDir)/Graph.o $(BinDir)/Tree.o $(BinDir)/Differ.o $(BinDir)/Text.o $(BinDir)/Utilities.o -o Frontend/Noname.out

NoNoname.out:  $(BinDir)/Frontend-1.o $(BinDir)/Grammar.o $(BinDir)/TreeDiskUtilities.o $(BinDir)/Tree.o $(BinDir)/Differ.o $(BinDir)/Graph.o $(BinDir)/Stack.o $(BinDir)/Text.o $(BinDir)/Utilities.o
	g++ 	   $(BinDir)/Frontend-1.o $(BinDir)/Grammar.o $(BinDir)/TreeDiskUtilities.o $(BinDir)/Tree.o $(BinDir)/Differ.o $(BinDir)/Graph.o $(BinDir)/Stack.o $(BinDir)/Text.o $(BinDir)/Utilities.o -o Frontend/Noname-1.out

Backend.out:   $(BinDir)/X86_Backend.o $(BinDir)/x86.o $(BinDir)/Backend.o $(BinDir)/Compilation.o $(BinDir)/Tree.o $(BinDir)/Graph.o $(BinDir)/TreeDiskUtilities.o $(BinDir)/Stack.o $(BinDir)/Text.o $(BinDir)/Utilities.o $(BinDir)/Differ.o
	g++		   $(BinDir)/X86_Backend.o $(BinDir)/x86.o $(BinDir)/Backend.o $(BinDir)/Compilation.o $(BinDir)/Tree.o $(BinDir)/Graph.o $(BinDir)/Stack.o $(BinDir)/Text.o $(BinDir)/TreeDiskUtilities.o $(BinDir)/Utilities.o $(BinDir)/Differ.o -o Backend/Backend.out

Middleend.out: 	$(BinDir)/Middleend.o $(BinDir)/Differ.o $(BinDir)/Tree.o $(BinDir)/TreeDiskUtilities.o $(BinDir)/Graph.o $(BinDir)/Text.o $(BinDir)/Utilities.o
	g++			$(BinDir)/Middleend.o $(BinDir)/Differ.o $(BinDir)/Tree.o $(BinDir)/TreeDiskUtilities.o $(BinDir)/Graph.o $(BinDir)/Text.o $(BinDir)/Utilities.o -o Middleend/Middleend.out

$(BinDir)/x86.o: $(SrcDir)/Backend/x86.h $(SrcDir)/Backend/x86.cpp
	g++ -c	     $(SrcDir)/Backend/x86.cpp -o $(BinDir)/x86.o $(CXXFLAGS)

$(BinDir)/Middleend.o: 	$(SrcDir)/Middleend.cpp $(SrcDir)/Middleend/Middleend.h $(SrcDir)/Utilities/Text.h $(SrcDir)/Backend/Backend.h $(SrcDir)/Tree/TreeDiskUtilities.h $(SrcDir)/Tree/Tree.h $(SrcDir)/Tree/Differ.h
	g++ -c 				$(SrcDir)/Middleend.cpp -o $(BinDir)/Middleend.o $(CXXFLAGS)

$(BinDir)/Frontend-1.o: $(SrcDir)/Frontend-1.cpp $(SrcDir)/Frontend/Frontend-1.h $(SrcDir)/Frontend/Grammar.h $(SrcDir)/Backend/Backend.h $(SrcDir)/Tree/TreeDiskUtilities.h
	g++ -c				$(SrcDir)/Frontend-1.cpp -o $(BinDir)/Frontend-1.o $(CXXFLAGS)

$(BinDir)/Compilation.o: $(SrcDir)/Backend/compilation.cpp $(SrcDir)/Backend/Compilation.h $(SrcDir)/Utilities/Text.h $(SrcDir)/Utilities/Utilities.h $(SrcDir)/Backend/commands.h $(SrcDir)/Backend/cmd_def.h
	g++ -c 				 $(SrcDir)/Backend/compilation.cpp -o $(BinDir)/Compilation.o $(CXXFLAGS)

$(BinDir)/X86_Backend.o: $(SrcDir)/X86_Backend.cpp  $(SrcDir)/Backend/X86_Backend.h $(SrcDir)/Backend/Compilation.h $(SrcDir)/Tree/Tree.h $(SrcDir)/Tree/Graph.h $(SrcDir)/Tree/TreeDiskUtilities.cpp
	g++ -c 		       $(SrcDir)/X86_Backend.cpp -o $(BinDir)/X86_Backend.o $(CXXFLAGS)

$(BinDir)/Backend.o: $(SrcDir)/Backend.cpp  $(SrcDir)/Backend/Backend.h $(SrcDir)/Backend/X86_Backend.h $(SrcDir)/Backend/Compilation.h $(SrcDir)/Tree/Tree.h $(SrcDir)/Tree/Graph.h $(SrcDir)/Tree/TreeDiskUtilities.cpp
	g++ -c 		       $(SrcDir)/Backend.cpp -o $(BinDir)/Backend.o $(CXXFLAGS)

$(BinDir)/Grammar.o: $(SrcDir)/Frontend/Grammar.cpp $(SrcDir)/Frontend/Grammar.h $(SrcDir)/Utilities/Text.h $(SrcDir)/Tree/Tree.h $(SrcDir)/Tree/Graph.h $(SrcDir)/Utilities/Utilities.h
	g++ -c 		  	 $(SrcDir)/Frontend/Grammar.cpp -o $(BinDir)/Grammar.o $(CXXFLAGS)

$(BinDir)/Frontend.o: $(SrcDir)/Frontend.cpp $(SrcDir)/Frontend/Frontend.h $(SrcDir)/Frontend/Grammar.h $(SrcDir)/Tree/TreeDiskUtilities.h $(SrcDir)/Tree/Graph.h
	g++ -c 		      $(SrcDir)/Frontend.cpp -o $(BinDir)/Frontend.o $(CXXFLAGS)

$(BinDir)/Differ.o: $(SrcDir)/Tree/Differ.cpp $(SrcDir)/Tree/Differ.h $(SrcDir)/Tree/Tree.h $(SrcDir)/Tree/Graph.h $(SrcDir)/Utilities/Text.h $(SrcDir)/Utilities/Stack.h
	g++ -c          $(SrcDir)/Tree/Differ.cpp -o $(BinDir)/Differ.o $(CXXFLAGS)

$(BinDir)/Text.o: $(SrcDir)/Utilities/Text.cpp $(SrcDir)/Utilities/Text.h
	g++ -c        $(SrcDir)/Utilities/Text.cpp -o $(BinDir)/Text.o $(CXXFLAGS)

$(BinDir)/Utilities.o: $(SrcDir)/Utilities/Utilities.cpp $(SrcDir)/Utilities/Utilities.h
	g++ -c 			   $(SrcDir)/Utilities/Utilities.cpp -o $(BinDir)/Utilities.o $(CXXFLAGS)

$(BinDir)/TreeDiskUtilities.o: 	$(SrcDir)/Tree/TreeDiskUtilities.cpp $(SrcDir)/Tree/TreeDiskUtilities.h $(SrcDir)/Utilities/Text.h $(SrcDir)/Backend/Compilation.h
	g++	-c						$(SrcDir)/Tree/TreeDiskUtilities.cpp -o $(BinDir)/TreeDiskUtilities.o $(CXXFLAGS)

$(BinDir)/Tree.o: $(SrcDir)/Tree/Tree.cpp $(SrcDir)/Tree/Tree.h $(SrcDir)/Utilities/Text.h
	g++ -c 	      $(SrcDir)/Tree/Tree.cpp -o $(BinDir)/Tree.o $(CXXFLAGS)

$(BinDir)/Graph.o: $(SrcDir)/Tree/Graph.cpp $(SrcDir)/Tree/Graph.h $(SrcDir)/Tree/Tree.h
	g++ -c         $(SrcDir)/Tree/Graph.cpp -o $(BinDir)/Graph.o $(CXXFLAGS)

$(BinDir)/Stack.o: $(SrcDir)/Utilities/Stack.cpp $(SrcDir)/Utilities/Stack.h
	g++ -c $(SrcDir)/Utilities/Stack.cpp -o $(BinDir)/Stack.o $(CXXFLAGS)
