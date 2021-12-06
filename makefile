CXXFLAGS = -g -std=c++17 -Wall -Wextra -Weffc++ -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Waggressive-loop-optimizations -Walloc-zero -Walloca -Walloca-larger-than=8192 -Warray-bounds -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wdangling-else -Wduplicated-branches -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Wformat-overflow=2 -Wformat-truncation=2 -Winline -Wlarger-than=8192 -Wvla-larger-than=8192 -Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wrestrict -Wshadow -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wstringop-overflow=4 -Wsuggest-attribute=noreturn -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wnarrowing -Wno-old-style-cast -Wvarargs -Waligned-new -Walloc-size-larger-than=1073741824 -Walloc-zero -Walloca -Walloca-larger-than=8192 -Wcast-align -Wdangling-else -Wduplicated-branches -Wformat-overflow=2 -Wformat-truncation=2 -Wmissing-attributes -Wmultistatement-macros -Wrestrict -Wshadow=global -Wsuggest-attribute=malloc -fcheck-new -fsized-deallocation -fstack-check -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer 

SrcDir = src
BinDir = bin

Calc.exe: $(BinDir)/compilation.o $(BinDir)/Calc.o $(BinDir)/Codegen.o $(BinDir)/Grammar.o $(BinDir)/Text.o $(BinDir)/Utilities.o $(BinDir)/Stack.o $(BinDir)/Tree.o $(BinDir)/Graph.o $(BinDir)/Differ.o $(BinDir)/Latex.o
	g++   $(BinDir)/compilation.o $(BinDir)/Calc.o $(BinDir)/Codegen.o $(BinDir)/Grammar.o $(BinDir)/Text.o $(BinDir)/Utilities.o $(BinDir)/Stack.o $(BinDir)/Tree.o $(BinDir)/Graph.o $(BinDir)/Differ.o $(BinDir)/Latex.o -o Calc.exe

$(BinDir)/compilation.o: $(SrcDir)/compilation.cpp $(SrcDir)/Compilation.h $(SrcDir)/Text.h $(SrcDir)/Utilities.h $(SrcDir)/commands.h $(SrcDir)/cmd_def.h $(SrcDir)/stack.h
	g++ -c $(SrcDir)/compilation.cpp -o $(BinDir)/compilation.o $(CXXFLAGS)

$(BinDir)/Codegen.o: $(SrcDir)/Codegen.cpp  $(SrcDir)/Codegen.h $(SrcDir)/Grammar.h $(SrcDir)/Text.h $(SrcDir)/Tree.h $(SrcDir)/Graph.h
	g++ -c 		     $(SrcDir)/Codegen.cpp -o $(BinDir)/Codegen.o $(CXXFLAGS)

$(BinDir)/Grammar.o: $(SrcDir)/Grammar.cpp $(SrcDir)/Grammar.h $(SrcDir)/Text.h $(SrcDir)/Tree.h $(SrcDir)/Graph.h
	g++ -c 		  $(SrcDir)/Grammar.cpp -o $(BinDir)/Grammar.o $(CXXFLAGS)

$(BinDir)/Calc.o: $(SrcDir)/Calc.cpp $(SrcDir)/Calc.h $(SrcDir)/Text.h $(SrcDir)/Tree.h $(SrcDir)/Graph.h
	g++ -c 		  $(SrcDir)/Calc.cpp -o $(BinDir)/Calc.o $(CXXFLAGS)

$(BinDir)/Latex.o: $(SrcDir)/Latex.cpp $(SrcDir)/Latex.h
	g++ -c 		   $(SrcDir)/Latex.cpp -o $(BinDir)/Latex.o $(CXXFLAGS)

$(BinDir)/Differ.o: $(SrcDir)/Differ.cpp $(SrcDir)/Differ.h
	g++ -c          $(SrcDir)/Differ.cpp -o $(BinDir)/Differ.o $(CXXFLAGS)

$(BinDir)/Text.o: $(SrcDir)/Text.cpp $(SrcDir)/Text.h
	g++ -c        $(SrcDir)/Text.cpp -o $(BinDir)/Text.o $(CXXFLAGS)

$(BinDir)/Utilities.o: $(SrcDir)/Utilities.cpp $(SrcDir)/Utilities.h
	g++ -c 			   $(SrcDir)/Utilities.cpp -o $(BinDir)/Utilities.o $(CXXFLAGS)

$(BinDir)/Tree.o: $(SrcDir)/Tree.cpp $(SrcDir)/Tree.h
	g++ -c 	      $(SrcDir)/Tree.cpp -o $(BinDir)/Tree.o $(CXXFLAGS)

$(BinDir)/Graph.o: $(SrcDir)/Graph.cpp $(SrcDir)/Graph.h
	g++ -c         $(SrcDir)/Graph.cpp -o $(BinDir)/Graph.o $(CXXFLAGS)

$(BinDir)/Stack.o: $(SrcDir)/Stack.cpp $(SrcDir)/Stack.h
	g++ -c $(SrcDir)/Stack.cpp -o $(BinDir)/Stack.o $(CXXFLAGS)
