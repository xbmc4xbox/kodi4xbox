cmake_minimum_required(VERSION 3.18)
project(sqlite LANGUAGES CXX)

add_library(sqlite
	alter.c
	analyze.c
	attach.c
	auth.c
	btree.c
	build.c
	callback.c
	complete.c
	date.c
	delete.c
	expr.c
	func.c
	hash.c
	insert.c
	legacy.c
	lockfile.cpp
	main.c
	opcodes.c
	os_win.c
	pager.c
	parse.c
	pragma.c
	prepare.c
	printf.c
	random.c
	select.c
	table.c
	tclsqlite.c
	tokenize.c
	trigger.c
	update.c
	utf.c
	util.c
	vacuum.c
	vdbe.c
	vdbeapi.c
	vdbeaux.c
	vdbefifo.c
	vdbemem.c
	where.c
)

add_definitions(
	-D_XBOX
	-D_LIB
	-D_WIN32
	-DNXDK
	-DTHREADSAFE
	-DNO_TCL
	-DSQLITE_OMIT_DATETIME_FUNCS
	-DSQLITE_OMIT_AUTHORIZATION
	-DSQLITE_OMIT_AUTOVACUUM
	-DSQLITE_OMIT_MEMORYDB
	-DSQLITE_OMIT_INTEGRITY_CHECK
)

target_include_directories(sqlite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
