#compiler
CXX = cl
WARS = 
LNKS = 
INCS = /I inc /I ..\external\cpp\inc
DEFS = /D "NOMINMAX" /D "_USE_MATH_DEFINES"
CXXFLAGS = /nologo /std:c++20 /EHsc /c /openmp /diagnostics:caret $(DEFS) $(INCS) $(WARS)

LIBS = 

DLLS = 

#mode
ifneq ($(m), r)
	mode = debug
	LNKS += /debug
	CXXFLAGS += /Zi /FS
	CXXFLAGS += /D "_DEBUG" /Od /MDd /RTC1
	CXXFLAGS += /Fddist/windows/$(mode)/test.pdb
else
	mode = release
	CXXFLAGS += /D "NDEBUG" /O2 /MD
endif

#ouput
out = dist/windows/$(mode)/plate-ss.exe

#sources
dir := $(shell cd)
src := $(subst \,/,$(subst $(dir)\,,$(shell dir src /s /b /a:a)))

#objects
obj := $(subst src/,build/windows/$(mode)/,$(subst .cpp,.o,$(src)))

#dependencies
dep := $(subst .o,.d, $(obj))

#rules
all : $(out)
	@echo build($(mode)): complete!

run : $(out)
	@$(out)

debug : 
	@devenv /debugexe $(out)

setup :
	@if not exist dist\windows\$(mode) mkdir dist\windows\$(mode)

$(out) : setup $(DLLS) $(obj)
	@echo executable($(mode)): $@
	@link /nologo $(LNKS) /out:$(out) $(obj) $(LIBS)

build/windows/$(mode)/%.o : src/%.cpp build/windows/$(mode)/%.d
	@echo compiling($(mode)): $<
	@if exist $(subst /,\,$@) del $(subst /,\,$@)
	@if not exist $(subst /,\,$(dir $@)) mkdir $(subst /,\,$(dir $@))
	@$(CXX) $(CXXFLAGS) /Fo:$@ $< > nul
	@python msvc-dep.py $< $@

dist/windows/$(mode)/%.dll : ../external/cpp/dll/x64/%.dll
	@if not exist $(subst /,\,$(dir $@)) mkdir $(subst /,\,$(dir $@))
	@copy /y $(subst /,\,$<) $(subst /,\,$@) > nul

$(dep) : ;

-include $(dep)

clean :
	@if exist dist\windows\$(mode) rmdir /s /q dist\windows\$(mode)
	@if exist build\windows\$(mode) rmdir /s /q build\windows\$(mode)
	@echo clean($(mode)): complete!

print-% :
	@echo $* = $($*)

.PHONY : all run clean print-%