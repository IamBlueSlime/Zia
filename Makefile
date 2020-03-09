##
## EPITECH PROJECT, 2019
## CPP_zia_2019
## File description:
## Makefile
##

ifneq ($(OS),Windows_NT)
	CMAKE_ARGS=	--no-print-directory
endif

.PHONY:		all conan debug tests_run coverage clean fclean re

all:		conan
			if [ ! -d  build ]; then mkdir build; fi
			cd build && cmake .. && cmake --build . -- ${CMAKE_ARGS}

conan:
			if [ ! -d build ]; then mkdir build; fi
			./scripts/setup_conan.sh build

debug:		conan
			if [ ! -d build ]; then mkdir build; fi
			cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && cmake --build . -- ${CMAKE_ARGS}

tests_run:
			if [ ! -d build ]; then mkdir build; fi
			cd build && cmake .. -DBUILD_TESTS=ON && cmake --build . -- ${CMAKE_ARGS} && make test ARGS="-V"

coverage:
			gcovr -r . --exclude-directories tests | tee coverage-litteral.txt
			gcovr -r . --exclude-directories tests --xml-pretty > coverage.xml

clean:
			rm -Rf build
			find . -name "*.gc*" -delete | true
			find . -name "report.xml" -delete | true
			find . -name "coverage.xml" -delete | true
			find . -name "coverage-litteral.txt" -delete | true

fclean:		clean
			rm -f zia
			rm -f modules/*.so
			rm -f modules/*.dll
			rm -f modules/*.dylib

re:			fclean all
