
appSrcDir		=../app/src

all:usrapp
	
clean:usrappClean

debug:usrappdebug

usrapp:
	@cd $(appSrcDir) && echo "making $(appSrcDir)" && $(MAKE)
	@cp $(appSrcDir)/main .
usrappClean:
	@cd $(appSrcDir) && echo "making $(appSrcDir)" && $(MAKE) clean

usrappdebug:
	@cd $(appSrcDir) && echo "making $(appSrcDir)" && $(MAKE) debug

