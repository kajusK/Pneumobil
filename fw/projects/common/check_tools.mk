#
# code and style checking
#

STYLECHECK	:= $(TOOLS)/checkpatch.pl
STYLECHECK_FLAGS	:= --no-tree -f --terse --mailback
STYLECHECK_FILES	:= $(USRSRC) \
    $(shell find $(SOURCES) -name '*.h' | grep -v $(EXTERNAL) | grep -v $(SOURCES)/tests)

CODECHECK	:= cppcheck
CODECHECK_FLAGS := --enable=warning,performance -j 4 \
    $(addprefix -I, $(USRINC)) \
    --template=gcc --suppress=missingIncludeSystem \
    --suppress=unusedFunction --suppress=unmatchedSuppression \
    --suppress=missingInclude --platform=unix32 -q

.PHONY : stylecheck codecheck

codecheck:
	@$(CODECHECK) $(CODECHECK_FLAGS) $(STYLECHECK_FILES)

stylecheck:
	$(STYLECHECK) $(STYLECHECK_FLAGS) $(STYLECHECK_FILES)

POST_MAKE_ALL_RULE_HOOK: codecheck
